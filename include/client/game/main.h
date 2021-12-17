// MAIN CLIENT .h

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
// #include <glib-2.0/glib.h>
// #include <gtk-3.0/gtk/gtk.h>
#include "../../../include/lib/libutil.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef MAIN_H
#define MAIN_H

#define BUFFERSIZE 2048

void *threadProcess(void *ptr);
int open_connection();

typedef struct s_clientData {
  unsigned long currentBet;
  unsigned long baseMoney;
  bool cooperate;
  bool gameOn;
} s_clientData;

// typedef struct dataSentReceived {
//   unsigned long currentBet;
//   unsigned long totalMoney;
//   unsigned short round;
//   int iDGame;
//   bool cooperate;
//   bool gameStarted;
//   bool gameEnded;
// } dataSentReceived;

typedef struct dataSentReceived {
  unsigned long currentBet;
  bool cooperate; // 1 collaborer     0 trahir
  unsigned long totalMoney;
  bool gameEnded;
  bool gameStarted;
} dataSentReceived;

#endif // MAIN_H
