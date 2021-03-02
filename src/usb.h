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

#include "layout.h"

#ifndef __USB_H__
#define __USB_H__ 1

typedef uint16_t bl_matrix_t[NUMLAYERS_MAX][NUMROWS][NUMCOLS];

typedef struct bl_layout_t {
    int nlayers;
    bl_matrix_t matrix;
} bl_layout_t;

typedef uint8_t bl_macro_keylist_t[NUM_MACROKEYS][LEN_MACRO];

typedef struct bl_macro_t {
    int nmacros;
    bl_macro_keylist_t macros;
} bl_macro_t;

int bl_usb_openctrl();
void bl_usb_closectrl();
void bl_usb_enable_service_mode();
void bl_usb_enable_service_mode_safe();
void bl_usb_disable_service_mode();
int bl_usb_read_matrix_pos(int *, int *);
int bl_usb_read_layout(uint8_t **, int *);
int bl_usb_write_layout(uint8_t *, int);
void bl_usb_raw_print_layout(uint16_t *, int, FILE *);
void bl_usb_print_layout(uint8_t *, int);

void bl_usb_read_version(int *, int *);

void bl_usb_pwm_read(uint8_t *, uint8_t *);
void bl_usb_pwm_write(uint8_t, uint8_t);

uint8_t bl_usb_debounce_read();
void bl_usb_debounce_write(uint8_t debounce);

bl_macro_t* bl_usb_macro_read();
void bl_usb_macro_write(bl_macro_t *macros);
void bl_usb_macro_print(bl_macro_t *bm);
void bl_usb_set_mode(int mode);
int bl_usb_get_mode();
int bl_usb_set_numlock(int is_on);

/*
 * Macros
 */
bl_macro_t *bl_macro_parse(char *fname);

/*
 * Layout
 */
void bl_layout_configure(bl_layout_t *);
int bl_layout_write(bl_layout_t *);
int bl_layout_write_from_file(char *);
void bl_layout_print(bl_layout_t *);
int bl_layout_save(bl_layout_t *, char *);
uint8_t *bl_layout_convert(bl_layout_t *);
bl_layout_t *bl_layout_load_file(char *);
bl_layout_t *bl_layout_create(int);
void bl_layout_destroy(bl_layout_t *);
void bl_layout_init_layout(bl_layout_t *);


#endif /* __USB_H__ */
