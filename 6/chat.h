#ifndef CHAT_H
#define CHAT_H

#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MSG_SIZE 256
#define END_PRIORITY 99

struct message {
    long priority;
    char text[MSG_SIZE];
};

#endif