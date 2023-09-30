#include <stdio.h>

#include <sys/ipc.h>

int main() {
    struct message {
        int len;
        int type;
        char *message;
    };

    struct message msg = {
        .len = 10, 
        .type = 1, 
        .message = "What up?"
    };

    printf("'%s'", (&msg)->message);
}
