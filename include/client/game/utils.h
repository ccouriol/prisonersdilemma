
#include <stdio.h>
#include <stdlib.h>

#include "../../../include/lib/libutil.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef UTILS_H
#define UTILS_H

extern bool isGameStarted;
extern bool isGameFinished;

void *threadProcess(void *ptr);
int open_connection();

#endif // UTILS_H
