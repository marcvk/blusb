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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "blusb.h"
#include "usb.h"
#include "layout.h"

/*
 * Start the interactive text ui to configure the keyboard layout, macros, etc.
 */
void
bl_ui() {
    bl_layout_configure(NULL);
}

/*
 * Load the file and start the text ui
 */
void
bl_ui_load_file(char *fname) {
    bl_layout_t *layout = bl_layout_load_file(fname);
    bl_layout_configure(layout);
}

/*
 * Read the current layout from the controller and output the result in a machine readable format.
 */
void
bl_read_layout() {
    unsigned char *buffer;
    int nlayers;
    bl_usb_read_layout(&buffer, &nlayers);
    bl_usb_raw_print_layout((uint16_t *)buffer, nlayers, stdout);
    free(buffer);
}

/*
 * Print the current layout in a human friendly format
 */
void
bl_print_layout() {
    unsigned char *buffer;
    int nlayers;
    bl_usb_read_layout(&buffer, &nlayers);
    bl_usb_print_layout(buffer, nlayers);
    free(buffer);
}

void
bl_write_layout(char *fname) {
    bl_layout_write_from_file(fname);
}

/*
 * Read the current USB en BT, used to set the
 * brightness of the LEDs.
 *
 * The PWM stands for Pulse Width Modulation,
 * a technique used in digital controllers
 * to dimm an LED.
 */
void
bl_read_pwm() {
    uint8_t pwm_usb;
    uint8_t pwm_bt;
    bl_usb_pwm_read(&pwm_usb, &pwm_bt);
    printf("%d, %d\n", pwm_usb, pwm_bt);
}

void
bl_write_pwm(char *usb_val, char *bt_val) {
    int pwm_usb = atoi(usb_val);
    int pwm_bt = atoi(bt_val);

    bl_usb_pwm_write(pwm_usb, pwm_bt);
}

/*
 * Read the debounce parameter. This is a time span in which the keyboard's
 * physical contacts are allowed to settle in order to register a single click instead of
 * (possibly) multiple, the valid range is 1-255, the recommended value is 15ms.
 */
void
bl_read_debounce() {
    printf("%d\n", bl_usb_debounce_read());
}

/*
 * Write the debounce value to the controller.
 */
void
bl_write_debounce(char *debounce) {
    bl_usb_debounce_write(atoi(debounce));
}

/*
 * Read the currently defined macros
 */
void
bl_read_macros() {
    bl_macro_t *macros = bl_usb_macro_read();
    bl_usb_macro_print(macros);
}

void
bl_write_macros(char *fname) {
    bl_macro_t *bm = bl_macro_parse(fname);
    if (bm != NULL) {
        printf("%d macros found\n", bm->nmacros);
        for (int i=0; i<bm->nmacros; i++) {
            for (int j=0; j<LEN_MACRO && bm->macros[i][j] != 0; j++) {
                printf("%d ", bm->macros[i][j]);
            }
            printf("\n");
        }
    } else {
        printf("Error reading macro file\n");
    }
    bl_usb_macro_write(bm);
}

/*
 * Print the version of the firmware and this software's version.
 */
void
bl_print_version() {
    int major, minor;

    bl_usb_read_version(&major, &minor);
    // TODO print software version
    printf("Firmware Version: %d.%d\n", major, minor);
}

void
bl_print_usage(char **argv) {
    // TODO add more documentation.
    printf("\n");
    printf("Usage: %s [-option] [-optional parameter] [filename]\n", argv[0]);
    printf("\n");
    printf("Options:");
    printf("\n");
    printf("  -ui [filename]                   Start in interactive  UI mode, if a layout filename is supplied\n");
    printf("                                   the file is loaded first.\n");
    printf("  -read-pwm                        Read the pwm value from the controller.\n");
    printf("  -write-pwm [value_USB value_BT]  Write the pwm value to the controller, valid\n");
    printf("                                   range: 0-255\n");
    printf("  -read-debounce                   Read the debounce value from the controller.\n");
    printf("  -write-debounce [value]          Write the debounce value to the controller.\n");
    printf("                                   Recommended value is 15 (ms), valid range: 1-255\n");
    printf("  -read-macros                     Print the macros in parseable format.\n");
    printf("  -write-macros [filename]         Write the macros to the controller.\n");
    printf("  -print-layout                    Pretty print the layout.\n");
    printf("  -read-layout                     Print the layout in parseable format\n");
    printf("  -write-layout [filename]         Write the layout to the controller.\n");
    printf("  -v                               Print the version.\n");
    printf("  -h                               This help text.\n");
}

/*
 * Open a controller connection and execute the statement
 */
 #define BL_EXEC(stmt) {\
    if (bl_usb_openctrl()) {\
        stmt;\
        bl_usb_closectrl();\
    }\
}\

int
main(int argc, char **argv) {
    if (argc >= 2) {
        if (strcmp(argv[1], "-read-layout") == 0) {
            BL_EXEC(bl_read_layout());
        } else if (strcmp(argv[1], "-print-layout") == 0) {
            BL_EXEC(bl_print_layout());
        } else if (strcmp(argv[1], "-write-layout") == 0) {
            if (argc == 3) {
                BL_EXEC(bl_write_layout(argv[2]));
            } else {
                bl_print_usage(argv);
            }
        } else if (strcmp(argv[1], "-read-pwm") == 0) {
            BL_EXEC(bl_read_pwm());
        } else if (strcmp(argv[1], "-write-pwm") == 0) {
            if (argc == 4) {
                BL_EXEC(bl_write_pwm(argv[2], argv[3]));
            } else {
                printf("missing parameter\n");
                bl_print_usage(argv);
            }
        } else if (strcmp(argv[1], "-read-debounce") == 0) {
            BL_EXEC(bl_read_debounce());
        } else if (strcmp(argv[1], "-write-debounce") == 0) {
            if (argc == 3) {
                BL_EXEC(bl_write_debounce(argv[2]));
            } else {
                printf("missing parameter\n");
                bl_print_usage(argv);
            }
        } else if (strcmp(argv[1], "-read-macros") == 0) {
            BL_EXEC(bl_read_macros());
        } else if (strcmp(argv[1], "-write-macros") == 0) {
            if (argc == 3) {
                BL_EXEC(bl_write_macros(argv[2]));
            } else {
                printf("missing parameter\n");
                bl_print_usage(argv);
            }
        } else if (strcmp(argv[1], "-v") == 0) {
            BL_EXEC(bl_print_version());
        } else if (strcmp(argv[1], "-h") == 0) {
            bl_print_usage(argv);
        } else if (strcmp(argv[1], "-ui") == 0) {
            if (argc == 3) {
                BL_EXEC(bl_ui_load_file(argv[2]));
            } else {
                BL_EXEC(bl_ui());
            }
        } else {
            printf("unknown option\n");
            bl_print_usage(argv);
        }
    } else {
        bl_print_usage(argv);
    }
}

