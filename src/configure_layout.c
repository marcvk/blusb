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
#include "usb.h"

typedef struct key_mapping_struct {
    uint8_t vk;
    char *name;
    uint16_t hid;
} key_mapping_t;

key_mapping_t bl_key_mapping[] = {
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
    { VK_OEM_5, "Tilde", KB_TILDE },
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
    { VK_OEM_4, "Minus", KB_MINUS },
    { VK_OEM_6, "Equals", KB_EQUAL },
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
    { VK_OEM_3, "Semicolon", KB_SMCLN },
    { VK_OEM_7, "Quote", KB_QUOTE },
    { VK_OEM_2, "Backslash", KB_BSLSH },
    { VK_LSHIFT, "Left Shift", KB_LSHFT },
    { VK_OEM_102, "OEM_102", KB_PIPE },
    { VK_KEY_Y, "Y", KB_Y },
    { VK_KEY_X, "X", KB_X },
    { VK_KEY_C, "C", KB_C },
    { VK_KEY_V, "V", KB_V },
    { VK_KEY_B, "B", KB_B },
    { VK_KEY_N, "N", KB_N },
    { VK_KEY_M, "M", KB_M },
    { VK_OEM_COMMA, "Comma", KB_COMMA },
    { VK_OEM_PERIOD, "Period", KB_DOT },
    { VK_OEM_MINUS, "Slash", KB_SLASH },
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
    { VK_DECIMAL, "NP_Dot", KP_DOT },
    { VK_RETURN, "NP_Enter", KP_ENTER }
};
static int n_key_mappings = sizeof(bl_key_mapping) / sizeof(key_mapping_t);

void
bl_tui_exit() {
    endwin();
}

int
bl_tui_init() {
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
    if (x < 80 || y < 24) {
        bl_tui_exit();
        printf("The terminal must be at least 80 columns by 24 rows\n");
        printf("The reported size is: columns: %d, rows: %d\n", x, y);
        return FALSE;
    }

    return TRUE;
}

#define errmsg_and_abort(...) do { \
    bl_tui_exit(); \
    printf ("@ %s (%d): ", __FILE__, __LINE__); \
    printf (__VA_ARGS__); \
    printf("\n"); \
    exit(1); \
} while (0)

#define SELECT_BOX_WIDTH 8

typedef struct select_box_value_t {
    char *label;
    void *data;
} select_box_value_t;

typedef struct select_box_t {
    int n;
    int selected_item_index;
    int width;
    select_box_value_t *items;
} select_box_t;

typedef select_box_t bl_matrix_ui_t[NUMLAYERS_MAX][NUMROWS][NUMCOLS];

select_box_t *
init_select_box(select_box_t *sb, select_box_value_t *items, int n, int width) {
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
    select_box_value_t *selected_item = NULL;

    if (sb->selected_item_index < sb->n) {
        selected_item = &sb->items[sb->selected_item_index];
    } else {
        errmsg_and_abort("selected index out of range: %d", sb->selected_item_index);
    }

    char *template = (char*) malloc(20 + strlen(selected_item->label));
    sprintf(template, "%%.%ds", SELECT_BOX_WIDTH);

    if (inversed) {
        wattron(stdscr, A_REVERSE);
    }
    mvwprintw(stdscr, y, x, template, selected_item->label);
    if (inversed) {
        wattroff(stdscr, A_REVERSE);
    }

    free(template);
}

/*
 * Draw a matrix cell. The matrix is is turned 90 degrees, i.e. rows
 * are columns, columns are rows.
 */
void
draw_matrix_cell(select_box_t *sb, int x, int y, int inversed) {
    draw_select_box(sb, y*(SELECT_BOX_WIDTH+1)+4, x+4, inversed);
}

/**
 * Find the key mapping for the key in the matrix and return the index value. If
 * the item is not found return 0.
 */
int
bl_layout_get_selected_item(int layer, int row, int col,
			    bl_layout_t *layout, select_box_value_t *bl_key_mapping_items, int n_items) {
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
		      select_box_value_t *bl_key_mapping_items, int n_items) {
    /*
     * Create list boxes for each cell.
     */
    for (int layer=0; layer<NUMLAYERS_MAX; layer++) {
        for (int r=0; r<NUMROWS; r++) {
            for (int c=0; c<NUMCOLS; c++) {
                select_box_t *sb = &matrix[layer][r][c];
                init_select_box(sb, bl_key_mapping_items, n_items, SELECT_BOX_WIDTH);
                sb->selected_item_index = bl_layout_get_selected_item(layer, r, c, layout,
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
 */
void
bl_layout_draw_keyboard_matrix(bl_matrix_ui_t matrix) {
    
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
            draw_matrix_cell(&matrix[0][r][c], c, r, FALSE);
        }
    }
}

void
bl_layout_select_box_redraw_list(WINDOW *win, select_box_t *sb, 
                                 int cursor_i, int item_start, int item_end) {
//    mvprintw(1, 0, "cursor_i=%d, item_start=%d, item_end=%d", 
//            cursor_i, item_start, item_end);
    werase(win);
    box(win, 0, 0);
    for (int i=item_start; i<item_end; i++) {
        if (i == cursor_i) {
            wattron(win, A_REVERSE);
            mvwprintw(win, i-item_start+1, 1, "%s", sb->items[i].label);
            wattroff(win, A_REVERSE);
        } else {
            mvwprintw(win, i-item_start+1, 1, "%s", sb->items[i].label);
        }
    }
    wrefresh(win);
}

/*
 * Show a popup at the given coordinates and select a value from the 
 * select box using the arrow keys and enter key.
 */
int
bl_layout_select_box(select_box_t *sb, int y, int x) {
    int maxx, maxy;
    
    /*
     * Draw a popup box in the middle of the screen.
     */
    getmaxyx(stdscr, maxy, maxx);
    int w = 15;
    int h = maxy / 2 - 3;

    if (y > maxy / 2) {
        y = y - h ;
    } else {
        y = y + 1;
    }
    WINDOW *win = newwin(h, w, y, x);
    box(win, 0, 0);
    touchwin(win);
    wrefresh(win);

    /*
     * The cursor in the select box always points to an element sb->items
     *   cursor_i
     * The list of items displayed in the select box has a starting point:
     *   item_start
     * and an end point
     *   item_end
     * 
     * The following must remain invariant:
     * 
     * 0 <= elt_start <= cursor_i <= elt_end <= n_items-1
     * &&
     * elt_end - elt_start <= h
     */
    /*
     * substract top and bottom line of box
     */
    int n_items = h - 2;
    int old_selected_item_index = sb->selected_item_index;
    int cursor_i = 0;
    int item_start = 0;
    int item_end = sb->n > n_items ? n_items : sb->n;
    int ch = getch();
    int last_ch = ch;
    int selecting = TRUE;
    bl_layout_select_box_redraw_list(win, sb, cursor_i, item_start, item_end);
    while (selecting) {
        ch = getch();
        if (ch != last_ch) {
            if (ch == 27 /* ESC */) {
                sb->selected_item_index = old_selected_item_index;
                selecting = FALSE;
            } else if (ch == 10 /* ENTER */) {
                selecting = FALSE;
            } else if (ch == KEY_UP && cursor_i > 0) {
                cursor_i--;
                if (cursor_i < item_start) {
                    item_start--;
                    item_end--;
                }
            } else if (ch == KEY_DOWN && cursor_i < sb->n - 1) {
                cursor_i++;
                if (cursor_i >= item_end) {
                    item_start++;
                    item_end++;
                }
            } else if (ch == KEY_NPAGE && cursor_i < sb->n - 1) {
                cursor_i = MIN(cursor_i + n_items, sb->n - 1);
                item_start = MAX(0, MIN(item_start + n_items, sb->n - 1 - n_items));
                item_end = MIN(item_end + n_items, sb->n - 1 - n_items);
            }
                
            last_ch = ch;
            bl_layout_select_box_redraw_list(win, sb, cursor_i, item_start, item_end);
        }
        usleep(50);
    }
    delwin(win);
    return 0;
}

void
bl_layout_navigate_matrix(bl_matrix_ui_t matrix, bl_layout_t *layout) {
    int col = 0;
    int row = 0;
    int col_last = 0;
    int row_last = 0;
    int layer = 0;

    int ch = getch();
    int last_ch = ch;
    draw_matrix_cell(&matrix[layer][col][row], col, row, TRUE);
    while (ch != KEY_F(12)) {
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
        } else if (ch == '\n') {
            select_box_t *sb = &matrix[layer][row][col];
            int i = bl_layout_select_box(sb, col + 4, row  * (SELECT_BOX_WIDTH + 1) + 4);
            layout->matrix[layer][row][col] = *((uint16_t*) sb->items[sb->selected_item_index].data);
            redrawwin(stdscr);
        } else {
            if (ch != last_ch) {
                mvprintw(0, 1, "key=%d", ch);
                last_ch = ch;
            }
        }
        if (row != row_last || col != col_last) {
            draw_matrix_cell(&matrix[layer][row_last][col_last], col_last, row_last, FALSE);
            draw_matrix_cell(&matrix[layer][row][col], col, row, TRUE);
            col_last = col;
            row_last = row;
        }
        mvprintw(0, 0, "col: %d, row: %d, val: %u  ", col, row, layout->matrix[layer][row][col]);
        refresh();
        // don't hog the cpu too much
        usleep(50);
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
bl_layout_configure(uint8_t nlayers, char *p_layout_array_keyfile) {

    bl_layout_t layout;
    select_box_value_t bl_key_mapping_items[n_key_mappings + 1];
    static int not_selected_value = 0;
    
    /*
     * Construct the select box list for every select box in the 
     * matrix from the list of keycodes. 
     *
     * The select box is read only and will be shared.
     *
     * The first entry is for the case the mapping has (yet) been
     * defined.
     */
    bl_key_mapping_items[0].label = strdup("--");
    bl_key_mapping_items[0].data = &not_selected_value;
    for (int i=0; i<n_key_mappings; i++) {
      bl_key_mapping_items[i+1].label = bl_key_mapping[i].name;
      bl_key_mapping_items[i+1].data = &bl_key_mapping[i].hid;
    }
    
    if (bl_tui_init()) {
        bl_matrix_ui_t matrix;
        bl_layout_read(&layout);
        bl_layout_init_matrix(matrix, &layout, bl_key_mapping_items, n_key_mappings+1);
        bl_layout_draw_keyboard_matrix(matrix);
        bl_layout_navigate_matrix(matrix, &layout);
        bl_tui_exit();
    }
}
