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

#ifndef __BL_TUI_H__
#define __BL_TUI_H__ 1

#ifdef __CYGWIN__
#include "curses.h"
#else
#include <ncurses.h>
#endif

#include <ctype.h>

#include "bl_io.h"

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
    /*
     * cursor position in string
     */
    int pos;
    /*
     * first character to be displayed
     */
    int scroll;
    int maxlength;
} bl_tui_textbox_t;

typedef struct bl_tui_select_box_value_t {
    char *label;
    void *data;
} bl_tui_select_box_value_t;

typedef struct select_box_t {
    char *title;
    int n;
    int selected_item_index;
    // Width when displaying the select "button"
    int width;
    /*
     * Width when display the popup, if 0 take the max width of the
     * item labels.
     */
    int popup_width;
    bl_tui_select_box_value_t *items;
} select_box_t;


/**
 * Exit the TUI system, restoring the terminal
 */
void bl_tui_exit();

/**
 * Initialize the TUI system
 */
int bl_tui_init();

bl_tui_textbox_t *bl_tui_textbox_create(WINDOW *parent_win, char *label, int x, int y, int width, int maxlength);

void bl_tui_textbox_destroy(bl_tui_textbox_t *textbox);

bl_tui_button_t *bl_tui_buttons_create(int x, int y, char *labels[], int n);

void bl_tui_buttons_destroy(bl_tui_button_t *buttons, int n);

void bl_tui_buttons_select(bl_tui_button_t button);

void bl_tui_buttons_deselect(bl_tui_button_t button);

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
int bl_tui_buttons(int x, int y, char *labels[], int n);

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
int bl_tui_confirm_or_msg(int width, int height, int is_confirm, char *title, char *msg, ...);

int bl_tui_confirm(int width, int height, char *title, char *msg, ...);

int bl_tui_msg(int width, int height, char *title, char *msg, ...);

/**
 * Enter a string in a text box, returns the string entered, or null if
 * Cancel was pressed. If a string is returned, i.e. the returned value is
 * not NULL, it must be freed after use.
 *
 * @param title Title for the popup
 * @param label Label text for the input field
 * @param x x coordinate
 * @param y y coordinate
 * @param width width of the text input box (i.e. the visible part of the string)
 * @param maxlength Maximum length of the input
 *
 * @return if OK was clicked return the string entered, if Cancel was clicked
 *         ESC was pressed return NULL.
 *         If a string is returned it must be freed after use.
 */
char *bl_tui_textbox(char *title, char *label, int x, int y, int width, int maxlength);

/**
 * Create a select box and return it. The select box must be freed after use.
 *
 * @param title Title of the popup, if NULL not used.
 * @param items List of items to be shown in the select box.
 * @param n Number of items
 * @param width Width of the select box when drawn.
 * @param popup_width Width of the select box's popup window, if 0 the max of the
 *                    length of the labels is used.
 *
 * @return The select box, must be freed after used.
 */
select_box_t *bl_tui_select_box_create(char *title, bl_tui_select_box_value_t *items, int n, int width, int popup_width);

void bl_tui_select_box_destroy(select_box_t *sb);

/**
 * Draw the select box, with the selected item inversed
 * and printed as the first element.
 *
 * @param sb selectbox to draw
 * @param x x coordinate (column)
 * @param y y coordinate (row)
 * @param inversed if true draw inversed, otherwise draw normally
 */
void bl_tui_select_box_draw(select_box_t *sb, int x, int y, int inversed);

void bl_tui_select_box_redraw_list(WINDOW *win, select_box_t *sb,
                                 int cursor_i, int item_start, int item_end);
/**
 * Show a popup at the given coordinates and select a value from the
 * select box using the arrow keys and enter key.
 *
 * @param sb Select box variable that was created earlier by using
 *           @link(bl_tui_textbox_create())
 * @param x x coordinate where to put the popup.
 * @param y y coordinate where to put the popup.
 *
 * @return if a value was selected, returns TRUE, if ESC was pressed,
 *         return FALSE.
 */
int bl_tui_select_box(select_box_t *sb, int x, int y);

/**
 * Show a file selector, starting in the directory given by dname.
 * If enter is pressed over a file, that filename is returned, if ESC
 * is pressed NULL is returned.
 *
 * @param dname Directory name where to start listing files.
 *
 * @return Filename that was selected or NULL if ESC was pressed. If not NULL,
 *         the string must be freed after use.
 */
bl_io_dirent_t *bl_tui_fselect(char *dname);

#endif /* __BL_TUI_H_ */
