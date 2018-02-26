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

    // Critical Section
    while(1) {
        // Receive
        receive_message(msgqid, &msgbuf);
        //struct clock clock = { .seconds = msgbuf.clock.seconds, .nanoseconds = msgbuf.clock.nanoseconds };
        //printf("user: read clock: %d:%d\n", msgbuf.clock.seconds, msgbuf.clock.nanoseconds);

        gettimeofday(&tv_start, NULL);

        gettimeofday(&tv_stop, NULL);
        //diffsec = tv_stop.tv_sec - tv_start.tv_sec;
        diffusec = tv_stop.tv_usec - tv_start.tv_usec;
        diffnano = diffusec * 1000;
        new_nano = diffnano;

        time_incremented += new_nano;

        //printf("user: diffsec: %d, diffnano: %d\n", diffsec, diffnano);
        //printf("user: diffsec: %d, new_nano: %d\n", diffsec, new_nano);

        // Send
        if (time_incremented > duration) {
            // terminate and send message to master


            time_incremented -= new_nano;
            // need to add my pid to message, and need to add termination time
            new_nano = duration - time_incremented;
            //printf("user: terminating and incrementing clock nanoseconds by: %dns\n", new_nano);
            //printf("user: incrementing clock nanoseconds by: %dns\n", new_nano);
            msgbuf.clock.nanoseconds += new_nano;
            send_message(msgqid, &msgbuf);
            break;
        }
        else {
            //printf("user%d: incrementing clock nanoseconds by: %dns\n", getpid(), new_nano);
            //printf("user%d: clock: %dns\n", getpid(), msgbuf.clock.nanoseconds);
            msgbuf.clock.nanoseconds += new_nano;
            send_message(msgqid, &msgbuf);
        }
    }
    printf("user: exiting: duration: %d\n", duration);
    printf("user: simulated system clock: %d\n", msgbuf.clock.nanoseconds);
    return 0;  
}

int get_duration() {
    return rand() % USER_RUNTIME;
}
