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

// restrict direct access to handle
static libusb_device_handle *handle = NULL;

// IBM Enhanced Performance Keyboard identifiers
const uint16_t vendor = 0x04b3;
const uint16_t product = 0x301c;

/**
 * Functions to find, open and close access to the controller via usb
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
    libusb_device **dev_list;

    libusb_init(NULL);

    // locate device
    uint8_t cnt = libusb_get_device_list(NULL, &dev_list);
    int err = FALSE;
    for (uint8_t i = 0; i<cnt && handle == NULL; i++) {
        libusb_device *dev = dev_list[i];
        struct libusb_device_descriptor dev_descr;

        libusb_get_device_descriptor(dev, &dev_descr);
        if ((vendor == dev_descr.idVendor) && (product == dev_descr.idProduct)) {
            if (libusb_open(dev, &handle)) {
                printf("\n\n");
                printf("LIBUSB error code: %s", libusb_error_name(err));
                printf("\n\n");
#ifdef _WIN32
                printf
                    (
                        "Don't panic! This is a simple driver issue. "
                        "If you have not already, download Zadig at\n"
                        "http://zadig.akeo.ie and install the WinUSB driver.\n"
                        "You can also give the LibUSB-win32 driver a try, whatever is going to work for you.\n"
                        "Quirky Windows(c) likes a little tinkering!\n"
                        );
#else
                printf("Could not open the usb device, do you have the right permissions?\n");
                printf("You could try running with sudo.\n");
#endif
            } else {
                err = TRUE;
            }
        }
    }
    libusb_free_device_list(dev_list, 1);

    if (handle == NULL) {
        printf("Could not find keyboard\n");
        return FALSE;
    } else {
        return TRUE;
    }
}

/**
 * Close the handle for the controller
 */
void
bl_usb_closectrl() {
    libusb_close(handle);
    handle = NULL;
    libusb_exit(NULL);
}

/*
 * Enable the service mode, necessary to be able to read the matrix position using
 * the firmware.
 */
void
bl_usb_enable_service_mode() {
    libusb_control_transfer(handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_ENABLE_VENDOR_RQ, 0, 0, 0, 0, 1000);
}

void
bl_usb_disable_service_mode() {
    libusb_control_transfer(handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_DISABLE_VENDOR_RQ, 0, 0, 0, 0, 1000);
}

void
bl_usb_read_matrix_pos_raw(int *row, int *col) {
    static uint8_t buffer[8] = { 0 };

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_IN |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_READ_MATRIX, 0, 0, buffer, sizeof(buffer), 1000);

    *row = buffer[0];
    *col = buffer[1];

    return;
}

void
bl_usb_enable_service_mode_safe() {
    bl_usb_enable_service_mode();
    int row = -1;
    int col = -1;
    while (row != 0 && col != 0) {
        bl_usb_read_matrix_pos_raw(&row, &col);
    }
    bl_usb_disable_service_mode();
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
    static uint8_t buffer[8] = { 0 };
    static uint8_t buffer_last[8] = { 0 };
    static uint8_t buffer_changed = 0;
    uint8_t i = 0;

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_IN |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_READ_MATRIX, 0, 0, buffer, sizeof(buffer), 1000);

    if (buffer[7]) {
        for (i = 0; i < 2; i++) {
            if (buffer[i] ^ buffer_last[i]) {
                buffer_changed = TRUE;
            }
        }
    }

    if (buffer_changed) {
        buffer_changed = FALSE;
        for (i = 0; i < 2; i++) {
            buffer_last[i] = buffer[i];
        }
        *row = buffer[0];
        *col = buffer[1];
        return TRUE;
    } else {
        return FALSE;
    }
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

    memset(uc_buffer, 0, buf_size);
    libusb_control_transfer(handle, LIBUSB_RECIPIENT_ENDPOINT | LIBUSB_ENDPOINT_IN |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_READ_LAYOUT, 0, 0, uc_buffer, buf_size, BL_USB_TIMEOUT);

    *nlayers = uc_buffer[0];

    if (*nlayers == 0){
        printf("No layers configured.\n");
        return FALSE;
    }

    if (*nlayers > 6) {
        printf("More than 6 layers reported, bad flash value!\n");
        return FALSE;
    }

    *buffer = (uint8_t *) malloc(buf_size-2);
    *buffer = memcpy(*buffer, uc_buffer+2, buf_size-2);

    return TRUE;
}

int
bl_usb_write_layout(uint8_t *layout, int nlayers) {
    enum { buf_size = 2048 };
    unsigned char uc_buffer[buf_size];

    uc_buffer[0] = nlayers;
    memcpy(&uc_buffer[1], layout, 2 * nlayers * NUMCOLS * NUMROWS);

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_ENDPOINT | LIBUSB_ENDPOINT_OUT | \
        LIBUSB_REQUEST_TYPE_VENDOR, USB_WRITE_LAYOUT, 0, 0, uc_buffer, nlayers*2*NUMKEYS+1, 1000);

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

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_IN |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_READ_VERSION, 0, 0, buffer, sizeof(buffer), 1000);

    *major = buffer[0];
    *minor = buffer[1];
}

void
bl_usb_pwm_read(uint8_t *pwm_usb, uint8_t *pwm_bt) {
    uint8_t buffer[8];

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_IN |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_READ_BR, 0, 0, buffer, sizeof(buffer), 1000);

    *pwm_usb = buffer[0];
    *pwm_bt = buffer[1];

    return;
}

void
bl_usb_pwm_write(uint8_t pwm_usb, uint8_t pwm_bt) {
    static uint8_t buffer[8] = { 0 };

    if ((pwm_usb < 0 || pwm_usb > 255) || (pwm_bt < 0 || pwm_bt > 255))
    {
        printf("Value out of range, no changes applied. Valid range is between 0 and 255 (inclusive).\n");
        exit(0);
    }

    buffer[0] = (uint8_t)pwm_usb;
    buffer[1] = (uint8_t)pwm_bt;

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_WRITE_BR, 0, 0, buffer, sizeof(buffer), 1000);
}


uint8_t
bl_usb_debounce_read() {
    uint8_t buffer[8] = { 0 };

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_IN |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_READ_DEBOUNCE, 0, 0, buffer, sizeof(buffer), 1000);

    return buffer[0];
}

void
bl_usb_debounce_write(uint8_t debounce) {
    uint8_t buffer[8] = { 0 };

    if ((debounce < 1 || debounce > 255)) {
        printf("Value out of range, no changes applied. Exiting.\n");
        return;
    }

    buffer[0] = (uint8_t)debounce;

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_INTERFACE | LIBUSB_ENDPOINT_OUT |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_WRITE_DEBOUNCE, 0, 0, buffer, sizeof(buffer), 1000);
}

void
bl_usb_macro_print(bl_macro_t *bm) {
    uint8_t macro_cnt = 0;
/*
    printf("Macro key table\n\n");
    printf("            Mods%-3cRsvd%-3cKey1%-3cKey2%-3cKey3%-3cKey4%-3cKey5%-3cKey6\n\n", '\0', '\0', '\0', '\0', '\0', '\0', '\0');
    printf("Macro %-6u", ++macro_cnt);

    for (uint8_t i = 0; i < sizeof(char_ctr_buf); i++)
    {
        printf("%-7u", char_ctr_buf[i]);
        if(i && i != sizeof(char_ctr_buf) - 1)
            if ((i + 1) % 8 == 0)
            {
                printf("\n");
                printf("Macro %-6u", ++macro_cnt);
            }
    }
    printf("\n");
*/

    printf("Macro key table\n\n");
    printf("            Mods%-3cRsvd%-3cKey1%-3cKey2%-3cKey3%-3cKey4%-3cKey5%-3cKey6\n\n", '\0', '\0', '\0', '\0', '\0', '\0', '\0');

    for (int i=0; i<NUM_MACROKEYS; i++) {
        printf("Marco %-6u", i);
        for (int j=0; j<LEN_MACRO; j++) {
            printf("%-7u", bm->macros[i][j]);
        }
        printf("\n");
    }

}

bl_macro_t*
bl_usb_macro_read() {
    unsigned char char_ctr_buf[192];
    uint8_t bad_value1 = 0;
    uint8_t bad_value2 = 0;

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_ENDPOINT | LIBUSB_ENDPOINT_IN |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_READ_MACROS, 0, 0, char_ctr_buf, sizeof(char_ctr_buf), 1000);

    for (uint8_t i = 0; i < sizeof(char_ctr_buf); i++) {
        if (char_ctr_buf[i] == 0) bad_value1++;
        if (char_ctr_buf[i] == 255) bad_value2++;
    }

    if (bad_value1 == sizeof(char_ctr_buf) || bad_value2 == sizeof(char_ctr_buf)) {
        printf("Bad EEPROM value!\n");
        exit(0);
    }

    bl_macro_t *bm = (bl_macro_t *) malloc(sizeof(bl_macro_t));
    bm->nmacros = 24;
    memcpy(bm->macros, char_ctr_buf, sizeof(char_ctr_buf));

    return bm;
}

void
bl_usb_macro_write(bl_macro_t* macros)
{
    libusb_control_transfer(handle, LIBUSB_RECIPIENT_ENDPOINT | LIBUSB_ENDPOINT_OUT | \
        LIBUSB_REQUEST_TYPE_VENDOR, USB_WRITE_MACROS, 0, 0, (unsigned char *) macros->macros, NUM_MACROKEYS*LEN_MACRO, 1000);
}
