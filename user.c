#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/time.h>

#include "global_constants.h"
#include "helpers.h"
#include "message_queue.h"

int get_duration();

int main (int argc, char *argv[]) {
    struct timeval  tv_start, tv_stop;
    srand(time(NULL)); // Only seed once
   
    struct msgbuf msgbuf;
    int duration = get_duration();
    int time_incremented = 0;
    int msgqid = atoi(argv[MSGQ_ID_IDX]);
    int new_nano = 0;
    int diffusec = 0, diffnano = 0;

    printf("user: waiting on message\n");

    // Receive
    receive_message(msgqid, &msgbuf);
    struct clock clock = { .seconds = msgbuf.clock.seconds, .nanoseconds = msgbuf.clock.nanoseconds };
    printf("user: read clock: %d:%d\n", msgbuf.clock.seconds, msgbuf.clock.nanoseconds);
    // Critical Section
    while(1) {
        gettimeofday(&tv_start, NULL);

        gettimeofday(&tv_stop, NULL);
        //diffsec = tv_stop.tv_sec - tv_start.tv_sec;
        diffusec = tv_stop.tv_usec - tv_start.tv_usec;
        diffnano = diffusec * 1000;
        new_nano = diffnano + clock.nanoseconds;

        time_incremented += new_nano;

        //printf("user: diffsec: %d, diffnano: %d\n", diffsec, diffnano);
        //printf("user: diffsec: %d, new_nano: %d\n", diffsec, new_nano);

        // Send
        if (time_incremented > duration) {
            // terminate and send message to master
            printf("user: incrementing clock nanoseconds by: %dns\n", duration);
            msgbuf.clock.nanoseconds += duration;
            send_message(msgqid, &msgbuf);
            break;
        }
    }
    printf("user: sent message\n");
    return 0;  
}

int get_duration() {
    return rand() % USER_RUNTIME;
}
