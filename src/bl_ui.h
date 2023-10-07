
#ifndef __BL_UI_H__
#define __BL_UI_H__ 1

/*
 * key mappings, values are defined in bl_ui_layout.c
 */
typedef struct key_mapping_struct {
    uint8_t vk;
    char *name;
    uint16_t hid;
} key_mapping_t;

extern key_mapping_t bl_key_mapping[];
extern int _n_key_mappings;

/*
 * Menu type definition
 */
typedef enum {
    BL_UI_MENU_OPEN_LAYOUT_FILE,
    BL_UI_MENU_SAVE_LAYOUT_FILE,
    BL_UI_MENU_WRITE_LAYOUT_TO_CTRL,
    BL_UI_MENU_EDIT_LAYERS,
    BL_UI_MENU_MANAGE_LAYERS,
    BL_UI_MENU_EDIT_MACROS,
    BL_UI_MENU_QUIT,
    BL_UI_MENU_UNDEFINED
} bl_ui_menu_id_t;

typedef struct bl_ui_menu_t {
    // label to display
    char *label;
    // which character to underline (shortcut), -1 if no underline needed
    int underline;
    // global shortcut key
    int key;
    // menu item identifier identifier to return
    bl_ui_menu_id_t id;
    // its children, NULL if no children
    struct bl_ui_menu_t *children;
} bl_ui_menu_t;


#define SELECT_BOX_WIDTH 8

typedef bl_tui_select_box_t *bl_matrix_ui_t[NUMLAYERS_MAX][NUMROWS][NUMCOLS];

/*
 * prototypes
 */
void bl_layout_read(bl_layout_t *layout);
void bl_layout_init_matrix(bl_matrix_ui_t matrix, bl_layout_t *layout,
                           bl_tui_select_box_value_t *bl_key_mapping_items, int n_items);
void bl_layout_draw_keyboard_matrix(WINDOW *win, bl_matrix_ui_t matrix, int layer, int nlayers);
int bl_layout_navigate_matrix(WINDOW *win, bl_matrix_ui_t matrix, bl_layout_t *layout, int layer,
							  bl_tui_select_box_value_t *bl_key_mapping_items, int n_key_mappings);

bl_layout_t *bl_layout_select_and_load_file();
void bl_layout_save_to_file(bl_layout_t *layout);
void bl_layout_write_to_controller(bl_layout_t *layout);
int bl_layout_manage_layers(bl_layout_t *layout, int *layer);

int bl_macro_navigate();

void bl_ui_loop(bl_layout_t *layout);

#endif
