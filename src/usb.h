#include "layout.h"

#ifndef __USB_H_
#define __USB_H_

typedef uint16_t bl_matrix_t[NUMLAYERS_MAX][NUMROWS][NUMCOLS];

typedef struct bl_layout_t {
    int nlayers;
    bl_matrix_t matrix;
} bl_layout_t;

typedef uint16_t bl_macro_keylist_t[NUM_MACROKEYS][LEN_MACRO];

typedef struct bl_macro_t {
    int nmacros;
    bl_macro_keylist_t macros;
} bl_macro_t;

int bl_usb_openctrl();
void bl_usb_closectrl();
void bl_usb_enable_service_mode();
void bl_usb_disable_service_mode();
int bl_usb_read_matrix_pos(int *, int *);
int bl_usb_read_layout(uint8_t **, int *);
int bl_usb_write_layout(uint8_t *, int);
void bl_usb_raw_print_layout(uint16_t *, int);
void bl_usb_print_layout(uint8_t *, int);

void bl_layout_configure(uint8_t, char *);
int bl_layout_write(char *);
void bl_layout_print(bl_layout_t *);

void bl_usb_read_version(int *, int *);

void bl_usb_pwm_read(uint8_t *, uint8_t *);
void bl_usb_pwm_write(uint8_t, uint8_t);

uint8_t bl_usb_debounce_read();
void bl_usb_debounce_write(uint8_t debounce);

bl_macro_t* bl_usb_macro_read();
void bl_usb_macro_write(bl_macro_t *macros);

#endif /* __USB_H_ */
