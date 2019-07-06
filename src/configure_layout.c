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
#include <libusb.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

#include "blusb.h"
#include "layout.h"
#include "vkeycodes.h"
#include "usb.h"
#include "bl_tui.h"

typedef struct key_mapping_struct {
    uint8_t vk;
    char *name;
    uint16_t hid;
} key_mapping_t;

key_mapping_t bl_key_mapping[] = {
    { VK_APPS, "Win Menu", KB_APP },
    { VK_BACK, "Backspace", KB_BKSPC },
    { VK_CAPITAL, "Caps Lock", KB_CAPLK },
    { VK_DELETE, "Delete", KB_DEL },
    { VK_DOWN, "Down Arrow", KB_DOWN },
    { VK_END, "End", KB_END },
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
    { VK_HOME, "Home", KB_HOME },
    { VK_INSERT, "Insert", KB_INS },
    { VK_KEY_0, "KB 0", KB_0 },
    { VK_KEY_1, "KB 1", KB_1 },
    { VK_KEY_2, "KB 2", KB_2 },
    { VK_KEY_3, "KB 3", KB_3 },
    { VK_KEY_4, "KB 4", KB_4 },
    { VK_KEY_5, "KB 5", KB_5 },
    { VK_KEY_6, "KB 6", KB_6 },
    { VK_KEY_7, "KB 7", KB_7 },
    { VK_KEY_8, "KB 8", KB_8 },
    { VK_KEY_9, "KB 9", KB_9 },
    { VK_KEY_A, "A", KB_A },
    { VK_KEY_B, "B", KB_B },
    { VK_KEY_C, "C", KB_C },
    { VK_KEY_D, "D", KB_D },
    { VK_KEY_E, "E", KB_E },
    { VK_KEY_F, "F", KB_F },
    { VK_KEY_G, "G", KB_G },
    { VK_KEY_H, "H", KB_H },
    { VK_KEY_I, "I", KB_I },
    { VK_KEY_J, "J", KB_J },
    { VK_KEY_K, "K", KB_K },
    { VK_KEY_L, "L", KB_L },
    { VK_KEY_M, "M", KB_M },
    { VK_KEY_N, "N", KB_N },
    { VK_KEY_O, "O", KB_O },
    { VK_KEY_P, "P", KB_P },
    { VK_KEY_Q, "Q", KB_Q },
    { VK_KEY_R, "R", KB_R },
    { VK_KEY_S, "S", KB_S },
    { VK_KEY_T, "T", KB_T },
    { VK_KEY_U, "U", KB_U },
    { VK_KEY_V, "V", KB_V },
    { VK_KEY_W, "W", KB_W },
    { VK_KEY_X, "X", KB_X },
    { VK_KEY_Y, "Y", KB_Y },
    { VK_KEY_Z, "Z", KB_Z },
    { VK_LCONTROL, "Left Ctrl", KB_LCTRL },
    { VK_LEFT, "Left Arrow", KB_LEFT },
    { VK_LMENU, "Left Alt", KB_LALT },
    { VK_LSHIFT, "Left Shift", KB_LSHFT },
    { VK_LWIN, "Left Win", KB_LGUI },
    { VK_NEXT, "Page down", KB_PGDN },
    { VK_NUMLOCK, "Num Lock", KB_NUMLK },
    { VK_DIVIDE, "NP Divide", KP_SLASH },
    { VK_MULTIPLY, "NP Multiply", KP_ASTRX },
    { VK_SUBTRACT, "NP Subtract", KP_MINUS },
    { VK_ADD, "NP Add", KP_PLUS },
    { VK_RETURN, "NP Enter", KP_ENTER },
    { VK_DECIMAL, "NP Dot", KP_DOT },
    { VK_NUMPAD0, "NP 0", KP_0 },
    { VK_NUMPAD1, "NP 1", KP_1 },
    { VK_NUMPAD2, "NP 2", KP_2 },
    { VK_NUMPAD3, "NP 3", KP_3 },
    { VK_NUMPAD4, "NP 4", KP_4 },
    { VK_NUMPAD5, "NP 5", KP_5 },
    { VK_NUMPAD6, "NP 6", KP_6 },
    { VK_NUMPAD7, "NP 7", KP_7 },
    { VK_NUMPAD8, "NP 8", KP_8 },
    { VK_NUMPAD9, "NP 9", KP_9 },
    { VK_OEM_1, "OEM_1", KB_LBRCE },
    { VK_OEM_102, "OEM_102", KB_PIPE },
    { VK_OEM_2, "Backslash", KB_BSLSH },
    { VK_OEM_3, "Semicolon", KB_SMCLN },
    { VK_OEM_4, "Minus", KB_MINUS },
    { VK_OEM_5, "Tilde", KB_TILDE },
    { VK_OEM_6, "Equals", KB_EQUAL },
    { VK_OEM_7, "Quote", KB_QUOTE },
    { VK_OEM_COMMA, "Comma", KB_COMMA },
    { VK_OEM_MINUS, "Slash", KB_SLASH },
    { VK_OEM_PERIOD, "Period", KB_DOT },
    { VK_OEM_PLUS, "OEM_PLUS", KB_RBRCE },
    { VK_PRIOR, "Page up", KB_PGUP },
    { VK_RCONTROL, "Right Ctrl", KB_RCTRL },
    { VK_RETURN, "Enter", KB_ENTER },
    { VK_RETURN, "Enter", KB_RETURN },
    { VK_RIGHT, "Right Arrow", KB_RIGHT },
    { VK_RMENU, "Right Alt", KB_RALT },
    { VK_RSHIFT, "Right Shift", KB_RSHFT },
    { VK_RWIN, "Right Win", KB_RGUI },
    { VK_SCROLL, "Scroll Lock", KB_SCRLK },
    { VK_SNAPSHOT, "Print Screen", KB_PSCRN },
    { VK_SPACE, "Space", KB_SPACE },
    { VK_TAB, "Tab", KB_TAB },
    { VK_UP, "Up Arrow", KB_UP },
    { VK_PAUSE, "Pause", KB_PAUSE }
};
static int _n_key_mappings = sizeof(bl_key_mapping) / sizeof(key_mapping_t);

#define SELECT_BOX_WIDTH 8

typedef bl_tui_select_box_t *bl_matrix_ui_t[NUMLAYERS_MAX][NUMROWS][NUMCOLS];

/*
 * Draw a matrix cell. The matrix is is turned 90 degrees, i.e. rows
 * are columns, columns are rows.
 */
void
draw_matrix_cell(bl_tui_select_box_t *sb, int x, int y, int inversed) {
    bl_tui_select_box_draw(sb, y*(sb->width+1)+4, x+2, inversed);
}

/**
 * Find the key mapping for the key in the matrix and return the index value. If
 * the item is not found return 0.
 */
int
bl_layout_get_selected_item(int layer, int row, int col,
			    bl_layout_t *layout, bl_tui_select_box_value_t *bl_key_mapping_items, int n_items) {
  for (int i=0; i<n_items; i++) {
    if (*((uint16_t *)bl_key_mapping_items[i].data) == layout->matrix[layer][row][col]) {
      return i;
    }
  }
  return 0;
}

/**
 * Initialize the matrix with select boxes.
 */
void
bl_layout_init_matrix(bl_matrix_ui_t matrix, bl_layout_t *layout,
                      bl_tui_select_box_value_t *bl_key_mapping_items, int n_items) {
    /*
     * Create list boxes for each cell.
     */
    for (int layer=0; layer<NUMLAYERS_MAX; layer++) {
        for (int r=0; r<NUMROWS; r++) {
            for (int c=0; c<NUMCOLS; c++) {
                matrix[layer][r][c] = bl_tui_select_box_create(NULL, bl_key_mapping_items, n_items,
                                                               SELECT_BOX_WIDTH, 2*SELECT_BOX_WIDTH);
                matrix[layer][r][c]->selected_item_index = bl_layout_get_selected_item(layer, r, c, layout,
                                                                                       bl_key_mapping_items, n_items);
            }
        }
    }
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
 *
 * @param matrix Data structure containing the select boxes for each layer
 * @param layer The index of the layer to draw, starts at 0.
 */
void
bl_layout_draw_keyboard_matrix(bl_matrix_ui_t matrix, int layer, int nlayers) {

    /*
     * Draw column headers (vertically)
     */
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    for (int c=0; c<NUMCOLS; c++) {
        mvprintw(2+c, 0, "C%0d", c);
    }

    /*
     * Draw layer tabs
     */
    mvprintw(0, 0, "Layer");
    attron(A_REVERSE);
    for (int i=0; i<nlayers; i++) {
        if (i == layer) {
            attron(A_BOLD);
        }
        mvprintw(0, 6 + i*5, "  %d  ", i+1);
        attroff(A_BOLD);
    }
    attroff(A_REVERSE);

    /*
     * Draw row headers (horizontally
     */
    for (int r=0; r<NUMROWS; r++) {
        mvprintw(1, 4+r*(SELECT_BOX_WIDTH+1), "R%0d", r);
    }
    attroff(COLOR_PAIR(1));

    /*
     * Draw the matrix, but draw the columns as rows and rows as columns, this way
     * we can use the space on the terminal more efficiently.
     */
    for (int c=0; c<NUMCOLS; c++) {
        for (int r=0; r<NUMROWS; r++) {
            draw_matrix_cell(matrix[layer][r][c], c, r, FALSE);
        }
    }

    /*
     * Footer
     */
    int maxy = getmaxy(stdscr);
    wmove(stdscr, maxy - 1, 0);
    printw("Enter: select key, ");
    attron(A_UNDERLINE); printw("O"); attroff(A_UNDERLINE);
    printw("pen, ");
    attron(A_UNDERLINE); printw("S"); attroff(A_UNDERLINE);
    printw("ave, ");
    attron(A_UNDERLINE); printw("W"); attroff(A_UNDERLINE);
    printw("rite to ctrl, ");
    attron(A_UNDERLINE); printw("L"); attroff(A_UNDERLINE);
    printw("ayers, ");
    attron(A_UNDERLINE); printw("Q"); attroff(A_UNDERLINE);
    printw("uit, Select layer: ");
    attron(A_UNDERLINE); printw("1"); attroff(A_UNDERLINE);
    printw(" - ");
    attron(A_UNDERLINE); printw("6"); attroff(A_UNDERLINE);
}

bl_layout_t *
bl_layout_select_and_load_file() {
    bl_io_dirent_t *de = bl_tui_fselect(".");
    if (de != NULL) {
        bl_layout_t *layout = bl_layout_load_file(de->name);
        bl_io_dirent_destroy(de);
        return layout;
    } else {
        return NULL;
    }
}

void
bl_layout_save_to_file(bl_layout_t *layout) {
    char *fname = bl_tui_textbox("Save File", "Enter a name for the file", NULL, 5, 5, 20, 100);
    if (fname != NULL) {
        if (bl_layout_save(layout, fname) == 0) {
            bl_tui_msg(20, 1, "Save File", "File saved");
        } else {
            bl_tui_msg(20, 1, "Save File", "Error while saving");
        }
    }
}

void
bl_layout_write_to_controller(bl_layout_t *layout) {
    if (bl_tui_confirm(61, 1, "Write to Controller",
                       "Do you wish to write the new configuration to the controller?")) {
        bl_layout_write(layout);
    }
}

/**
 * Get a number between 1 and 6 to store in the parameter nlayers
 *
 * @param nlayers Pointer to int to hold the number of variables
 * @return FALSE if operation was cancelled, TRUE if success.
 */
int
bl_layout_manage_layers(bl_layout_t *layout) {
    /*
     * layout->nlayers < 10, so we can use this trick
     */
    char value[] = { '0' + layout->nlayers };
    char *nr_of_layers_str = bl_tui_textbox("Number of layers (1-6)", "Layers", value, 10, 10, 30, 2);
    int nr_of_layers = nr_of_layers_str != NULL ? strtol(nr_of_layers_str, NULL, 10) : -1;

    if (nr_of_layers == -1) {
        return FALSE;
    } else if (errno == EINVAL) {
        return bl_layout_manage_layers(layout);
    } else if (nr_of_layers < 1 || nr_of_layers > 6) {
        bl_tui_err(FALSE, "Value must be an integer between 1 and 6");
        return bl_layout_manage_layers(layout);
    } else {
        if (nr_of_layers < layout->nlayers) {
            if (bl_tui_confirm(35, 5,
                              "Deleting layers",
                              "You are deleting %d layer(s). Are you sure?",
                              layout->nlayers - nr_of_layers)) {
                layout->nlayers = nr_of_layers;
            }
        } else {
            layout->nlayers = nr_of_layers;
        }
        return TRUE;
    }
}

void
bl_layout_navigate_matrix(bl_matrix_ui_t matrix, bl_layout_t *layout, int layer, bl_tui_select_box_value_t *bl_key_mapping_items, int n_key_mappings) {
    int col = 0;
    int row = 0;
    int col_last = 0;
    int row_last = 0;

    int ch = getch();
    int redraw = FALSE;
    draw_matrix_cell(matrix[layer][row][col], col, row, TRUE);
    while (ch != 'q' && ch != 'Q') {
        /*
         * See if key was pressed on the IBM model m keyboard and get
         * its position if so.
         */
        int m_row, m_col;
        if (bl_usb_read_matrix_pos(&m_row, &m_col)) {
            col = m_col;
            row = m_row;
        }
        /*
         * Check the key presses on the alternate keyboard
         */
        ch = getch();
        if (ch == KEY_DOWN && col < NUMCOLS-1) {
            col++;
        } else if (ch == KEY_UP && col > 0) {
            col--;
        } else if (ch == KEY_RIGHT && row < NUMROWS-1) {
            row++;
        } else if (ch == KEY_LEFT && row > 0) {
            row--;
        } else if (ch == '\n' || ch == '\r') {
            bl_tui_select_box_t *sb = matrix[layer][row][col];
            bl_tui_select_box(sb, row  * (SELECT_BOX_WIDTH + 1) + 4, col + 4);
            mvprintw(1, 0, "sel=%d\n",  *((uint16_t*) sb->items[sb->selected_item_index].data));
            layout->matrix[layer][row][col] = *((uint16_t*) sb->items[sb->selected_item_index].data);
            erase();
            redraw = TRUE;
        } else if (ch == 'o' || ch == 'O') {
            bl_layout_t *layout_new = bl_layout_select_and_load_file();
            if (layout_new != NULL) {
                bl_layout_destroy(layout);
                layout = layout_new;
                bl_layout_init_matrix(matrix, layout, bl_key_mapping_items, n_key_mappings);
            }
            redraw = TRUE;
        } else if (ch == 's' || ch == 'S') {
            bl_layout_save_to_file(layout);
            redraw = TRUE;
        } else if (ch == 'w' || ch == 'W') {
            bl_layout_write_to_controller(layout);
            redraw = TRUE;
        } else if (ch == 'l' || ch == 'L') {
            bl_layout_manage_layers(layout);
            redraw = TRUE;
        } else if (ch - (int)'0' >= 1 && ch - (int)'0' <= layout->nlayers) {
            layer = ch - (int)'0' - 1;
            redraw = TRUE;
        }
        if (redraw) {
            clear();
            bl_layout_draw_keyboard_matrix(matrix, layer, layout->nlayers);
            draw_matrix_cell(matrix[layer][row][col], col, row, TRUE);
            redraw = FALSE;
        }
        if (row != row_last || col != col_last) {
            draw_matrix_cell(matrix[layer][row_last][col_last], col_last, row_last, FALSE);
            draw_matrix_cell(matrix[layer][row][col], col, row, TRUE);
            col_last = col;
            row_last = row;
        }
        mvprintw(0, 55, "col: %d, row: %d, val: %u  ", col, row, layout->matrix[layer][row][col]);
        refresh();
        // don't hog the cpu too much
        usleep(50);
    }
}

/**
 * Initialize all layers to 0
 *
 * @param layout Layout data
 */
void
bl_layout_init_layout(bl_layout_t *layout) {
    for (int layer=0; layer < NUMLAYERS_MAX; layer++) {
        for (int row=0; row<NUMROWS; row++) {
            for (int col=0; col<NUMCOLS; col++) {
                layout->matrix[layer][row][col] = 0;
            }
        }
    }
}

/**
 * Read the existing keyboard layout and return it.
 *
 * @param layout A bl_layout_t variable which will be initialised with the values
 *               for the current layout.
 */
void
bl_layout_read(bl_layout_t *layout) {
    unsigned char *buffer;
    int nlayers;

    bl_usb_read_layout(&buffer, &nlayers);
    layout->nlayers = nlayers;
    for (int layer=0; layer<nlayers; layer++) {
        for (int row=0; row<NUMROWS; row++) {
            for (int col=0; col<NUMCOLS; col++) {
                int n = layer * NUMROWS * NUMCOLS + row * NUMCOLS + col;
                layout->matrix[layer][row][col] = ((uint16_t *) buffer)[n];
            }
        }
    }

    return;
}

void
bl_layout_configure(bl_layout_t *layout) {

    bl_tui_select_box_value_t bl_key_mapping_items[_n_key_mappings + 1];
    static int not_selected_value = 0;

    /*
     * Construct the select box list for every select box in the
     * matrix from the list of keycodes.
     *
     * The select box is read only and will be shared.
     *
     * The first entry is for the case the mapping has (not yet) been
     * defined.
     */
    bl_key_mapping_items[0].label = strdup("--");
    bl_key_mapping_items[0].data = &not_selected_value;
    for (int i=0; i<_n_key_mappings; i++) {
      bl_key_mapping_items[i+1].label = bl_key_mapping[i].name;
      bl_key_mapping_items[i+1].is_bold = FALSE;
      bl_key_mapping_items[i+1].data = &bl_key_mapping[i].hid;
    }

    if (bl_tui_init()) {
        bl_matrix_ui_t matrix;

        /*
         * wait until key has been released and then enable service mode.
         * It would be better if we could detect key up and key down
         * events, but that doesn't work in terminal mode.
         */
        usleep(100000);
        bl_usb_enable_service_mode();

        if (layout == NULL) {
            fprintf(stderr, "reading from controller\n");
            layout = bl_layout_create(0);
            bl_layout_init_layout(layout);
            bl_layout_read(layout);
        } else {
            printf("using existing layout\n");
            bl_layout_print(layout);
        }

        bl_layout_init_matrix(matrix, layout, bl_key_mapping_items, _n_key_mappings+1);
        bl_layout_draw_keyboard_matrix(matrix, 0, layout->nlayers);
        bl_layout_navigate_matrix(matrix, layout, 0, bl_key_mapping_items, _n_key_mappings+1);
        bl_tui_exit();
        bl_usb_disable_service_mode();
    }
}
