/*!
 * \file main.c
 * \author Cédric gabette
 * \version 0.1
 * \date 26/11/2021
 * \brief
 * \remarks None
 *
 *
 *
 */

/*! Importation of librairies*/
#include "../../../include/client/game/main.h"
#include "../../../include/client/game/utils.h"

GtkBuilder *builder = NULL;
int time_remaining = 15;
int len;
int sockfd;
int roundLeft;
int i = 0;
bool gamEnded = false;
s_clientData clientData;
dataSentReceived *sending;

void closeAll() {

  close(sockfd);
  free(sending);
  gtk_main_quit();
}

/*!
 * \fn void sendData()
 * \author GABETTE Cédric
 * \version 0.1
 * \date  17/12/2021
 * \brief Send data to the server
 * \remarks None
 */
void sendData() {

  puts("Sending data to server");
  sending->cooperate = clientData.cooperate;
  sending->currentBet = clientData.currentBet;
#if DEBUG
  printf("SENDING----------------- -----------------------\n");
  printf("cooperate : %d\n", sending->cooperate);
  printf("currentBet : %ld\n", sending->currentBet);
  printf("END SENDING------------------------------------\n");
#endif

  write(sockfd, sending, sizeof(dataSentReceived));
}

void receiveData() {

  clientData.gameOn = isGameStarted;

#if DEBUG
  printf("RECEIVING----------------------------------------\n");
  printf("Status game : %d\n", isGameStarted);
  printf("FINISH ? %d\n", isGameFinished);
  printf("END RECEIVING------------------------------------\n");
#endif
  if (isGameFinished) {
    puts("Closing the program");
    closeAll();
    return;
  }
}

/*!
 * \fn int initRound()
 * \author GABETTE Cédric
 * \version 0.1
 * \date  17/12/2021
 * \brief Get the number of rounds from the config file
 * \remarks None
 * \return
 */
int initRound() {
  roundLeft = atoi(read_config("rounds"));
  return roundLeft;
}

/*!
 * \fn void on_window_main_destroy()
 * \author Clément Couriol
 * \version 0.1
 * \date  17/11/2021
 * \brief Close window (invoked by the event anager)
 * \remarks None
 */
void on_window_main_destroy() {
  puts("Quitting");
  gtk_main_quit();
}

/*!
 * \fn int start_countdown()
 * \author Clément Couriol && Cedric Gabette
 * \version 0.1
 * \date  17/11/2021
 * \brief Countdown for round
 * \remarks This function have a while behaviour
 * \return
 */

int start_countdown() {

  if (time_remaining == 0) {
    sendData();
    roundLeft--;
    // printf("Round left : %d\n", roundLeft);

    receiveData();
    time_remaining = 15;

    // When we reach the end of the game
    if (roundLeft == 0) {
      while (!isGameFinished) {
        sleep(1);
        receiveData();
      }
    }
  }

  if (time_remaining > 0) {
    time_remaining--;
    char timer_text[5];
    // puts("Timer running, time");
    // printf("Timer running time %d\n", time_remaining);
    // For displaying on the GTK
    GtkLabel *timelabel =
        GTK_LABEL(gtk_builder_get_object(builder, "text_timerremaining"));
    snprintf(timer_text, 5, "%i", time_remaining);
    gtk_label_set_text(timelabel, timer_text);
  }
  return 1;
}

/*!
 * \fn void on_bet_clicked(GtkWidget *widget)
 * \author Clément Couriol
 * \version 0.1
 * \date  24/11/2021
 * \brief Function called when user click on a bet button
 * \remarks None
 * \param widget
 */
void on_bet_clicked(GtkWidget *widget) {
  char bet_text[5];
  GtkLabel *betlabel =
      GTK_LABEL(gtk_builder_get_object(builder, "current_bet_label"));
  gchar *value = (gchar *)gtk_button_get_label(widget);
  char *end;

  // puts("Bet clicked");
  clientData.currentBet = strtoul(value, &end, 10);
  // printf("currentbet : %lu\n", clientData.currentBet);

  snprintf(bet_text, 5, "%i", atoi(value));
  gtk_label_set_text(betlabel, bet_text);
}

/*!
 * \fn void on_action_clicked(GtkWidget *widget)
 * \author Clément Couriol
 * \version 0.1
 * \date  24/11/2021
 * \brief Function called when user click on coop or betray button
 * \remarks None
 * \param widget
 */
void on_action_clicked(GtkWidget *widget) {
  gchar *value = (gchar *)gtk_button_get_label(widget);
  if (!(strcmp(value, "Coop"))) {
    // puts("Coop clicked");
    clientData.cooperate = true;
  } else {
    // puts("Betray clicked");
    clientData.cooperate = false;
  }
  // printf("clientData.cooperate : %i\n", clientData.cooperate);
}

/*!
 * \fn void start_gtk_gui(int *ac, char ***av)
 * \author Clément Couriol
 * \version 0.1
 * \date  03/12/2021
 * \brief
 * \remarks None
 * \param ac
 * \param **av
 */
void start_gtk_gui(int *ac, char ***av) {
  GtkWidget *win;
  char bet_text[5];

  gtk_init(ac, av);
  // Default choice
  clientData.currentBet = 25;
  clientData.cooperate = true;
  builder = gtk_builder_new_from_file("./glade/Prisoner.glade");
  win = GTK_WIDGET(gtk_builder_get_object(builder, "app_prisoner"));

  gtk_builder_connect_signals(builder, NULL);
  gtk_widget_show(win);
  if (time_remaining >= 0) {
    g_timeout_add(1000, (GSourceFunc)start_countdown, NULL);
  }

  GtkLabel *betlabel =
      GTK_LABEL(gtk_builder_get_object(builder, "current_bet_label"));
  snprintf(bet_text, 5, "%i", (int)clientData.currentBet);
  gtk_label_set_text(betlabel, bet_text);
  gtk_main();
}

/*!
 * \fn int main(int argc, char **argv)
 * \author Clément Couriol && Cedric Gabette
 * \version 0.1
 * \date 17/11/2021
 * \brief Main function of the program
 * \remarks None
 * \param argc
 * \param *argv
 * \return 0 if all went good
 */

int main(int argc, char **argv) {

  roundLeft = initRound();
  pthread_t thread;
  sending = malloc(sizeof(dataSentReceived));

  // REGEX display
  char *ip = read_config("ip");
  // printf("\nip= %s\n", ip);
  char *port = read_config("port");
  // printf("\nport= %s\n", port);

  // Verify if the server's IP is good
  int IP = verifyIP("test");
  // printf("ipverif: %d\n", a);
  if (IP == 1) {
    sockfd = open_connection();
  } else {
    perror("Connection timed fault : IP");
    EXIT_FAILURE;
    return (-1);
  }

  pthread_create(&thread, 0, threadProcess, &sockfd);
  pthread_detach(thread);

  // Waiting for server to initialize a game instance
  do {
    // if ((len = read(sockfd, receiving, sizeof(dataSentReceived)) > 0)) {
    //   printf("Status gameStarted : %d\n", receiving->gameStarted);
    //   if (receiving->gameStarted == true) {
    //     // puts("Game on !");
    //     start_gtk_gui(&argc, &argv);
    //     break;
    //   }
    // }

    if (isGameStarted) {
      // puts("Game on !");
      start_gtk_gui(&argc, &argv);
      break;
    }
  } while (1);
}