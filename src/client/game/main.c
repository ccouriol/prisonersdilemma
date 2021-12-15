/*!
 * \file main.c
 * \author GABETTE Cédric
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

GtkBuilder *builder = NULL;
int time_remaining = 10;
s_clientData clientData;

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
 * \author Clément Couriol
 * \version 0.1
 * \date  17/11/2021
 * \brief
 * \remarks None
 * \return
 */
int start_countdown() {
  // puts("Timer started");
  if (time_remaining > 0) {
    time_remaining--;
    char timer_text[5];
    puts("Timer running, time");
    printf("%d\n", time_remaining);
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

  puts("Bet clicked");
  clientData.currentBet = strtoul(value, &end, 10);
  printf("currentbet : %lu\n", clientData.currentBet);

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
    puts("Coop clicked");
    clientData.cooperate = true;
  } else {
    puts("Betray clicked");
    clientData.cooperate = false;
  }
  printf("clientData.cooperate : %i\n", clientData.cooperate);
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
  clientData.baseMoney = 500;
  clientData.currentBet = 25;
  clientData.cooperate = true;
  builder = gtk_builder_new_from_file("./glade/Prisoner.glade");
  win = GTK_WIDGET(gtk_builder_get_object(builder, "app_prisoner"));

  gtk_builder_connect_signals(builder, NULL);
  gtk_widget_show(win);
  if (time_remaining > 0) {
    g_timeout_add(1000, (GSourceFunc)start_countdown, NULL);
    start_countdown();
  }

  GtkLabel *betlabel =
      GTK_LABEL(gtk_builder_get_object(builder, "current_bet_label"));
  snprintf(bet_text, 5, "%i", (int)clientData.currentBet);
  gtk_label_set_text(betlabel, bet_text);
  gtk_main();
}

/*!
 * \fn int main(int argc, char **argv)
 * \author Clément Couriol
 * \version 0.1
 * \date 17/11/2021
 * \brief Main function of the program
 * \remarks None
 * \param argc
 * \param *argv
 * \return 0 if all went good
 */
int main(int argc, char **argv) {
  int sockfd;
  long int status = 0;
  char msg[100];
  pthread_t thread;

  sockfd = open_connection();
  strcpy(msg, "Hello from Player1"); // Xeon is the name of the this client
  printf("sending : %s\n", msg);
  write(sockfd, msg, strlen(msg));

  // phthread created for reading
  pthread_create(&thread, 0, threadProcess, &sockfd);
  // write(connection->sock,"Main APP Still running",15);
  pthread_detach(thread);
  do {
    fgets(msg, 100, stdin);
    // printf("sending : %s\n", msg);
    status = write(sockfd, msg, strlen(msg));
    // memset(msg,'\0',100);
  } while (status != -1);

  status = 0;

  // phthread created to launch the game
  pthread_create(&thread, 0, threadIsGame, &sockfd);
  // write(connection->sock, "<IdGame>")
  pthread_detach(thread);
  do {
    fgets(msg, 100, stdin);
    // printf("sending : %s\n", msg);
    status = write(sockfd, msg, strlen(msg));
    // memset(msg,'\0',100);
  } while (status != -1);

  char *ip = read_config("ip");
  printf("\nip= %s\n", ip);
  char *port = read_config("port");
  printf("\nport= %s\n", port);
  char *rounds = read_config("rounds");
  printf("\nport= %s\n", rounds);
  char *basemoney = read_config("basemoney");
  printf("\nport= %s\n", basemoney);
  int a = verifyIP("caca");
  printf("ipverif: %d\n", a);
  start_gtk_gui(&argc, &argv);

  return (EXIT_SUCCESS);
}
