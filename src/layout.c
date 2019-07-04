/*
 *
 * (c) 2019 Marc van Kempen (marc@vankempen.com)
 * All rights reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE. *
 */

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <libusb.h>
#include "blusb.h"
#include "layout.h"
#include "usb.h"

/**
 * Layout is a type for an object that represents the keyboard layout in
 * up to 6 different layers.
 */

/**
 * Convert the layout matrix to a sequential byte array that can be used
 * by the bl_usb api. The data actually contains 16 bit numbers, so every two
 * bytes make up a number in little endian order. The first two bytes in the
 * array represent the number of layers, the rest of the data are the layers,
 * and for every layer, row per row the columns.
 *
 * @param layout The layout struct from which to get the matrix data.
 * @return Newly allocated array of bytes representing the matrix data, must
 *         be freed after use.
 */
uint8_t *
bl_layout_convert(bl_layout_t *layout) {
    uint8_t *data = (uint8_t *) malloc(sizeof(uint16_t) * (1 + layout->nlayers * NUMCOLS * NUMROWS));
    ((uint16_t *)data)[0] = layout->nlayers;
    for (int layer=0; layer<layout->nlayers; layer++) {
        for (int row=0; row<NUMROWS; row++) {
            for (int col=0; col<NUMCOLS; col++) {
                int n = layer * NUMROWS * NUMCOLS +
                    row * NUMCOLS +
                    col;
                ((uint16_t *)data)[n] = layout->matrix[layer][row][col];
            }
        }
    }

    return data;
}

/**
 * Create an empty layout struct. Set the number of layers, the
 * matrix array is allocated but left undefined.
 *
 * @param nlayers The number of layers to create in the struct
 */
bl_layout_t *
bl_layout_create(int nlayers) {
    bl_layout_t *layout = (bl_layout_t *) malloc( sizeof(bl_layout_t) );
    layout->nlayers = nlayers;

    return layout;
}

void
bl_layout_destroy(bl_layout_t *layout) {
    free(layout);
    return;
}

/**
 * Parse the file and return a layout struct. The memory for the layout is allocated and
 * must be freed after use.
 */
bl_layout_t*
bl_layout_load_file(char *fname) {
//char curdir[PATH_MAX];
//sprintf(curdir, "C:\\Users\\Marc van Kempen\\Documents\\src\\blusb\\build\\%s", fname);
//sprintf(curdir, "C:\\Users\\Marc van Kempen\\Documents\\src\\blusb\\build\\..\\layouts\\ibm_model_m_blusb_universal_iso.bin");
    FILE *f = fopen(fname, "r");
    if (f == NULL) {
        bl_tui_err(FALSE, "Could not open file %s\n", fname);
        return NULL;
    }

    /*
     * Format to be parsed is:
     *
     * LAYOUT = LAYERS
     * LAYERS = KEYS
     *        | KEYS '\n' LAYERS
     * KEYS = KEY ','
     *      | KEY
     * KEY = DIGIT+
     * DIGIT= [0-9]
     *
     */
    const int BL_STATE_DIGIT = 0;
    const int BL_STATE_WHITESPACE = 1;
    bl_layout_t *layout = bl_layout_create(NUMLAYERS_MAX);
    int layer = 0;
    int col = 0;
    int row = 0;
    int state = BL_STATE_WHITESPACE;
    char parse_buffer[20];
    parse_buffer[0] = 0;
    char ch = fgetc(f);
    while (!feof(f)) {
        if (state == BL_STATE_DIGIT) {
            if (isdigit(ch)) {
                if (strlen(parse_buffer) >= sizeof(parse_buffer) -1) {
                    printf("Error: ran out of buffer space for parsing, comma missing? Line %d, key %d, (byte position=%ld)\n", layer+1, col+1, ftell(f));
                    free(layout);
                    return NULL;
                } else {
                    int i = strlen(parse_buffer);
                    parse_buffer[i] = ch;
                    parse_buffer[i+1] = 0;
                }
            } else if (ch == ',') {
                uint16_t key = atoi(parse_buffer);
                layout->matrix[layer][row][col] = key;
                parse_buffer[0] = 0;
                if (col == NUMCOLS-1) {
                    col = 0;
                    row++;
                } else {
                    col++;
                }
                state = BL_STATE_WHITESPACE;
            } else if (ch == '\n' || ch == '\r') {
                layout->matrix[layer][row][col] = atoi(parse_buffer);
                parse_buffer[0] = 0;
                state = BL_STATE_WHITESPACE;
                if (col < NUMCOLS-1) {
                    printf("Invalid number of keys in row, actually %d, expected %d at line %d, key %d (byte position=%ld)\n", col+1, NUMCOLS, layer+1, col+1, ftell(f));
                    free(layout);
                    return NULL;
                }
                if (row < NUMROWS-1) {
                    printf("Invalid number of rows in layer, actually %d, expected %d at line %d, key %d (byte position=%ld)\n", row+1, NUMROWS, layer+1, col+1, ftell(f));
                    free(layout);
                    return NULL;
                }
                layer++;
                col = 0;
                row = 0;
            } else {
                printf("Unexpected character encountered while parsing digits: %c, at line %d, key %d (position=%ld)\n", ch, layer+1, col+1, ftell(f));
                free(layout);
                return NULL;
            }
            ch = fgetc(f);
        } else if (state == BL_STATE_WHITESPACE) {
            if (ch == '\n' || ch == '\r' || ch == ' ' || ch == '\t') {
                // ignore
                ch = fgetc(f);
            } else if (isdigit(ch)) {
                state = BL_STATE_DIGIT;
                // don't read the next character, we need the current character to be processed as a digit in the BL_STATE_DIGIT state.
            } else {
                printf("Unexpected character encountered while skipping whitespace: %c, at line %d, key %d (position=%ld)\n", ch, layer, col+row*col, ftell(f));
                free(layout);
                return NULL;
            }
        }
    }
    /*
     * We need to add the last key in the parse_buffer if there is one.
     */
    if (strlen(parse_buffer) > 0) {
        layout->matrix[layer][row][col] = atoi(parse_buffer);
    }
    layout->nlayers = layer;

    if ((row > 0 && row < NUMROWS-1) || (col > 0 && col < NUMCOLS-1)) {
        printf("Invalid layout file, not enough key entries for layer %d, actually %d, expected %d at line=%d\n",
               layer+1, 1+col+row*col, NUMROWS*NUMCOLS, layer+1);
        free(layout);
        fclose(f);
        return NULL;
    } else {
        fclose(f);
        return layout;
    }
}

/**
 * Pretty print the layout file
 */
void
bl_layout_print(bl_layout_t *layout) {
    printf("Number of layers: %d\n\n", layout->nlayers);
    for (int layer=0; layer<layout->nlayers; layer++) {
        printf("Layer %d\n\n", layer);
        printf("    ");
        for (int col=0; col<NUMCOLS; col++) {
            printf("C%-5u", col+1);
        }
        printf("\n");
        for (int row=0; row<NUMROWS; row++) {
            printf("R%d  ", row+1);
            for (int col=0; col<NUMCOLS; col++) {
                printf("%-6u", layout->matrix[layer][row][col]);
            }
            printf("\n");
        }
        printf("\n");
    }
}

/**
 * Write the bl_layout_t to the controller
 */
int
bl_layout_write(bl_layout_t *layout) {
    uint8_t *data = bl_layout_convert(layout);
    int ret = bl_usb_write_layout(data, layout->nlayers);
    free(data);
    return ret;
}

/**
 * Write the layout in the given file to the controller.
 *
 * returns TRUE if successfull, FALSE if not.
 */
int
bl_layout_write_from_file(char *fname) {
    bl_layout_t *layout = bl_layout_load_file(fname);
    int ret = bl_layout_write(layout);
    free(layout);

    return ret;
}

int
bl_layout_save(bl_layout_t *layout, char *fname) {
    FILE *f = fopen(fname, "w");
    if (f == NULL) {
        return -1;
    }
    uint8_t *buffer = bl_layout_convert(layout);
    bl_usb_raw_print_layout((uint16_t *)buffer, layout->nlayers, f);
    fclose(f);

    return 0;
}

