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

void closeAll();
void sendData();
void receiveData();
int initRound();
void on_window_main_destroy();
int start_countdown();
void on_bet_clicked(GtkWidget *widget);
void on_action_clicked(GtkWidget *widget);
void start_gtk_gui(int *ac, char ***av);
int main(int argc, char **argv);

void *threadProcess(void *ptr);
int open_connection();

typedef struct s_clientData {
  unsigned long currentBet;
  unsigned long baseMoney;
  bool cooperate;
  bool gameOn;
} s_clientData;

typedef struct dataSentReceived {
  unsigned long currentBet;
  bool cooperate; // 1 collaborer     0 trahir
  bool gameEnded;
  bool gameStarted;
} dataSentReceived;

#endif // MAIN_H
