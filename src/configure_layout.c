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
#include <ctype.h>

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
    { VK_ADD, "NP_Add", KP_PLUS },
    { VK_APPS, "Win Menu", KB_APP },
    { VK_BACK, "Backspace", KB_BKSPC },
    { VK_CAPITAL, "Caps Lock", KB_CAPLK },
    { VK_DECIMAL, "NP_Dot", KP_DOT },
    { VK_DELETE, "Delete", KB_DEL },
    { VK_DIVIDE, "NP_Divide", KP_SLASH },
    { VK_DOWN, "Down Arrow", KB_DOWN },
    { VK_END, "End", KB_END },
    { VK_ESCAPE, "ESC", KB_ESC },
    { VK_F1, "F1", KB_F1 },
    { VK_F10, "F10", KB_F10 },
    { VK_F11, "F11", KB_F11 },
    { VK_F12, "F12", KB_F12 },
    { VK_F2, "F2", KB_F2 },
    { VK_F3, "F3", KB_F3 },
    { VK_F4, "F4", KB_F4 },
    { VK_F5, "F5", KB_F5 },
    { VK_F6, "F6", KB_F6 },
    { VK_F7, "F7", KB_F7 },
    { VK_F8, "F8", KB_F8 },
    { VK_F9, "F9", KB_F9 },
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
    { VK_MULTIPLY, "NP_Multiply", KP_ASTRX },
    { VK_NEXT, "Page down", KB_PGDN },
    { VK_NUMLOCK, "Num Lock", KB_NUMLK },
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
    { VK_RETURN, "NP_Enter", KP_ENTER },
    { VK_RIGHT, "Right Arrow", KB_RIGHT },
    { VK_RMENU, "Right Alt", KB_RALT },
    { VK_RSHIFT, "Right Shift", KB_RSHFT },
    { VK_RWIN, "Right Win", KB_RGUI },
    { VK_SCROLL, "Scroll Lock", KB_SCRLK },
    { VK_SNAPSHOT, "Print Screen", KB_PSCRN },
    { VK_SPACE, "Space", KB_SPACE },
    { VK_SUBTRACT, "NP_Subtract", KP_MINUS },
    { VK_TAB, "Tab", KB_TAB },
    { VK_UP, "Up Arrow", KB_UP },
    { VK_PAUSE, "Pause", KB_PAUSE }
};
static int n_key_mappings = sizeof(bl_key_mapping) / sizeof(key_mapping_t);

typedef struct bl_tui_button_t {
    WINDOW *win;
    char *label;
    int x;
    int y;
} bl_tui_button_t;

typedef struct bl_tui_textbox_t {
    WINDOW *win;
    char *label;
    char *text;
    int pos;
    int maxlength;
} bl_tui_textbox_t;

/**
 * Exit the TUI system, restoring the terminal
 */
void
bl_tui_exit() {
    endwin();
}

/**
 * Initialize the TUI system
 */
int
bl_tui_init() {
    initscr();
    raw();
    keypad(stdscr, TRUE);
    noecho();
    start_color();
    nodelay(stdscr, TRUE);
    curs_set(0);

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

bl_tui_textbox_t *
bl_tui_textbox_create(WINDOW *parent_win, char *label, int x, int y, int width, int maxlength) {
    bl_tui_textbox_t *textbox = (bl_tui_textbox_t *) malloc( sizeof(bl_tui_textbox_t) );
    textbox->win = newwin(2, width, y, x);
    textbox->label = label;
    textbox->text = (char *) malloc(maxlength + 1);
    textbox->text[0] = 0;
    textbox->pos = 0;
    textbox->maxlength = maxlength;
    return textbox;
}

void
bl_tui_textbox_destroy(bl_tui_textbox_t *textbox) {
    werase(textbox->win);
    delwin(textbox->win);
    free(textbox->text);

    return;
}

bl_tui_button_t *
bl_tui_buttons_create(int x, int y, char *labels[], int n) {
    bl_tui_button_t *buttons = (bl_tui_button_t *) malloc(n * sizeof(bl_tui_button_t));
    int x_offset = 0;

    for (int i=0; i<n; i++) {
        buttons[i].win = newwin(3, strlen(labels[i]) + 2, y, x + x_offset);
        buttons[i].label = labels[i];
        buttons[i].x = x;
        buttons[i].y = y;
        box(buttons[i].win, 0, 0);
        mvwprintw(buttons[i].win, 1, 1, labels[i]);
        wrefresh(buttons[i].win);
        x_offset += strlen(labels[i]) + 3;
    }

    return buttons;
}

void
bl_tui_buttons_destroy(bl_tui_button_t *buttons, int n) {
    for (int i=0; i<n; i++) {
        werase(buttons[i].win);
        delwin(buttons[i].win);
    }
    free(buttons);
}

void
bl_tui_buttons_select(bl_tui_button_t button) {
    wattron(button.win, A_REVERSE);
    mvwprintw(button.win, 1, 1, button.label);
    wrefresh(button.win);
}

void
bl_tui_buttons_deselect(bl_tui_button_t button) {
    wattroff(button.win, A_REVERSE);
    mvwprintw(button.win, 1, 1, button.label);
    wrefresh(button.win);
}

/**
 * Display the list of buttons horizontally and return the index of the button
 * that was clicked on.
 *
 * @param x x coordinate of upper left of first button
 * @param y y coordinate of upper left of first button
 * @param labels char array of button labels
 * @param n length of array
 *
 * @return the index of the button that was selected (clicked)
 */
int
bl_tui_buttons(int x, int y, char *labels[], int n) {
    bl_tui_button_t *buttons = bl_tui_buttons_create(x, y, labels, n);

    int done = FALSE;
    int selected = 0;
    int old_selected = -1;
    while (!done) {
        int ch = getch();
        if (ch == '\n') {
            // select button
            done = TRUE;
        } else if (ch == '\t' || ch == KEY_RIGHT) {
            // move to next button
            selected = (selected + 1) % n;
        } else if (ch == KEY_BTAB || ch == KEY_LEFT) {
            // move to previous button
            selected = (selected + n - 1) % n;
        }
        if (selected != old_selected) {
            /*
            * redraw labels
            */
            for (int i=0; i<n; i++) {
                if (i == selected) {
                    bl_tui_buttons_select(buttons[i]);
                } else {
                    bl_tui_buttons_deselect(buttons[i]);
                }
            }
            old_selected = selected;
        }
        usleep(50);
    }

    bl_tui_buttons_destroy(buttons, n);

    return selected;
}

/**
 * Show a popup with the given text and ask for confirmation. The popup is
 * centered on the screen.
 *
 * @param width Width of the popup
 * @param height Height of the popup
 * @param is_confirm Show message and Ok/Cancel buttons, or just a message and Ok button.
 * @param msg Format string, works like printf, the caller must make the popup
 *            large enough to show all of the text. There is no scrolling.
 * @param ... The rest of the params are varargs as needed by the format string.
 *
 * @return if is_confirm is TRUE, then it returns TRUE if confirmed, FALSE if not confirmed
 *         otherwise the return value is undefined.
 */
int
bl_tui_confirm_or_msg(int width, int height, int is_confirm, char *msg, ...) {
    va_list varglist;
    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    int x = (maxx - width) / 2;
    int y = (maxy - height) / 2;

    // Add space for the borders (2), and leave space for the buttons (3 + 1)
    WINDOW *win = newwin(height + 6, width + 2, y, x);
    box(win, 0, 0);
    mvprintw(y + 1, x + 1, msg, varglist);
    touchwin(win);
    wrefresh(win);

    char *buttons_confirm[] = { "Ok", "Cancel" };
    char *buttons_ok[] = { "Ok" };
    char **buttons = is_confirm ? buttons_confirm : buttons_ok;
    int n_buttons = is_confirm ? 2 : 1;

    int answer = -1;
    while (answer != FALSE && answer != TRUE) {
        int button = bl_tui_buttons(x+1, y+1+height+1, buttons, n_buttons);
        if (button == 0) {
            answer = TRUE;
        } else if (button == 1) {
            answer = FALSE;
        }
        usleep(50);
    }
    wclear(win);
    wrefresh(win);
    delwin(win);

    return answer;
}

int
bl_tui_confirm(int width, int height, char *msg, ...) {
    va_list varglist;
    return bl_tui_confirm_or_msg(width, height, TRUE, msg, varglist);
}

int
bl_tui_msg(int width, int height, char *msg, ...) {
    va_list varglist;
    return bl_tui_confirm_or_msg(width, height, FALSE, msg, varglist);
}

/**
 * Enter a string in a text box, returns the string entered, or null if
 * Cancel was pressed. If a string is returned, i.e. the returned value is
 * not NULL, it must be freed after use.
 *
 * @param x x coordinate
 * @param y y coordinate
 * @param width width of the text boxes
 * @param height of the text box
 *
 * @return if OK was clicked return the string entered, if Cancel was clicked
 *         ESC was pressed return NULL.
 */
char *
bl_tui_textbox(char *label, int x, int y, int width, int maxlength) {
    int total_width = 1 + strlen(label) + 3 + width + 1;
    WINDOW *win = newwin(7, total_width, y, x);
    box(win, 0, 0);
    touchwin(win);
    wrefresh(win);

    char *labels[] = { "Ok", "Cancel" };
    int n = 2;
    bl_tui_button_t *buttons = bl_tui_buttons_create(x+1, y+3, labels, n);
    int done = FALSE;
    int selected = 0;
    int old_selected = -1;

    bl_tui_textbox_t *textbox = bl_tui_textbox_create(win, label, x+1, y+1, total_width-2, maxlength);
    mvwprintw(textbox->win, 0, 0, "%s: %s", textbox->label, textbox->text);
    wrefresh(textbox->win);

    const int IN_BUTTONS = 0;
    const int IN_TEXT = 1;
    int state = IN_BUTTONS;
    int ch;
    while (!done) {
        ch = getch();
        if (state == IN_BUTTONS) {
            if (ch == '\n') {
                // select button
                done = TRUE;
            } else if (ch == 27) {
                selected = 1;
                done = TRUE;
            } else if (ch == '\t' || ch == KEY_RIGHT) {
                // move to next button
                selected = (selected + 1) % n;
            } else if (ch == KEY_BTAB || ch == KEY_LEFT) {
                // move to previous button
                selected = (selected + n - 1) % n;
            } else if (ch == KEY_UP || ch == KEY_DOWN) {
                bl_tui_buttons_deselect(buttons[selected]);
                state = IN_TEXT;
            }
            if (selected != old_selected) {
                /*
                * redraw labels
                */
                for (int i=0; i<n; i++) {
                    if (i == selected) {
                        bl_tui_buttons_select(buttons[i]);
                    } else {
                        bl_tui_buttons_deselect(buttons[i]);
                    }
                }
                old_selected = selected;
            }
        } else if (state == IN_TEXT) {
            // edit text
            if (ch == '\n') {
                // select button
                done = TRUE;
            } else if (ch == 27) {
                selected = 1;
                done = TRUE;
            } else if (ch == KEY_UP || ch == KEY_DOWN) {
                bl_tui_buttons_select(buttons[selected]);
                state = IN_BUTTONS;
            } else if (ch == KEY_RIGHT) {
                if (textbox->pos < strlen(textbox->text)) {
                    textbox->pos++;
                }
            } else if (ch == KEY_LEFT) {
                if (textbox->pos > 0) {
                    textbox->pos--;
                }
            } else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') {
                if (textbox->pos > 0) {
                    textbox->pos--;
                    strcpy(&textbox->text[textbox->pos], &textbox->text[textbox->pos+1]);
                }
            } else if (ch == KEY_DC) {
                strcpy(&textbox->text[textbox->pos], &textbox->text[textbox->pos+1]);
                textbox->text[strlen(textbox->text)] = 0;
            } else if (isprint(ch)) {
                if (strlen(textbox->text) < textbox->maxlength) {
                    strcpy(&textbox->text[textbox->pos+1], &textbox->text[textbox->pos]);
                    textbox->text[textbox->pos] = ch;
                    textbox->pos++;
                }
            }
            werase(textbox->win);
            mvwprintw(textbox->win, 0, 0, "%s: %s", textbox->label, textbox->text);
            wattron(textbox->win, A_REVERSE);
            if (textbox->pos < strlen(textbox->text)) {
                mvwprintw(textbox->win, 0, strlen(label) + 2 + textbox->pos, "%c", textbox->text[textbox->pos]);
            } else {
                mvwprintw(textbox->win, 0, strlen(label) + 2 + textbox->pos, " ");
            }
            wattroff(textbox->win, A_REVERSE);
            wrefresh(textbox->win);
        } else {
            printf("Invalid state: %d\n", state);
        }
        usleep(50);
    }

    // If Ok clicked copy text, If Cancel clicked or ESC pressed return NULL
    char *text = (selected == 0  && ch != 27) ? strdup(textbox->text) : NULL;
    bl_tui_buttons_destroy(buttons, n);
    bl_tui_textbox_destroy(textbox);
    wclear(win);
    wrefresh(win);
    delwin(win);

    return text;
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

    /*
     * Footer
     */
    int maxy = getmaxy(stdscr);
    mvprintw(maxy - 1, 0, "Enter: select key, F1: Open, F2: Save, F3: Write to ctrl, F12: Quit");
}

void
bl_layout_select_box_redraw_list(WINDOW *win, select_box_t *sb,
                                 int cursor_i, int item_start, int item_end) {
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
void
bl_layout_select_box(select_box_t *sb, int y, int x) {

    /*
     * Draw a popup box in the middle of the screen.
     */
    int maxy = getmaxy(stdscr);
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
     * 0 <= item_start <= cursor_i <= item_end <= n_items-1
     * &&
     * item_end - item_start <= h
     */
    /*
     * substract top and bottom line of box
     */
    int n_items = h - 2;
    int old_selected_item_index = sb->selected_item_index;
    int cursor_i = sb->selected_item_index;
    int item_start = sb->selected_item_index;
    int item_end = sb->n > item_start + n_items ? item_start + n_items : sb->n;
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
            } else if (ch == '\n' /* ENTER */) {
                sb->selected_item_index = cursor_i;
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
                item_start = MAX(0, MIN(item_start + n_items, sb->n - n_items));
                item_end = MIN(item_start + n_items, sb->n);
            } else if (ch == KEY_PPAGE && cursor_i > 0) {
                cursor_i = MAX(cursor_i - n_items, 0);
                item_start = MAX(item_start - n_items, 0);
                item_end = MIN(item_start + n_items, sb->n);
            }

            last_ch = ch;
            bl_layout_select_box_redraw_list(win, sb, cursor_i, item_start, item_end);
        }
        usleep(50);
    }
    delwin(win);
    return;
}

void
bl_layout_load_file() {
    char *msg = bl_tui_textbox("test title", 5, 5, 10, 10);
    printf("msg=%s\n", msg);
}

void
bl_layout_save_to_file(bl_layout_t *layout) {
    char *fname = bl_tui_textbox("Enter a name for the file", 5, 5, 20, 100);
    if (fname != NULL) {
        if (bl_layout_save(layout, fname) == 0) {
            bl_tui_msg(20, 1, "File saved");
        } else {
            bl_tui_msg(20, 1, "Error while saving");
        }
    }
}

void
bl_layout_write_to_controller() {
    bl_tui_confirm(61, 1, "Do you wish to write the new configuration to the controller?");
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
    int redraw = FALSE;
    draw_matrix_cell(&matrix[layer][row][col], col, row, TRUE);
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
            bl_layout_select_box(sb, col + 4, row  * (SELECT_BOX_WIDTH + 1) + 4);
            mvprintw(1, 0, "sel=%d\n",  *((uint16_t*) sb->items[sb->selected_item_index].data));
            layout->matrix[layer][row][col] = *((uint16_t*) sb->items[sb->selected_item_index].data);
            erase();
            redraw = TRUE;
        } else if (ch == KEY_F(1)) {
            bl_layout_load_file();
            redraw = TRUE;
        } else if (ch == KEY_F(2)) {
            bl_layout_save_to_file(layout);
            redraw = TRUE;
        } else if (ch == KEY_F(3)) {
            bl_layout_write_to_controller();
            redraw = TRUE;
        } else {
            if (ch != last_ch) {
                mvprintw(0, 1, "key=%d", ch);
                last_ch = ch;
            }
        }
        if (redraw) {
            bl_layout_draw_keyboard_matrix(matrix);
            draw_matrix_cell(&matrix[layer][row][col], col, row, TRUE);
            redraw = FALSE;
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
     * The first entry is for the case the mapping has (not yet) been
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

        /*
         * wait until key has been released and then enable service mode.
         * It would be better if we could detect key up and key down
         * events, but that doesn't work in terminal mode.
         */
        usleep(100000);
        bl_usb_enable_service_mode();

        bl_layout_read(&layout);
        bl_layout_init_matrix(matrix, &layout, bl_key_mapping_items, n_key_mappings+1);
        bl_layout_draw_keyboard_matrix(matrix);
        bl_layout_navigate_matrix(matrix, &layout);
        bl_tui_exit();
        bl_usb_disable_service_mode();
    }
}
