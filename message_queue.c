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

void receive_message(int msgqid, struct msgbuf* rbuf) {
    if (msgrcv(msgqid, rbuf, sizeof(rbuf->clock), 1, 0) == -1) {
        perror("msgrcv");
        exit(1);
    }
}

void send_message(int msgqid, struct msgbuf* sbuf) {
    if (msgsnd(msgqid, sbuf, sizeof(sbuf->clock), IPC_NOWAIT) < 0) {
        printf("%d, %ld, %d:%d, %lu\n", msgqid, sbuf->mtype, sbuf->clock.seconds, sbuf->clock.nanoseconds, sizeof(sbuf->clock));
        perror("msgsnd");
        exit(1);
    }
}


