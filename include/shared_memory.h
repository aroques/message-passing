#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define MSGSZ 60

struct clock {
    int seconds;
    int nanoseconds;
};

struct msgbuf {
    long mtype;
    char mtext[MSGSZ];
};

int get_shared_memory();

#endif
