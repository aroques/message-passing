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
#include "shared_memory.h"

void wait_for_all_children();
char* get_qid(int qid);

int main (int argc, char *argv[]) {
    int n = parse_cmd_line_args(argc, argv);
    if (n == 0) {
        n = 1;
    }
    int proc_count = 0;                // Number of concurrent children
    pid_t childpid;

    struct msgbuf sbuf;
    sbuf.mtype = 1;
    strcpy(sbuf.mtext, "did you get this?");

    // need total processes generated
    // need total time elapsed (timer)

    int qid = get_shared_memory();

    char* execv_arr[EXECV_SIZE];
    execv_arr[0] = "./user";
    execv_arr[EXECV_SIZE - 1] = NULL;

    int i;
    for (i = 0; i < n; i++) {

        if (proc_count == 10) {
            // Wait for one child to finish and decrement proc_count
            wait(NULL);
            proc_count -= 1;
        }

        if ((childpid = fork()) == 0) {
            // Child so...
            char queue_id[3];
            sprintf(queue_id, "%d", qid);
            execv_arr[QID_IDX] = queue_id;

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
            printf("child finished\n");
            proc_count -= 1;
        }

    }

    if (msgsnd(qid, &sbuf, sizeof(sbuf.mtext), IPC_NOWAIT) < 0) {
        printf("%d, %ld, %s, %d\n", qid, sbuf.mtype, sbuf.mtext, MSGSZ);
        perror("msgsnd");
        exit(1);
    }
   else {
       printf("Message: \"%s\" Sent\n", sbuf.mtext);
   }


    sleep(1);

    msgctl(qid, IPC_RMID, NULL);
    return 0;

}

void wait_for_all_children() {
    pid_t childpid;
    while  ( (childpid = wait(NULL) ) > 0) {
        printf("Child exited. pid: %d\n", childpid);
    }
}
