#include "../lib/headers.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <unistd.h>

int main() {
    int filesize = 5000;
    int mmpasize = 15000;

    int fd = open("file", O_CREAT | O_RDWR | O_TRUNC, FILE_MODE);

    if (fd < 0) {
        err_quit("open");
    }

    lseek(fd, filesize - 1, SEEK_SET);

    // write 1 byte
    write(fd, "", 1);

    char *ptr = mmap(NULL, mmpasize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (ptr == NULL) {
        err_quit("mmap");
    }

    long pagesize = sysconf(_SC_PAGESIZE);

    printf("pagesize = %lu\n", pagesize);

    int i;

    for (i = 0; i < MAX(filesize, mmpasize); i += pagesize) {
        printf("ptr[%d] = %d\n", i, ptr[i]);
        ptr[i] = 1;
        printf("ptr[%lu] = %d\n", i + pagesize - 1, ptr[i + pagesize - 1]);
        ptr[i + pagesize - 1] = 1;
    }

    printf("ptr[%d] = %d\n", i, ptr[i]);
}
