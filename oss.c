#include <stdio.h>
#include <sys/wait.h>
#include <getopt.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <locale.h>

#include "global_constants.h"
#include "helpers.h"
#include "message_queue.h"

void wait_for_all_children();
char* get_msgqid(int msgqid);
void add_signal_handlers();
void handle_sigint(int sig);
void handle_sigalrm(int sig);
void cleanup_and_exit();
void fork_child(char** execv_arr, int num_procs_spawned);

// Globals used in signal handler
int simulated_clock_id, termination_log_id;
int cleaning_up = 0;
pid_t* childpids;
FILE *fp;

int main (int argc, char *argv[]) {
    set_timer(TIMER_DURATION);
    add_signal_handlers();

    setlocale(LC_NUMERIC, "");  // For comma separated integers in printf

    int n = parse_cmd_line_args(argc, argv);
    if (n == 0) {
        n = 1;
    }
    int proc_limit = 5;
    int proc_count = 0;                // Number of concurrent children
    childpids = malloc(sizeof(pid_t) * TOTAL_PROC_LIMIT);
    int num_procs_spawned = 0;


    if ((fp = fopen("./oss.log", "w")) == NULL) {
        perror("fopen");
        exit(1);
    }

    struct sysclock sysclock = {.mtype = 1, .clock.seconds = 0, .clock.nanoseconds = 0};
    struct termlog termlog;
    simulated_clock_id = get_message_queue();
    termination_log_id = get_message_queue();

    update_clock(simulated_clock_id, &sysclock);

    char* execv_arr[EXECV_SIZE];
    execv_arr[0] = "./user";
    execv_arr[EXECV_SIZE - 1] = NULL;

    for (num_procs_spawned = 0; num_procs_spawned < proc_limit; num_procs_spawned++) {
        fork_child(execv_arr, num_procs_spawned);
        proc_count += 1;
    }

    while ( (num_procs_spawned < TOTAL_PROC_LIMIT) && (sysclock.clock.seconds < 2) ) {

        if (proc_count == proc_limit) {
            // Receive
            read_termlog(termination_log_id, &termlog);
            read_clock(simulated_clock_id, &sysclock);

            printf("Master: Child pid %d is terminating at my time %d:%'d because it reached %d:%'d, which lived for time %d:%'d\n",
                    termlog.pid, sysclock.clock.seconds, sysclock.clock.nanoseconds,
                    termlog.termtime.seconds, termlog.termtime.nanoseconds,
                    0, termlog.duration);
            fprintf(fp, "Master: Child pid %d is terminating at my time %d:%'d because it reached %d:%'d, which lived for time %d:%'d\n",
                    termlog.pid, sysclock.clock.seconds, sysclock.clock.nanoseconds,
                    termlog.termtime.seconds, termlog.termtime.nanoseconds,
                    0, termlog.duration);
            proc_count -= 1;

            increment_sysclock(&sysclock, 100);

            fork_child(execv_arr, num_procs_spawned);
            printf("Master: Creating new child pid %d at my time %d:%'d\n",
                                childpids[num_procs_spawned],
                                sysclock.clock.seconds, sysclock.clock.nanoseconds);
            fprintf(fp, "Master: Creating new child pid %d at my time %d:%'d\n",
                                childpids[num_procs_spawned],
                                sysclock.clock.seconds, sysclock.clock.nanoseconds);
            proc_count += 1;
            num_procs_spawned += 1;

            // Send
            update_clock(simulated_clock_id, &sysclock);
        }

    }

    printf("Master: Exiting because 100 processes have been spawned or because two simulated clock seconds have been passed\n");
    printf("Master: Simulated clock time: %d:%'d\n",
            sysclock.clock.seconds, sysclock.clock.nanoseconds);
    printf("Master: %d processes spawned\n", num_procs_spawned);
    fprintf(fp, "Master: Exiting because 100 processes have been spawned or because two simulated clock seconds have been passed\n");
    fprintf(fp, "Master: Simulated clock time: %d:%'d\n",
            sysclock.clock.seconds, sysclock.clock.nanoseconds);
    fprintf(fp, "Master: %d processes spawned\n", num_procs_spawned);
    cleanup_and_exit();

    return 0;

}

void fork_child(char** execv_arr, int idx) {
    if ((childpids[idx] = fork()) == 0) {
        // Child so...
        char sysclock_id[10];
        char termlog_id[10];
        sprintf(sysclock_id, "%d", simulated_clock_id);
        sprintf(termlog_id, "%d", termination_log_id);
        execv_arr[SYSCLOCK_ID_IDX] = sysclock_id;
        execv_arr[TERMLOG_ID_IDX] = termlog_id;

        execvp(execv_arr[0], execv_arr);

        perror("Child failed to execvp the command!");
        exit(1);
    }

    if (childpids[idx] == -1) {
        perror("Child failed to fork!\n");
        exit(1);
    }
}

void wait_for_all_children() {
    pid_t childpid;
    printf("Master: Waiting for all children to exit\n");
    fprintf(fp, "Master: Waiting for all children to exit\n");
    while  ( (childpid = wait(NULL) ) > 0);
}

void terminate_children() {
    printf("Master: Sending SIGTERM to all children\n");
    fprintf(fp, "Master: Sending SIGTERM to all children\n");
    int length = sizeof(childpids)/sizeof(childpids[0]);
    int i;
    for (i = 0; i < length; i++) {
        if (childpids[i] > 0) {
            if (kill(childpids[i], SIGTERM) == -1) {
                perror("kill");
                exit(1);
            }
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
    printf("\nMaster: Caught SIGINT signal %d\n", sig);
    fprintf(fp, "\nMaster: Caught SIGINT signal %d\n", sig);
    if (!cleaning_up) {
        cleaning_up = 1;
        cleanup_and_exit();
    }
}

void handle_sigalrm(int sig) {
    printf("\nMaster: Caught SIGALRM signal %d\n", sig);
    printf("Master: Timer duration: %d\n", TIMER_DURATION);
    fprintf(fp, "\nMaster: Caught SIGALRM signal %d\n", sig);
    fprintf(fp, "Master: Timer duration: %d\n", TIMER_DURATION);
    if (!cleaning_up) {
        cleaning_up = 1;
        cleanup_and_exit();
    }

}

void cleanup_and_exit() {
    terminate_children();
    wait_for_all_children();
    printf("Master: Removing message queues\n");
    fprintf(fp, "Master: Removing message queues\n");
    remove_message_queue(simulated_clock_id);
    remove_message_queue(termination_log_id);
    fclose(fp);
    exit(0);
}

