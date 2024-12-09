#ifndef CHAT_H
#define CHAT_H


#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define QUEUE_NAME "/chat_queue"
#define MSG_SIZE 256
#define END_PRIORITY 99

#endif
