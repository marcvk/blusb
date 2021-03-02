
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

#include "layout.h"
#include "vkeycodes.h"
#include "usb.h"
#include "bl_tui.h"
#include "bl_ui.h"

typedef struct bl_ui_windows_t {
    WINDOW *menu_win;
    WINDOW *content_win;
    WINDOW *footer_win;
} bl_ui_windows_t;

static bl_ui_menu_t menu[] =
{
    { "File", 0, 'f', BL_UI_MENU_UNDEFINED,
        (bl_ui_menu_t[]) {
            { "Open layout file", 0, 'o', BL_UI_MENU_OPEN_LAYOUT_FILE, NULL },
            { "Save layout file", 0, 's', BL_UI_MENU_SAVE_LAYOUT_FILE, NULL },
            { "Write layout to controller", 0, 'w', BL_UI_MENU_WRITE_LAYOUT_TO_CTRL, NULL }
        }
    },
    { "Layer", 0, 'l', BL_UI_MENU_UNDEFINED,
        (bl_ui_menu_t []) {
            { "Edit layers", -1, -1, BL_UI_MENU_EDIT_LAYERS, NULL },
            { "Manage layers", -1, -1, BL_UI_MENU_MANAGE_LAYERS, NULL }
        }
    },
    { "Macro", 0, 'm', BL_UI_MENU_UNDEFINED,
        (bl_ui_menu_t []) {
            { "Edit macros", -1, -1, BL_UI_MENU_EDIT_MACROS, NULL }
        }
    },
    { "Quit", 0, 'q', BL_UI_MENU_QUIT, NULL }
};
static int _n_menu_items = sizeof(menu) / sizeof(bl_ui_menu_t);

static void
bl_ui_setup(bl_ui_windows_t *windows) {
    int maxx = getmaxx(stdscr);
    int maxy = getmaxy(stdscr);

    assert(maxx > 1);
    assert(maxy > 2);

    windows->menu_win = newwin(1, maxx, 0, 0);
    windows->content_win = newwin(maxy-2, maxx, 1, 0);
    windows->footer_win = newwin(1, maxx, maxy, 0);

    return;
}

void
bl_ui_destroy(bl_ui_windows_t *windows) {
    delwin(windows->menu_win);
    delwin(windows->content_win);
    delwin(windows->footer_win);

    return;
}

/**
 * Draw the label of the menu item with its shorcut character
 * underlined (if any is defined).
 *
 * Pre:
 *  the cursor is positioned at the place where the label must be drawn.
 *  &&
 *  strlen(item->label) > item->underline
 *
 * @param menu_win
 * @param item
 */
void
bl_ui_menu_draw_item(WINDOW *menu_win, bl_ui_menu_t *item) {
    if (item->underline >= 0) {
        for (int i=0; i<item->underline; i++) {
            wprintw(menu_win, "%c", item->label[i]);
        }
        wattron(menu_win, A_UNDERLINE);
        wprintw(menu_win, "%c", item->label[item->underline]);
        wattroff(menu_win, A_UNDERLINE);
        for (int i=item->underline+1; i<strlen(item->label); i++) {
            wprintw(menu_win, "%c", item->label[i]);
        }
    } else {
        wprintw(menu_win, item->label);
    }
}

void
bl_ui_menu_draw(WINDOW *menu_win) {
    wmove(menu_win, 0, 0);
    wclrtoeol(menu_win);
    wattron(menu_win, A_REVERSE);
    wmove(menu_win, 0, 0);
    for (int i=0; i<_n_menu_items; i++) {
        bl_ui_menu_draw_item(menu_win, &menu[i]);
        wprintw(menu_win, " ");
    }
    wattroff(menu_win, A_REVERSE);
    wrefresh(menu_win);
}


void
bl_ui_do_file_menu(bl_layout_t *layout) {
    bl_tui_select_box_value_t items[] = {
        { "Open layout file (O)", FALSE, (void*)0 },
        { "Save layout file (S)", FALSE, (void*)1 },
        { "Write layout to controller (W)", FALSE, (void*)2 }
    };
    bl_tui_select_box_t *sb = bl_tui_select_box_create(NULL, items, 3, 8, 0);
    if (bl_tui_select_box(sb, 0, 0)) {
        switch (sb->selected_item_index) {
            case 0:
                bl_layout_select_and_load_file();
                break;
            case 1:
                bl_layout_save_to_file(layout);
                break;
            case 2:
                bl_layout_write_to_controller(layout);
                break;
            default:
                bl_tui_err(TRUE, "unsupported menu item, should not happen: %d", sb->selected_item_index);
                break;
        }
    }
    bl_tui_select_box_destroy(sb);
}

int
bl_ui_do_layer_menu(bl_layout_t *layout, int *layer) {
    bl_tui_select_box_value_t items[] = {
        { "Show layers", FALSE, (void*)0 },
        { "Number of layers", FALSE, (void*)1 }
    };
    bl_tui_select_box_t *sb = bl_tui_select_box_create(NULL, items, 2, 8, 0);
    if (bl_tui_select_box(sb, 9, 0)) {
        switch (sb->selected_item_index) {
            case 1:
                bl_layout_manage_layers(layout, layer);
                break;
            default:
                bl_tui_err(TRUE, "unsupported menu item, should not happen: %d", sb->selected_item_index);
                break;
        }
    }
    bl_tui_select_box_destroy(sb);
}

int
bl_ui_do_macro_menu() {
    bl_tui_select_box_value_t items[] = {
        { "Show macros", FALSE, (void*)0 }
    };
    bl_tui_select_box_t *sb = bl_tui_select_box_create(NULL, items, 1, 8, 0);
    if (bl_tui_select_box(sb, 18, 0)) {
        switch (sb->selected_item_index) {
            case 0:
                break;
            default:
                bl_tui_err(TRUE, "unsupported menu item, should not happen: %d", sb->selected_item_index);
                break;
        }
    }
    bl_tui_select_box_destroy(sb);
}

int
bl_ui_menu_do_menu(bl_ui_menu_t *menu) {

}

/**
 * Check if the given character matches a key for a menu item,
 * if so return the menu's id, if not return -1.
 * Only the top level menus are checked.
 *
 * @param ch key pressed
 * @return id of menu item corresponding to the key pressed, or -1 if
 *         not found.
 */
int
bl_ui_menu_handle_event(int ch, bl_layout_t *layout, int *layer) {

    for (int i=0; i<_n_menu_items; i++) {
        if (ch == menu[i].key) {
            return bl_ui_menu_do_menu(&menu[i]);
        }
    }
    return -1;
}

void
bl_ui_footer_draw(WINDOW *footer_win) {
    attron(A_REVERSE);
    wmove(footer_win, 0, 0);
    wclrtoeol(footer_win);
    wprintw(footer_win, "Enter: select key, ");
    wprintw(footer_win, "Select layer: ");
    attron(A_UNDERLINE); printw("1"); attroff(A_UNDERLINE);
    wprintw(footer_win, " - ");
    attron(A_UNDERLINE); printw("6"); attroff(A_UNDERLINE);
    attroff(A_REVERSE);
}

/**
 * Run the main loop for the UI. This is not a proper UI hierarchy / event
 * loop design, just a pragmatic approach to get the two different panels
 * working.
 *
 * @layout Initial layout to load, if NULL read the layout from the controller.
 *
 */
void
bl_ui_loop(bl_layout_t *layout) {

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
        bl_ui_windows_t windows;
        bl_ui_setup(&windows);

        bl_ui_menu_draw(windows.menu_win);
        bl_ui_footer_draw(windows.footer_win);
        bl_matrix_ui_t matrix;

        /*
         * wait until key has been released and then enable service mode.
         * If we don't do this and you use the IBM keyboard to start the program
         * the last key pressed will keep repeating (most likely the enter key).
         *
         * It would be better if we could detect key up and key down
         * events, but that doesn't work in terminal mode, so we hack around
         * it with a short sleep.
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
        int show_layers = TRUE;
        int ch = 0;
        while (ch != 'q' && ch != 'Q') {
            bl_ui_menu_draw(windows.menu_win);
            if (show_layers) {
                bl_layout_draw_keyboard_matrix(windows.content_win, matrix, 0, layout->nlayers);
                ch = bl_layout_navigate_matrix(matrix, layout, 0, bl_key_mapping_items, _n_key_mappings+1);
                show_layers = FALSE;
            } else {
                ch = bl_macro_navigate();
                show_layers = TRUE;
            }
        }
        bl_tui_exit();
        bl_usb_disable_service_mode();
    }
}
