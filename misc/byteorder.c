#include "../lib/headers.h"
#include <stdio.h>

int main() {
    union {
        short s;
        char c[sizeof(short)];
    } un;

    un.s = 0x0102;

    if (sizeof(short) == 2) {
        if (un.c[0] == 1 && un.c[1] == 2) {
            println("Big endian");
        } else if (un.c[0] == 2 && un.c[1] == 1) {
            println("Little endian");
        } else {
            println("unknown");
        }
    } else {
        println("sizeof(short): %d", sizeof(short));
    }

    println("un.s %hd, un.c[0] %d, un.c[1] %d", un.s, un.c[0], un.c[1]);
    printf("un.s %hd, un.c[0] %s\n", un.s, un.c);
}
