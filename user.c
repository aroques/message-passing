#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include "global_constants.h"
#include "helpers.h"
#include "shared_memory.h"

int get_duration();

int main (int argc, char *argv[]) {
    srand(time(NULL)); // Only seed once
   
    struct msgbuf rbuf;
    int duration = get_duration();
    printf("hello world!\n");
    printf("duration %d!\n", duration);
    key_t key = 1234;
    int qid;
    printf("key: %d\n", key);

    if ((qid = msgget(key, IPC_CREAT | 0666)) < 0) {
        perror("msgget\n");
        exit(1);
    }

    printf("waiting to rcv\n");
    if (msgrcv(qid, &rbuf, MSGSZ, 1, 0) < 0) {
        perror("msgrcv\n");
        exit(1);
    }

    /*
     * Print the answer.
     */
    printf("test\n");
    printf("msg rcv: %s\n", rbuf.mtext);

    // Need access to shared clock

    return 0;  
}

int get_duration() {
    return rand() % USER_RUNTIME;
}
