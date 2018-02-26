#ifndef MEESAGE_QUEUE_H
#define MESSAGE_QUEUE_H

struct clock {
    int seconds;
    int nanoseconds;
};

struct sysclock {
    long mtype;
    struct clock clock;
};

struct termlog {
    long mtype;
    pid_t pid;
    struct clock termtime;
    int duration;
};

int get_message_queue();
void remove_message_queue(int msgqid);
void read_clock(int msgqid, struct sysclock* rbuf);
void update_clock(int msgqid, struct sysclock* sbuf);

#endif
