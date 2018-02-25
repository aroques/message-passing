#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>

#include "message_queue.h"

int get_message_queue() {
    int qid;

    qid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

    if (qid == -1) {
        perror("msgget");
        exit(1);
    }

    return qid;
}

void remove_message_queue(int msgqid) {
    if (msgctl(msgqid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(1);
    }
}

