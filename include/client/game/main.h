#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
// #include <glib-2.0/glib.h>
// #include <gtk-3.0/gtk/gtk.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef MAIN_H
#define MAIN_H

#define BUFFERSIZE 2048
#define ROUND_MAX 3

void *threadProcess(void *ptr);
void *threadGame(void *ptr);
int open_connection();
int roundFunction();
void sendData();

typedef struct {
  unsigned long currentBet;
  unsigned long baseMoney;
  unsigned long roundRemaining;
  bool cooperate;
} s_clientData;

typedef struct dataSentReceived {
  unsigned long currentBet;
  unsigned long moneyGainLost;
  bool cooperate; // 1 collaborer     0 trahir
  unsigned long totalMoney;
  int iDGame;
  bool gameEnded;
} dataSentReceived;


#endif // MAIN_H
