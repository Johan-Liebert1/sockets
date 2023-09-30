#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("usage: msgsend <name>\n");
        return 1;
    }

    char *file_name = argv[1];

    mqd_t mqd = mq_open(file_name,  O_RDONLY);

    if (mqd < 0) {
        perror("mq_open");
        return 1;
    }

    struct mq_attr mqattr;
    uint prio;

    if (mq_getattr(mqd, &mqattr) == -1) {
        perror("mq_getattr");
        return 1;
    }

    void *buff = malloc(mqattr.mq_msgsize);

    int n = mq_receive(mqd, buff, mqattr.mq_msgsize, &prio);

    if (n < 0) {
        perror("mq_receive");
        return 1;
    }

    printf("Read %d bytes of priority %d\n", n, prio);

    return 0;
}
