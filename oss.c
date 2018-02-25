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
void handle_sigint(int sig);

// Globals used in signal handler
int msgqid;

int main (int argc, char *argv[]) {
    signal(SIGINT, handle_sigint);

    int n = parse_cmd_line_args(argc, argv);
    if (n == 0) {
        n = 1;
    }
    int proc_limit = 5;
    int proc_count = 0;                // Number of concurrent children
    pid_t childpids[proc_limit];
    int num_procs_spawned = 0;

    struct msgbuf sbuf = {.mtype = 1, .clock.seconds = 0, .clock.nanoseconds = 0};

    // need total processes generated
    // need total time elapsed (timer)

    msgqid = get_message_queue();

    char* execv_arr[EXECV_SIZE];
    execv_arr[0] = "./user";
    execv_arr[EXECV_SIZE - 1] = NULL;

    int i;
    for (i = 0; i < proc_limit; i++) {

        if (proc_count == proc_limit) {
            // Wait for one child to finish and decrement proc_count
            wait(NULL);
            proc_count -= 1;
        }

        if ((childpids[i] = fork()) == 0) {
            // Child so...
            char msgq_id[10];
            sprintf(msgq_id, "%d", msgqid);
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

        if (msgsnd(msgqid, &sbuf, sizeof(sbuf.clock), IPC_NOWAIT) < 0) {
            printf("%d, %ld, %d:%d, %lu\n", msgqid, sbuf.mtype, sbuf.clock.seconds, sbuf.clock.nanoseconds, sizeof(sbuf.clock));
            perror("msgsnd");
            exit(1);
        }
       else {
           printf("oss : wrote to clock: %d:%d \n", sbuf.clock.seconds, sbuf.clock.nanoseconds);
       }

    }

    sleep(5);

    // Remove message queue
    remove_message_queue(msgqid);

    return 0;

}

void wait_for_all_children() {
    pid_t childpid;
    while  ( (childpid = wait(NULL) ) > 0) {
        printf("Child exited. pid: %d\n", childpid);
    }
}

void handle_sigint(int sig) {
    printf("\nCaught signal %d\n", sig);
    printf("msgqid: %d\n", msgqid);
    remove_message_queue(msgqid);
    exit(0);
}
