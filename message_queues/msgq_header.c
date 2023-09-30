#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv)  {
    int flags = O_RDWR | O_CREAT;

    int e = getopt(argc, argv, "e:");

    if (e == -1) {
        perror("getopt");
        return 1;
    }

    // this file is created inside /dev/mqueue
    mqd_t mqd = mq_open("/temp.1234", flags, 400, NULL);

    if (mqd < 0) {
        perror("mq_open");
        return 1;
    }

    mq_close(mqd);
}
