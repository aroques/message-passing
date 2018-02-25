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

    printf("duration %d!\n", duration);

    key_t key = 1234;
    int qid;
    printf("user: key: %d\n", key);

    if ((qid = msgget(key, IPC_CREAT | 0666)) < 0) {
        perror("user: msgget\n");
        exit(1);
    }

    printf("user: qid: %d\n", qid);
    printf("user: waiting to rcv\n");
    if (msgrcv(qid, &rbuf, sizeof(rbuf), 1, 0) < 0) {
        perror("user: msgrcv\n");
        exit(1);
    }

    /*
     * Print the answer.
     */
    printf("user: test\n");
    printf("user: msg rcv: %s\n", rbuf.mtext);

    // Need access to shared clock

    return 0;  
}

int get_duration() {
    return rand() % USER_RUNTIME;
}
