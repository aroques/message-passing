#ifndef MEESAGE_QUEUE_H
#define MESSAGE_QUEUE_H

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
