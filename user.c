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

    printf("user: duration: %d\n", duration);

    int msgq_id = atoi(argv[MSGQ_ID_IDX]);

    printf("user: waiting to rcv msg\n");
    if (msgrcv(msgq_id, &rbuf, sizeof(rbuf.clock), 1, 0) < 0) {
        perror("user: msgrcv");
        exit(1);
    }


    printf("user: read clock: %d:%d\n", rbuf.clock.seconds, rbuf.clock.nanoseconds);

    return 0;  
}

int get_duration() {
    return rand() % USER_RUNTIME;
}
