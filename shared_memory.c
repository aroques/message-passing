#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>

#include "shared_memory.h"

int get_shared_memory() {
    int qid;

    qid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

    if (qid == -1) {
        perror("msgget");
        exit(1);
    }

    return qid;
}

