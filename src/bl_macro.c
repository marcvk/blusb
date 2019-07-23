#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "usb.h"
#include "bl_tui.h"

bl_macro_t *
bl_macro_parse(char *fname) {
    bl_macro_t *bm = (bl_macro_t *) malloc( sizeof(bl_macro_t) );
    /*
     * Initialize macro array
     */
    for (int i=0; i<NUM_MACROKEYS; i++) {
        for (int j=0; j<LEN_MACRO; j++) {
            bm->macros[i][j] = 0;
        }
    }

    int IN_NUMBER = 0;
    int IN_WHITESPACE = 1;

    FILE *f = fopen(fname, "r");
    int buflen = 8;
    char buf[buflen+1];
    memset(buf, 0, buflen+1);
    int i = 0;
    /*
     * macro_x is the x-th key for macro macro_y
     */
    int macro_x = 0;
    int macro_y = 0;
    /*
     * For error reporting, column and line in the macro file
     */
    int col = 0;
    int line = 0;


    int state = IN_NUMBER;
    int ch = fgetc(f);
    while (!feof(f)) {
        if (state == IN_NUMBER) {
            if (isdigit(ch)) {
                if (i < buflen) {
                    buf[i++] = ch;
                    ch = fgetc(f);
                    col++;
                } else {
                    bl_tui_err(FALSE, "buffer overflow while reading macro digits, at line %d, col %d", line, col);
                    free(bm);
                    return NULL;
                }
            } else {
                /*
                * Try reading a number from buffer
                */
                if (i > 0) {
                    bm->macros[macro_y][macro_x++] = atoi(buf);
                    bm->nmacros = macro_y+1;
                    memset(buf, 0, buflen+1);
                    i = 0;
                }
                if (ch == '\n' || ch == '\r') {
                    macro_x = 0;
                    macro_y++;
                }
                if (ch == ',') {
                    ch = fgetc(f);
                }
                state = IN_WHITESPACE;
            }
        } else if (state == IN_WHITESPACE) {
            if (ch == '\n' || ch == '\r') {
                // ignore
                ch = fgetc(f);
                col = 0;
                line++;
            } else if  (ch == ' ' || ch == '\t') {
                ch = fgetc(f);
                col++;
            } else if (isdigit(ch)) {
                state = IN_NUMBER;
                // don't read the next character, we need the current character to be processed as a digit in the IN_NUMBER state.
            } else {
                bl_tui_err(FALSE, "Unexpected character encountered while skipping whitespace: [%c], at line %d, column %d.\n", ch, line+1, col+1);
                free(bm);
                return NULL;
            }

        }
    }

    return bm;
}

int
bl_macro_navigate() {
    clear();
    refresh();

    usleep(1000000);

    return 'q';
}
