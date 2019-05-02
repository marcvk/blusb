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
    bl_usb_enable_service_mode();
    bl_layout_configure(0, NULL);
    bl_usb_disable_service_mode();
}

/*
 * Read the current layout from the controller and output the result in a machine readable format.
 */
void
bl_read_layout() {
    unsigned char *buffer;
    int nlayers;
    bl_usb_read_layout(&buffer, &nlayers);
    bl_usb_raw_print_layout((uint16_t *)buffer, nlayers);
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
    bl_layout_write(fname);
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
}

void
bl_write_macros(char *fname) {

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
    printf("When executing without arguments it will enter in a text UI mode.\n");
    printf("If no options are passed, but a filename is supplied, the programm will start\n");
    printf("in UI mode and load the file as a layout file to be edited.\n");
    printf("\n");
    printf("Options:");
    printf("\n");
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
        } else if (strcmp(argv[1], "-write-layout") == 0 && argc == 3) {
            BL_EXEC(bl_write_layout(argv[2]));
        } else if (strcmp(argv[1], "-read-pwm") == 0) {
            BL_EXEC(bl_read_pwm());
        } else if (strcmp(argv[1], "-write-pwm") == 0 && argc == 4) {
            BL_EXEC(bl_write_pwm(argv[2], argv[3]));
        } else if (strcmp(argv[1], "-read-debounce") == 0) {
            BL_EXEC(bl_read_debounce());
        } else if (strcmp(argv[1], "-write-debounce") == 0 && argc == 3) {
            BL_EXEC(bl_write_debounce(argv[2]));
        } else if (strcmp(argv[1], "-read-macros") == 0) {
            BL_EXEC(bl_read_macros());
        } else if (strcmp(argv[1], "-write-macros") == 0 && argc == 3) {
            BL_EXEC(bl_write_macros(argv[2]));
        } else if (strcmp(argv[1], "-v") == 0) {
            BL_EXEC(bl_print_version());
        } else if (strcmp(argv[1], "-h") == 0) {
            bl_print_usage(argv);
        } else {
            bl_print_usage(argv);
        }
    } else {
        BL_EXEC(bl_ui());
    }
}

