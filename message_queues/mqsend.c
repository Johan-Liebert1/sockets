#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("usage: msgsend <name> <#bytes> <priority>\n");
        return 1;
    }

    char *file_name = argv[1];
    int len = atoi(argv[2]);
    int priority = atoi(argv[3]);

    mqd_t mqd = mq_open(file_name, O_CREAT | O_WRONLY, 400, NULL);

    if (mqd < 0) {
        perror("mq_open");
        return 1;
    }

    void *ptr = calloc(len, sizeof(char));

    int send = mq_send(mqd, ptr, len, priority);

    if (send == -1) {
        perror("mq_send");
        return 1;
    }

    return 0;
}
