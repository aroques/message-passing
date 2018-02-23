#include <stdio.h>
#include <sys/wait.h>
#include <getopt.h>
#include <unistd.h>

#include "global_constants.h"
#include "helpers.h"

void wait_for_all_children();

int main (int argc, char *argv[]) {
    int n = parse_cmd_line_args(argc, argv);
    if (n == 0) {
        n = 1;
    }
    int proc_count = 0;                // Number of concurrent children
    pid_t childpid;
    // need total processes generated
    // need total time elapsed (timer)

    char* execv_arr[EXECV_SIZE];
    execv_arr[0] = "./user";
    execv_arr[EXECV_SIZE - 1] = NULL;

    // Allocate shared memory for a clock (enforce CS w/ message queues)
    // Clock - 2 integers, seconds, nanoseconds and start at 0:0

    for (int i = 0; i < n; i++) { 

        if (proc_count == 10) {
            // Wait for one child to finish and decrement proc_count
            wait(NULL);
            proc_count -= 1;
        }

        if ((childpid = fork()) == 0) {
            // Child so...

            execvp(execv_arr[0], execv_arr);

            perror("Child failed to execvp the command!");
            return 1;
        }

        if (childpid == -1) {
            perror("Child failed to fork!\n");
            return 1;
        }

        proc_count += 1; // increment because we forked

        if (waitpid(-1, NULL, WNOHANG) > 0) {
            // A child has finished executing
            proc_count -= 1;
        }
        sleep(1);

    }

    return 0;

}

void wait_for_all_children() {
    pid_t childpid;
    while  ( (childpid = wait(NULL) ) > 0) {
        printf("Child exited. pid: %d\n", childpid);
    }
}
