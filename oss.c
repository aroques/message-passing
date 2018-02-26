#include <stdio.h>
#include <sys/wait.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#include "global_constants.h"
#include "helpers.h"
#include "message_queue.h"

void wait_for_all_children();
char* get_msgqid(int msgqid);
void add_signal_handlers();
void handle_sigint(int sig);
void handle_sigalrm(int sig);
void cleanup_and_exit();

// Globals used in signal handler
int simulated_clock_id;
pid_t* childpids;

int main (int argc, char *argv[]) {
    set_timer(TIMER_DURATION);
    add_signal_handlers();

    int n = parse_cmd_line_args(argc, argv);
    if (n == 0) {
        n = 1;
    }
    int proc_limit = 5;
    int proc_count = 0;                // Number of concurrent children
    childpids = malloc(sizeof(pid_t) * 100);
    int num_procs_spawned = 0;
    //struct clock clock;

    struct sysclock msgbuf = {.mtype = 1, .clock.seconds = 0, .clock.nanoseconds = 0};
    simulated_clock_id = get_message_queue();
    update_clock(simulated_clock_id, &msgbuf);

    char* execv_arr[EXECV_SIZE];
    execv_arr[0] = "./user";
    execv_arr[EXECV_SIZE - 1] = NULL;

    int i = 0;
    while( i < 100) {
        if (proc_count == proc_limit) {
            // Wait for one child to finish and decrement proc_count
//            wait(NULL);
//            proc_count -= 1;
            // once certain number of children have been forked
            // wait for message
            //receive_message(msgqid, &msgbuf);

            printf("oss : child exited. time elapsed %d:%d\n", msgbuf.clock.seconds, msgbuf.clock.nanoseconds);

            msgbuf.clock.nanoseconds += 100;
            //clock.seconds = msgbuf.clock.seconds;
            //clock.nanoseconds = msgbuf.clock.nanoseconds;
            printf("oss : add 100 : time elapsed %d:%d\n", msgbuf.clock.seconds, msgbuf.clock.nanoseconds);
            // output contents of that message to a file

            // critical section to add 100 to the clock

            // send
            update_clock(simulated_clock_id, &msgbuf);
            // then fork off another child
            // continue until 2 seconds have past (in simulated system)
            // OR 100 processes in total have been forked off
            if (num_procs_spawned == 50) {
                break;
            }
        }

        if ((childpids[i] = fork()) == 0) {
            // Child so...
            char msgq_id[10];
            sprintf(msgq_id, "%d", simulated_clock_id);
            execv_arr[MSGQ_ID_IDX] = msgq_id;

            execvp(execv_arr[0], execv_arr);

            perror("Child failed to execvp the command!");
            return 1;
        }

        if (childpids[i] == -1) {
            perror("Child failed to fork!\n");
            return 1;
        }

        // Increment because we forked
        proc_count += 1;
        num_procs_spawned += 1;

        if (waitpid(-1, NULL, WNOHANG) > 0) {
            // A child has finished executing
            proc_count -= 1;
        }
        i += 1;
        sleep(1);
    }

    cleanup_and_exit();

    return 0;

}

void wait_for_all_children() {
    pid_t childpid;
    while  ( (childpid = wait(NULL) ) > 0) {
        printf("Child exited. pid: %d\n", childpid);
    }
}

void terminate_children() {
    int length = sizeof(childpids)/sizeof(childpids[0]);
    int i;
    for (i = 0; i < length; i++) {
        if (childpids[i] > 0) {
            kill(childpids[i], SIGTERM);
        }
    }
    free(childpids);
}

void add_signal_handlers() {
    struct sigaction act;
    act.sa_handler = handle_sigint; // Signal handler
    sigemptyset(&act.sa_mask);      // No other signals should be blocked
    act.sa_flags = 0;               // 0 so do not modify behavior
    if (sigaction(SIGINT, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

    act.sa_handler = handle_sigalrm; // Signal handler
    sigemptyset(&act.sa_mask);       // No other signals should be blocked
    if (sigaction(SIGALRM, &act, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }
}

void handle_sigint(int sig) {
    printf("\nCaught signal %d\n", sig);
    cleanup_and_exit();
}

void handle_sigalrm(int sig) {
    printf("\nCaught signal %d\n", sig);
    cleanup_and_exit();
}

void cleanup_and_exit() {
    terminate_children();
    remove_message_queue(simulated_clock_id);
    exit(0);
}

