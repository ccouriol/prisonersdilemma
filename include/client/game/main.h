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
#define ROUND_MAX 3

void *threadProcess(void *ptr);
int open_connection();

typedef struct s_clientData {
  unsigned long currentBet;
  unsigned long baseMoney;
  unsigned long roundRemaining;
  bool cooperate;
  bool gameOn;
} s_clientData;

typedef struct {
  int iDGame;
  bool gameLaunched;
} c_clientInfo;

// TODO Envoyer depuis le serveur une valeur pour dire que le jeu est lancé (int
// gameLaunched = 1)
typedef struct dataSentReceived {
  unsigned long currentBet;
  unsigned long moneyGainLost;
  bool cooperate; // 1 collaborer     0 trahir
  unsigned long totalMoney;
  unsigned short round;
  int iDGame;
  bool gameStarted;
  bool gameEnded;
} dataSentReceived;

#endif // MAIN_H
