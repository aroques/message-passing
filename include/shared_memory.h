#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define MSGSZ 60

struct clock {
    int seconds;
    int nanoseconds;
};

struct msgbuf {
    int mtype;
    char mtext[MSGSZ];
};

int get_shared_memory(key_t key);

#endif
