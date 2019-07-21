#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libusb.h>

int main (int argc, char **argv) {
    if (argc == 2) {
        int mode = atoi(argv[1]);     
        if (bl_usb_openctrl()) {
            bl_usb_set_mode(mode);
        }
    } else {
        printf("usage: %s <0 | 1>\n", argv[0]);
    }
}
