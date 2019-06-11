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

#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdarg.h>

#include "blusb.h"
#include "bl_tui.h"
#include "bl_io.h"

static int _bl_tui_initialised = FALSE;

void
bl_tui_exit() {
    if (_bl_tui_initialised) {
        endwin();
        _bl_tui_initialised = FALSE;
    }
}

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

    _bl_tui_initialised = TRUE;
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
    textbox->scroll = 0;
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

int
_bl_tui_confirm_or_msg(int width, int height, int is_confirm, char *title, char *msg, va_list varglist) {
    int maxx, maxy;
    getmaxyx(stdscr, maxy, maxx);

    int x = (maxx - width) / 2;
    int y = (maxy - height) / 2;

    // Add space for the borders (2), and leave space for the buttons (3 + 1)
    WINDOW *win = newwin(height + 7, width + 2, y, x);
    box(win, 0, 0);
    mvwprintw(win, 0, width / 2 - strlen(title) / 2 - 1, " %s ", title);
    wmove(win, 2, 1);
    vw_printw(win, msg, varglist);
    touchwin(win);
    wrefresh(win);

    char *buttons_confirm[] = { "Ok", "Cancel" };
    char *buttons_ok[] = { "Ok" };
    char **buttons = is_confirm ? buttons_confirm : buttons_ok;
    int n_buttons = is_confirm ? 2 : 1;

    int answer = -1;
    while (answer != FALSE && answer != TRUE) {
        int button = bl_tui_buttons(x+1, y+2+height+1, buttons, n_buttons);
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
bl_tui_confirm_or_msg(int width, int height, int is_confirm, char *title, char *msg, ...) {
    va_list varglist;

    va_start(varglist, msg);
    int ret = _bl_tui_confirm_or_msg(width, height, is_confirm, title, msg, varglist);
    va_end(varglist);

    return ret;
}

int
bl_tui_confirm(int width, int height, char *title, char *msg, ...) {
    va_list varglist;

    va_start(varglist, msg);
    int ret = bl_tui_confirm_or_msg(width, height, TRUE, title, msg, varglist);
    va_end(varglist);

    return ret;
}

int
bl_tui_msg(int width, int height, char *title, char *msg, ...) {
    va_list varglist;

    va_start(varglist, msg);
    int ret = _bl_tui_confirm_or_msg(width, height, FALSE, title, msg, varglist);
    va_end(varglist);

    return ret;
}

char *
bl_tui_textbox(char *title, char *label, int x, int y, int width, int maxlength) {
    int total_width = 1 + strlen(label) + 3 + width + 1;
    WINDOW *win = newwin(7, total_width, y, x);
    box(win, 0, 0);
    mvwprintw(win, 0, total_width / 2 - strlen(title) / 2 - 1, " %s ", title);
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
            } else if (ch == KEY_DC /* DEL key */) {
                strcpy(&textbox->text[textbox->pos], &textbox->text[textbox->pos+1]);
                textbox->text[strlen(textbox->text)] = 0;
            } else if (ch == 1 /* ctrl-a */ || ch == KEY_HOME) {
                textbox->pos = 0;
            } else if (ch == 5 /* ctrl-e */ || ch == KEY_END) {
                textbox->pos = strlen(textbox->text);
            } else if (isprint(ch)) {
                if (strlen(textbox->text) < textbox->maxlength) {
                    strcpy(&textbox->text[textbox->pos+1], &textbox->text[textbox->pos]);
                    textbox->text[textbox->pos] = ch;
                    textbox->pos++;
                }
            }
            /*
             * Check if we need to scroll the text left or right
             */
            if (textbox->pos < textbox->scroll) {
                textbox->scroll--;
            } else if (textbox->pos >= textbox->scroll + width) {
                textbox->scroll++;
            }
            werase(textbox->win);
            char fmt[10];
            sprintf(fmt, "%%s: %%.%ds", width);
            mvwprintw(textbox->win, 0, 0, fmt, textbox->label, &textbox->text[textbox->scroll]);
            /*
             * Draw the cursor
             */
            if (state == IN_TEXT) {
                wattron(textbox->win, A_REVERSE);
            }
            if (textbox->pos < strlen(textbox->text)) {
                mvwprintw(textbox->win, 0, strlen(label) + 2 + textbox->pos - textbox->scroll, "%c", textbox->text[textbox->pos]);
            } else {
                mvwprintw(textbox->win, 0, strlen(label) + 2 + textbox->pos - textbox->scroll, " ");
            }
            if (state == IN_TEXT) {
                wattroff(textbox->win, A_REVERSE);
            }
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

select_box_t *
bl_tui_select_box_create(char *title, bl_tui_select_box_value_t *items, int n, int width, int popup_width) {
    select_box_t *sb = (select_box_t *) malloc(sizeof(select_box_t));
    if (sb == NULL) {
        errmsg_and_abort("select_box");
    } else {
        sb->title = title;
        sb->n = n;
        sb->selected_item_index = 0;
        sb->width = width;
        sb->popup_width = popup_width;
        sb->items = items;
    }
    return sb;
}

void
bl_tui_select_box_destroy(select_box_t* sb) {
    free(sb);
}


void
bl_tui_select_box_draw(select_box_t *sb, int x, int y, int inversed) {
    bl_tui_select_box_value_t *selected_item = NULL;

    if (sb->selected_item_index < sb->n) {
        selected_item = &sb->items[sb->selected_item_index];
    } else {
        errmsg_and_abort("selected index out of range: %d", sb->selected_item_index);
    }

    char *template = (char*) malloc(20 + strlen(selected_item->label));
    sprintf(template, "%%.%ds", sb->width);

    if (inversed) {
        wattron(stdscr, A_REVERSE);
    }
    mvwprintw(stdscr, y, x, template, selected_item->label);
    if (inversed) {
        wattroff(stdscr, A_REVERSE);
    }

    free(template);
}

void
bl_tui_select_box_redraw_list(WINDOW *win, select_box_t *sb,
                                 int cursor_i, int item_start, int item_end) {
    werase(win);
    box(win, 0, 0);
    if (sb->title != NULL) {
        mvwprintw(win, 0, getmaxx(win) / 2 - strlen(sb->title) / 2 - 1, " %s ", sb->title);
    }
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

int
bl_tui_select_box(select_box_t *sb, int x, int y) {

    /*
     * Draw a popup box in the middle of the screen.
     */
    int maxy = getmaxy(stdscr);
    int w = sb->popup_width;
    int h = maxy / 2 - 3;

    if (w == 0) {
        for (int i=0; i<sb->n; i++) {
            w = MAX(w, strlen(sb->items[i].label));
        }
        w = w + 2;
    }

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
    int canceled = FALSE;
    bl_tui_select_box_redraw_list(win, sb, cursor_i, item_start, item_end);
    while (selecting) {
        ch = getch();
        if (ch != last_ch) {
            if (ch == 27 /* ESC */) {
                sb->selected_item_index = old_selected_item_index;
                selecting = FALSE;
                canceled = TRUE;
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
            bl_tui_select_box_redraw_list(win, sb, cursor_i, item_start, item_end);
        }
        usleep(50);
    }
    delwin(win);

    return !canceled;
}

bl_io_dirent_t *
bl_tui_fselect(char *dname) {
    char curdir[PATH_MAX];
    getcwd(curdir, PATH_MAX);
    if (chdir(dname) != 0) {
        errmsg_and_abort("Could not change directory: %s", dname);
        return NULL;
    } else {
        bl_io_dir_t *dir = bl_io_read_directory(".");
        bl_tui_select_box_value_t *sb_values = (bl_tui_select_box_value_t *) malloc( dir->n * sizeof(bl_tui_select_box_value_t) );
        for (int i=0; i<dir->n; i++) {
            sb_values[i].label = dir->dirs[i].name;
            sb_values[i].data = &dir->dirs[i];
        }
        select_box_t *sb = bl_tui_select_box_create("Select File", sb_values, dir->n, 30, 0);
        bl_io_dirent_t *de_copy = NULL;
        if (bl_tui_select_box(sb, 5, 5)) {
            bl_io_dirent_t *de_selected = (bl_io_dirent_t *) sb->items[sb->selected_item_index].data;
	    if (de_selected == NULL) {
		/* ESC was pressed */
		de_copy = NULL;
	    } else if (S_ISDIR(de_selected->fstatus.st_mode)) {
                de_copy = bl_tui_fselect(de_selected->name);
		if (de_copy != NULL) {
		    char *fname = (char *) malloc(strlen(dname) + 1 + strlen(de_copy->name));
		    sprintf(fname, "%s/%s", de_selected->name, de_copy->name);
		    //printf("fname=%s\n", fname);
		    free(de_copy->name);
		    de_copy->name = fname;
		}
            } else {
                de_copy = (bl_io_dirent_t *) malloc(sizeof(bl_io_dirent_t));
                de_copy->name = strdup(de_selected->name);
                de_copy->fstatus = de_selected->fstatus;
            }
        } else {
            de_copy = NULL;
        }

        bl_io_dir_destroy(dir);
        bl_tui_select_box_destroy(sb);
        chdir(curdir);

        return de_copy;
    }
}

