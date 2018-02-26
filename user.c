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
   
    struct sysclock sysclock;
    int duration = get_duration();
    int time_incremented = 0;
    int msgqid = atoi(argv[MSGQ_ID_IDX]);
    int new_nano = 0;
    int diffusec = 0, diffnano = 0;

    // Critical Section
    while(1) {
        // Receive
        read_clock(msgqid, &sysclock);
        //struct clock clock = { .seconds = sysclock.clock.seconds, .nanoseconds = sysclock.clock.nanoseconds };
        //printf("user: read clock: %d:%d\n", sysclock.clock.seconds, sysclock.clock.nanoseconds);

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
            sysclock.clock.nanoseconds += new_nano;
            update_clock(msgqid, &sysclock);
            break;
        }
        else {
            //printf("user%d: incrementing clock nanoseconds by: %dns\n", getpid(), new_nano);
            //printf("user%d: clock: %dns\n", getpid(), sysclock.clock.nanoseconds);
            sysclock.clock.nanoseconds += new_nano;
            update_clock(msgqid, &sysclock);
        }
    }
    printf("user: exiting: duration: %d\n", duration);
    printf("user: simulated system clock: %d\n", sysclock.clock.nanoseconds);
    return 0;  
}

int get_duration() {
    return rand() % USER_RUNTIME;
}
