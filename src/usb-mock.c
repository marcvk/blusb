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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <libusb.h>

#include "blusb.h"
#include "layout.h"
#include "usb.h"

/**
 * Mock implementation of usb functions, useful to test TUI on 
 * Windows.
 */


/**
 * Try to locate the controller, if it's not found return FALSE,
 * else return TRUE.
 *
 * Use bl_usb_closectrl() to close access to the controller before
 * exiting.
 *
 * FIXME distinguish between keyboard not found and could not open
 * keyboard (error).
 */
int
bl_usb_openctrl() {
    return TRUE;
}

/**
 * Close the handle for the controller
 */
void
bl_usb_closectrl() {}

/*
 * Enable the service mode, necessary to be able to read the matrix position using
 * the firmware.
 */
void
bl_usb_enable_service_mode() { }

void
bl_usb_disable_service_mode() { }

void
bl_usb_read_matrix_pos_raw(int *row, int *col) {
    *row = 0;
    *col = 0;

    return;
}

void
bl_usb_enable_service_mode_safe() {
    bl_usb_enable_service_mode();
}

/**
 * Read the matrix position of the last key pressed, returns TRUE and sets
 * row and col to the valid row and column of the key pressed in the matrix.
 * If no key was pressed it returns FALSE.
 *
 * @param row If a key was pressed, will be set to the row in the matrix of that key, else undefined.
 * @param col If a key was pressed, will be set to the column in the matrix of that key, else undefined.
 * @return TRUE if a key was pressed, FALSE otherwise
 */
int
bl_usb_read_matrix_pos(int *row, int *col)
{
    *row = 0;
    *col = 0;
    return FALSE;
}

/**
 * Read the layout from the controller, return the raw data. The raw data consists of 16 bit
 * numbers, length of data returned is nlayers * 2 bytes * NUMCOLS * NUMROWS, order
 * is little endian.
 *
 * FIXME How much data does the call return? Is it max 2048 bytes (that's what was in the
 * original code), or is it actually 2 * (1 + NUMLAYERS_MAX * NUMROWS * NUMCOLS), which is
 * 1922 bytes.
 *
 * @param buffer Pointer to unallocated buffer, the memory will be allocated for you
 *          and must be freed after use.
 * @param nlayers Address to number of layers, will be set to the number of layers
 *          found.
 * @return returns TRUE if successful, FALSE if not.
 */
int
bl_usb_read_layout(uint8_t **buffer, int *nlayers) {
    enum { buf_size = 2048 };
    unsigned char uc_buffer[buf_size];

    *nlayers = 1;
    *buffer = (uint8_t *) malloc(buf_size-2);
    memset(*buffer, 0, buf_size-2);

    return TRUE;
}

int
bl_usb_write_layout(uint8_t *layout, int nlayers) {
    return TRUE;
}

/**
 * Print machine parseable output to the given file stream.
 *
 * @param buffer Buffer containing the configuration
 * @param nlayers Number of layers
 * @param f File stream to write to
 */
void
bl_usb_raw_print_layout(uint16_t *buffer, int nlayers, FILE *f) {
    for (int layer=0; layer<nlayers; layer++) {
        for (int row=0; row<NUMROWS; row++) {
            for (int col=0; col<NUMCOLS; col++) {
                int index = layer * NUMROWS * NUMCOLS + row * NUMCOLS + col;
                if (col == NUMCOLS - 1 && row == NUMROWS - 1) {
                    fprintf(f, "%u\n", buffer[index]);
                } else {
                    fprintf(f, "%u, ", buffer[index]);
                }
            }
        }
    }
}

void
bl_usb_print_layout(uint8_t *buffer, int nlayers) {
    int layer, row, col;

    printf("\n");
    printf("Number of layers: %d\n", nlayers);

    for (layer=0; layer<nlayers; layer++) {
        printf("\n");
        printf("Layer %d\n", layer);
        printf("\n");
        printf("    ");
        for (col=0; col<NUMCOLS; col++) {
            printf("C%-5u", col+1);
        }
        printf("\n\n");
        for (row=0; row<NUMROWS; row++) {
            printf("R%d  ", row+1);
            for (col=0; col<NUMCOLS; col++) {
                printf("%-6d", *((uint16_t*)buffer + layer*NUMKEYS + col + row*NUMCOLS));
            }
            printf("\n");
        }
    }

    return;
}

void
bl_usb_read_version(int *major, int *minor) {
    uint8_t buffer[8];

    *major = 1;
    *minor = 0;
}

void
bl_usb_pwm_read(uint8_t *pwm_usb, uint8_t *pwm_bt) {
    *pwm_usb = 0;
    *pwm_bt = 0;

    return;
}

void
bl_usb_pwm_write(uint8_t pwm_usb, uint8_t pwm_bt) { }


uint8_t
bl_usb_debounce_read() {
    return 0;
}

void
bl_usb_debounce_write(uint8_t debounce) { }

bl_macro_t*
bl_usb_macro_read() {
    return NULL;
}

