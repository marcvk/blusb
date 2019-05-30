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

#include "bl_io.h"

bl_io_dir_t *
bl_io_read_directoy(char *dname) {
    DIR *dir;

    dir = opendir(dname);
    if (dir == NULL) {
        errmsg_and_abort("Can't open directory: %s", dname);
        return NULL;
    }
    /*
     * Get nr of dir entries
     */
    bl_io_dir_t *io_dir = (bl_io_dir_t *) malloc(sizeof(bl_io_dir_t));
    io_dir->n = 0;
    while (readdir(dir) != NULL) {
        io_dir->n++;
    }
    rewinddir(dir);

    /*
     * Allocate and read all entries
     */
    io_dir->dirs = (bl_io_dirent_t *) malloc( io_dir->n * sizeof(bl_io_dirent_t) );
    for (int i=0; i<io_dir->n; i++) {
        struct dirent *dire = readdir(dir);
        io_dir->dirs[i].name = strdup(dire->d_name);
        lstat(io_dir->dirs[i].name, &io_dir->dirs[i].fstatus);
    }
    closedir(dir);

    return io_dir;
}

void
bl_io_dir_destroy(bl_io_dir_t *dir) {
    for (int i=0; i<dir->n; i++) {
        bl_io_dirent_destroy(&dir->dirs[i]);
    }
    free(dir->dirs);
    free(dir);

    return;
}

void
bl_io_dirent_destroy(bl_io_dirent_t *dirent) {
    free(dirent->name);
    return;
}

