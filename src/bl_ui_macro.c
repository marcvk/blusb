#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "usb.h"
#include "bl_tui.h"

int
bl_macro_navigate() {
    clear();
    refresh();

    usleep(1000000);

    return 0;
}
