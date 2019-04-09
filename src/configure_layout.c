/*
 * Interactive configuration of blusb controller.
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

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <ncurses.h>
#include <libusb.h>
#include <string.h>

#include "blusb.h"
#include "layout.h"
#include "vkeycodes.h"

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

#define USB_TIMEOUT 1000

#ifndef _WIN32

uint8_t
read_nr_of_Layers() {
    uint8_t nrlayers = 1;
    uint8_t has_read = FALSE;

    return nrlayers;
}

typedef struct key_mapping_struct {
    uint8_t vk;
    char *name;
    uint16_t hid;
} key_mapping_t;

key_mapping_t key_mapping[] = {
    { VK_ESCAPE, "ESC", KB_ESC },
    { VK_F1, "F1", KB_F1 },
    { VK_F2, "F2", KB_F2 },
    { VK_F3, "F3", KB_F3 },
    { VK_F4, "F4", KB_F4 },
    { VK_F5, "F5", KB_F5 },
    { VK_F6, "F6", KB_F6 },
    { VK_F7, "F7", KB_F7 },
    { VK_F8, "F8", KB_F8 },
    { VK_F9, "F9", KB_F9 },
    { VK_F10, "F10", KB_F10 },
    { VK_F11, "F11", KB_F11 },
    { VK_F12, "F12", KB_F12 },
    { VK_SNAPSHOT, "Print Screen", KB_PSCRN },
    { VK_SCROLL, "Scroll Lock", KB_SCRLK },
    { VK_OEM_5, "OEM_5", KB_TILDE },
    { VK_KEY_1, "KB 1", KB_1 },
    { VK_KEY_2, "KB 2", KB_2 },
    { VK_KEY_3, "KB 3", KB_3 },
    { VK_KEY_4, "KB 4", KB_4 },
    { VK_KEY_5, "KB 5", KB_5 },
    { VK_KEY_6, "KB 6", KB_6 },
    { VK_KEY_7, "KB 7", KB_7 },
    { VK_KEY_8, "KB 8", KB_8 },
    { VK_KEY_9, "KB 9", KB_9 },
    { VK_KEY_0, "KB 0", KB_0 },
    { VK_OEM_4, "OEM_4", KB_MINUS },
    { VK_OEM_6, "OEM_6", KB_EQUAL },
    { VK_BACK, "Backspace", KB_BKSPC },
    { VK_INSERT, "Insert", KB_INS },
    { VK_HOME, "Home", KB_HOME },
    { VK_PRIOR, "Page up", KB_PGUP },
    { VK_TAB, "Tab", KB_TAB },
    { VK_KEY_Q, "Q", KB_Q },
    { VK_KEY_W, "W", KB_W },
    { VK_KEY_E, "E", KB_E },
    { VK_KEY_R, "R", KB_R },
    { VK_KEY_T, "T", KB_T },
    { VK_KEY_Z, "Z", KB_Z },
    { VK_KEY_U, "U", KB_U },
    { VK_KEY_I, "I", KB_I },
    { VK_KEY_O, "O", KB_O },
    { VK_KEY_P, "P", KB_P },
    { VK_OEM_1, "OEM_1", KB_LBRCE },
    { VK_OEM_PLUS, "OEM_PLUS", KB_RBRCE },
    { VK_RETURN, "Enter", KB_ENTER },
    { VK_DELETE, "Delete", KB_DEL },
    { VK_END, "End", KB_END },
    { VK_NEXT, "Page down", KB_PGDN },
    { VK_CAPITAL, "Caps Lock", KB_CAPLK },
    { VK_KEY_A, "A", KB_A },
    { VK_KEY_S, "S", KB_S },
    { VK_KEY_D, "D", KB_D },
    { VK_KEY_F, "F", KB_F },
    { VK_KEY_G, "G", KB_G },
    { VK_KEY_H, "H", KB_H },
    { VK_KEY_J, "J", KB_J },
    { VK_KEY_K, "K", KB_K },
    { VK_KEY_L, "L", KB_L },
    { VK_OEM_3, "OEM_3", KB_SMCLN },
    { VK_OEM_7, "OEM_7", KB_QUOTE },
    { VK_OEM_2, "OEM_2", KB_BSLSH },
    { VK_LSHIFT, "Left Shift", KB_LSHFT },
    { VK_OEM_102, "OEM_102", KB_PIPE },
    { VK_KEY_Y, "Y", KB_Y },
    { VK_KEY_X, "X", KB_X },
    { VK_KEY_C, "C", KB_C },
    { VK_KEY_V, "V", KB_V },
    { VK_KEY_B, "B", KB_B },
    { VK_KEY_N, "N", KB_N },
    { VK_KEY_M, "M", KB_M },
    { VK_OEM_COMMA, "OEM_COMMA", KB_COMMA },
    { VK_OEM_PERIOD, "OEM_PERIOD", KB_DOT },
    { VK_OEM_MINUS, "OEM_MINUS", KB_SLASH },
    { VK_RSHIFT, "Right Shift", KB_RSHFT },
    { VK_UP, "Up Arrow", KB_UP },
    { VK_LCONTROL, "Left Ctrl", KB_LCTRL },
    { VK_LWIN, "Left Win", KB_LGUI },
    { VK_LMENU, "Left Alt", KB_LALT },
    { VK_SPACE, "Space", KB_SPACE },
    { VK_RMENU, "Right Alt", KB_RALT },
    { VK_RWIN, "Right Win", KB_RGUI },
    { VK_APPS, "Win Menu", KB_APP },
    { VK_RCONTROL, "Right Ctrl", KB_RCTRL },
    { VK_LEFT, "Left Arrow", KB_LEFT },
    { VK_DOWN, "Down Arrow", KB_DOWN },
    { VK_RIGHT, "Right Arrow", KB_RIGHT },
    { VK_NUMLOCK, "Num Lock", KB_NUMLK },
    { VK_DIVIDE, "NP_Divide", KP_SLASH },
    { VK_MULTIPLY, "NP_Multiply", KP_ASTRX },
    { VK_SUBTRACT, "NP_Subtract", KP_MINUS },
    { VK_ADD, "NP_Add", KP_PLUS },
    { VK_RETURN, "Enter", KB_RETURN },
    { VK_NUMPAD0, "NP_0", KP_0 },
    { VK_NUMPAD1, "NP_1", KP_1 },
    { VK_NUMPAD2, "NP_2", KP_2 },
    { VK_NUMPAD3, "NP_3", KP_3 },
    { VK_NUMPAD4, "NP_4", KP_4 },
    { VK_NUMPAD5, "NP_5", KP_5 },
    { VK_NUMPAD6, "NP_6", KP_6 },
    { VK_NUMPAD7, "NP_7", KP_7 },
    { VK_NUMPAD8, "NP_8", KP_8 },
    { VK_NUMPAD9, "NP_9", KP_9 },
    { VK_DECIMAL, "NP_Dot", KP_DOT }
};

extern libusb_device_handle *handle;

void
exit_tui() {
    endwin();
}

int
init_tui() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    nodelay(stdscr, TRUE);

    if (!has_colors()) {
        printf("no colors\n");
        return FALSE;
    }

    int x, y;
    getmaxyx(stdscr, y, x);
    if (x < 80 || y < 25) {
        exit_tui();
        printf("The terminal must be at least 80 columns by 25 rows\n");
        printf("The reported size is: columns: %d, rows: %d\n", x, y);
        return FALSE;
    }

    return TRUE;
}

void
errmsg_and_abort(char *msg) {
    printf("Error allocating memory for: %s\n", msg);
    exit(1);
}

#define SELECT_BOX_WIDTH 8

typedef struct select_box_t {
    int n;
    int selected_item_index;
    int width;
    char **items;
} select_box_t;


select_box_t *
create_select_box(char **items, int n, int width) {
    select_box_t *sb = (select_box_t *) malloc(sizeof(select_box_t));
    if (sb == NULL) {
        errmsg_and_abort("select_box");
    } else {
        sb->n = n;
        sb->selected_item_index = 0;
        sb->width = width;
        sb->items = items;
    }
    return sb;
}

/*
 * Draw the select box, with the selected item inversed
 * and printed as the first element.
 *
 * @param sb selectbox to draw
 * @param x x coordinate (column)
 * @param y y coordinate (row)
 * @param inversed if true draw inversed, otherwise draw normally
 */
void
draw_select_box(select_box_t *sb, int x, int y, int inversed) {
    char *selected_item = NULL;

    if (sb->selected_item_index < sb->n) {
        selected_item = sb->items[sb->selected_item_index];
    } else {
        errmsg_and_abort("selected index out of range");
    }

    if (inversed) {
        wattron(stdscr, A_REVERSE);
    }
    char *template = (char*) malloc(20);
    sprintf(template, "%%.%ds", SELECT_BOX_WIDTH);
    mvwprintw(stdscr, y, x, template, selected_item);
    free(template);
    if (inversed) {
        wattroff(stdscr, A_REVERSE);
    }
}

/*
 * Draw a matrix cell. The matrix is is turned 90 degrees, i.e. rows
 * are columns, columns are rows.
 */
void
draw_matrix_cell(select_box_t *sb, int x, int y, int inversed) {
    draw_select_box(sb, y*(SELECT_BOX_WIDTH+1)+4, x+4, inversed);
}

/*
 * Draw the keyboard matrix. The matrix consists of many more columns than rows. At the time
 * of writing 20 rows, and 8 columns.
 *
 * In order to be able to print the labels for each cell in a terminal windows, we assume a minimum
 * terminal size of 80 columns by 25 rows, we let the rows run horizontally and the columns run vertically.
 *
 * i.e.
 *
 *    R1 R2 ... R20
 * C1
 * C2
 * .
 * .
 * .
 * C8
 */
select_box_t ***
drawKeyboardMatrix() {
    select_box_t ***matrix;

    /*
     * Allocate matrix
     */
    matrix = (select_box_t ***) malloc(NUMCOLS * sizeof(select_box_t **));
    for (int c=0; c<NUMCOLS; c++) {
        matrix[c] = (select_box_t **) malloc(NUMROWS * sizeof(select_box_t *));
    }

    /*
     * Create list boxes in each cell.
     */
    for (int c=0; c<NUMCOLS; c++) {
        for (int r=0; r<NUMROWS; r++) {
            char *items[] = { "first", "second" };
            matrix[c][r] = create_select_box(items, 2, SELECT_BOX_WIDTH);
        }
    }

    /*
     * Draw column headers (vertically)
     */
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    for (int c=0; c<NUMCOLS; c++) {
        mvprintw(4+c, 0, "C%0d", c);
    }

    /*
     * Draw row headers (horizontally
     */
    for (int r=0; r<NUMROWS; r++) {
        mvprintw(2, 4+r*(SELECT_BOX_WIDTH+1), "R%0d", r);
    }
    attroff(COLOR_PAIR(1));

    /*
     * Draw the matrix, but draw the columns as rows and rows as columns, this way
     * we can use the space on the terminal more efficiently.
     */
    for (int c=0; c<NUMCOLS; c++) {
        for (int r=0; r<NUMROWS; r++) {
            draw_matrix_cell(matrix[c][r], c, r, FALSE);
        }
    }

    return matrix;
}

void
navigateMatrix(select_box_t ***matrix) {
    int x = 0;
    int y = 0;
    int x_last = 0;
    int y_last = 0;

    int ch = getch();
    draw_matrix_cell(matrix[x][y], y, x, TRUE);
    uint16_t rowcol_last = 0;
    while (ch != KEY_F(12)) {
        uint16_t rowcol = read_matrix_pos();
        if (rowcol != 0xffff && rowcol != rowcol_last) {
            uint16_t row = rowcol & 0x0f;	// decipher with bitmask and assign value
            uint16_t col = (rowcol & 0xff00) / 256;
            rowcol_last = rowcol;
            x = col;
            y = row;
        }
        refresh();
        ch = getch();
        if (ch == KEY_DOWN && x < NUMCOLS-1) {
            x++;
        } else if (ch == KEY_UP && x > 0) {
            x--;
        } else if (ch == KEY_RIGHT && y < NUMROWS-1) {
            y++;
        } else if (ch == KEY_LEFT && y > 0) {
            y--;
        } else if (ch == KEY_ENTER) {
            // popup select box
        }
        if (x != x_last || y != y_last) {
            draw_matrix_cell(matrix[x_last][y_last], x_last, y_last, FALSE);
            draw_matrix_cell(matrix[x][y], x, y, TRUE);
            x_last = x;
            y_last = y;
        }
        mvprintw(0, 0, "col: %d, row: %d  ", x, y);
        refresh();
        usleep(10);
    }
}


void
mb_read_layout(uint16_t ***kbd_layout, uint8_t *nlayers) {
    char buffer[NUMLAYERS_MAX * NUMROWS * NUMCOLS + 1];

    libusb_control_transfer(handle, LIBUSB_RECIPIENT_ENDPOINT | LIBUSB_ENDPOINT_IN |
        LIBUSB_REQUEST_TYPE_VENDOR, USB_READ_LAYOUT, 0, 0, buffer, sizeof(buffer), USB_TIMEOUT);

    *nlayers = buffer[0];
    if (*nlayers == 0) {
        return;
    } else {
        for (int layer=0; layer<*nlayers; layer++) {
            for (int col=0; col<NUMCOLS; col++) {
                for (int row=0; row<NUMROWS; row++) {
                    kbd_layout[layer][col][row] = *((uint16_t *)buffer + 1 + col + row * NUMCOLS);
                }
            }
        }
    }
}

void
configure_layout(uint8_t nlayers, char *p_layout_array_keyfile) {

    uint16_t kbd_layout[NUMLAYERS_MAX][NUMCOLS][NUMROWS];
    if (init_tui()) {
        //mb_read_layout(kbd_layout, 1);
        select_box_t ***matrix = drawKeyboardMatrix(kbd_layout);
        navigateMatrix(matrix);
        exit_tui();
    }
}

#endif
