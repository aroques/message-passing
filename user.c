#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/time.h>
#include <locale.h>

#include "global_constants.h"
#include "helpers.h"
#include "message_queue.h"

int get_duration();

int main (int argc, char *argv[]) {
    srand(time(NULL));
    setlocale(LC_NUMERIC, "");
    // Used to calculate work/time per round
    struct timeval  tv_start, tv_stop;
    int diffusec = 0, diffnano = 0;

    // Shared memory structures
    int sysclock_id = atoi(argv[SYSCLOCK_ID_IDX]);
    int termlog_id = atoi(argv[TERMLOG_ID_IDX]);
    struct sysclock sysclock;
    struct termlog termlog;
    termlog.mtype = 1;

    // Other variables
    int duration = get_duration();  // Total duration to run for
    int time_incremented = 0;       /* Total simulated time that this process has
                                        incremented simulated clock while running */
    int new_nano = 0;               // Number of nanoseconds to increment simulated clock

    while(1) {
        // Receive
        read_clock(sysclock_id, &sysclock);
        // Critical Section

        // Get quantity of work
        gettimeofday(&tv_start, NULL);
        gettimeofday(&tv_stop, NULL);

        // Calculate nano-seconds to increment sysclock
        diffusec = tv_stop.tv_usec - tv_start.tv_usec;
        diffnano = diffusec * 1000;
        new_nano = diffnano;

        time_incremented += new_nano;

        // Send
        if (time_incremented >= duration) {
            // Terminate and send message to master

            // Calculated corrected new_nano
            time_incremented -= new_nano;
            new_nano = duration - time_incremented;

            increment_sysclock(&sysclock, new_nano);

            // Set termination log information
            termlog.termtime.seconds = sysclock.clock.seconds;
            termlog.termtime.nanoseconds = sysclock.clock.nanoseconds;
            termlog.pid = getpid();
            termlog.duration = duration;

            // Send
            update_clock(sysclock_id, &sysclock);
            update_termlog(termlog_id, &termlog);
            break;
        }
        else {
            increment_sysclock(&sysclock, new_nano);
            // Send
            update_clock(sysclock_id, &sysclock);
        }
    }

    return 0;  
}

int get_duration() {
    return rand() % USER_RUNTIME;
}
