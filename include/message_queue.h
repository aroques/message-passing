#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

struct clock {
    int seconds;
    int nanoseconds;
};

struct msgbuf {
    long mtype;
    struct clock clock;
};

int get_message_queue();

#endif
