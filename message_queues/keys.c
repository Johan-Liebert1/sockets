#include "../lib/headers.h"

#include <errno.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>

#define KEY ((key_t)98765L)
#define PERMS 0666

int main() {
    int msqid;

    for (int i = 0; i < 10; i++) {
        if ((msqid = msgget(KEY, PERMS | IPC_CREAT)) < 0) {
            err_sys("Cannot create message queue");
        }

        printf("msqid = %d\n", msqid);

        if (msgctl(msqid, IPC_RMID, (struct msqid_ds *)0) < 0) {
            printf("errno = %d\n", errno);
            err_sys("Cannot remove message queue");
        }
    }
}
