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
    key_t key = 1234;

    struct msgbuf sbuf;
    sbuf.mtype = 1;
    strcpy(sbuf.mtext, "did you get this?");


    // need total processes generated
    // need total time elapsed (timer)

    //int qid = get_shared_memory(key);

    int qid = msgget(key, IPC_CREAT | 0666);

    if (qid == -1) {
        perror("msgget");
        exit(1);
    }



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
            char* queue_id = get_qid(key);
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
            proc_count -= 1;
        }
        sleep(1);

    }

    if (msgsnd(qid, &sbuf, MSGSZ+1, IPC_NOWAIT) < 0) {
        printf("%d, %d, %s, %d\n", qid, sbuf.mtype, sbuf.mtext, MSGSZ);
        perror("msgsnd");
        exit(1);
    }
   else {
       printf("Message: \"%s\" Sent\n", sbuf.mtext);
   }


    sleep(1);
    return 0;

}

void wait_for_all_children() {
    pid_t childpid;
    while  ( (childpid = wait(NULL) ) > 0) {
        printf("Child exited. pid: %d\n", childpid);
    }
}

char* get_qid(int qid) {
    char* queue_id = malloc(sizeof(char)*3);
    sprintf(queue_id, "%d", (qid));
    return queue_id;
}