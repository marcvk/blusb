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

#ifndef __MBLUSB_H_
#define __MBLUSB_H_

#include <stdlib.h>
#include <stdint.h>

#include "bl_tui.h"

#define BL_SOFTWARE_VERSION "1.0"

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

#ifndef MAX
#define MAX(a, b) (a < b ? b : a)
#endif
#ifndef MIN
#define MIN(a, b) (a < b ? a : b)
#endif


#define errmsg_and_abort(...) do { \
    bl_tui_exit(); \
    printf ("@ %s (%d): ", __FILE__, __LINE__); \
    printf (__VA_ARGS__); \
    printf("\n"); \
    exit(1); \
} while (0)


/*
 * Custom USB commands
 */
#define USB_ENABLE_VENDOR_RQ	0x11
#define USB_DISABLE_VENDOR_RQ	0x10
#define USB_READ_BR				0x20
#define USB_WRITE_BR			0x21

#define USB_READ_MATRIX			0x30
#define USB_READ_LAYOUT			0x40
#define USB_WRITE_LAYOUT		0x41
#define USB_READ_DEBOUNCE		0x50
#define USB_WRITE_DEBOUNCE		0x51
#define USB_READ_MACROS			0x60
#define USB_WRITE_MACROS		0x61
#define USB_READ_VERSION		0x70

/*
 * timeout in ms
 */
#define BL_USB_TIMEOUT 1000

/************************************************************************/
/*                        Function prototypes                           */
/************************************************************************/

void print_keyfile(char *p_layout_array, uint8_t nlayers);
void print_macrosfile(char *p_macros_array);
char *fill_layout_array(uint8_t nlayers);
char *fill_macros_array(void);
uint8_t parse_keyfile(char *keyfile_namestring);
void parse_macrosfile(char *keyfile_namestring);
uint16_t read_matrix_pos(void);
void read_pwm(void);
void read_version(void);
void write_pwm(int pwm_USB, int pwm_BT);
void read_debounce(void);
void write_debounce(int debounce_value);
char *read_layout(void);
void write_layout(uint8_t nlayers, char *p_layout_array);
char *read_macros(void);
void write_macros(unsigned char *p_char_ctrl_buf);
void print_usage(char **argv);
void enable_service_mode(void);
void disable_service_mode(void);
void configure_layout(uint8_t nlayers, char *p_layout_array_keyfile);

#endif /* __MBLUSB_H_ */
