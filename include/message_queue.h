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

struct termlog {
    long mtype;
    pid_t pid;
    struct clock termtime;
    int duration;
};

int get_message_queue();
void remove_message_queue(int msgqid);
void receive_message(int msgqid, struct msgbuf* rbuf);
void send_message(int msgqid, struct msgbuf* sbuf);

#endif
