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
    char timer_text[2];
    puts("Timer running, time");
    printf("%d\n", time_remaining);
    GtkLabel *timelabel =
        GTK_LABEL(gtk_builder_get_object(builder, "text_timerremaining"));
    snprintf(timer_text, 100, "%i", time_remaining);
    gtk_label_set_text(timelabel, timer_text);
  }

  printf("Round %d\n", roundFunction());

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
  GtkButton *btn = GTK_BUTTON(widget);
  gchar *value = (gchar *)gtk_button_get_label(widget);
  char *end;

  puts("Bet clicked");
  printf("value button : %s\n", value);
  printf("str to ul %lu\n", strtoul(value, &end, 10));

  clientData.currentBet = strtoul(value, &end, 10);
  printf("currentbet : %lu\n", clientData.currentBet);
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
  GtkButton *btn = GTK_BUTTON(widget);
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
  GtkWidget *win;
  int game = 0;
  int sockfd;
  int oui;
  pthread_t thread;

  sockfd = open_connection();
  puts("Client alive");
  // phthread created for reading
  pthread_create(&thread, 0, threadProcess, &sockfd);
  pthread_detach(thread);

  // phthread created to launch the GUI
  pthread_create(&thread, 0, threadGame, &sockfd);
  pthread_detach(thread);

  printf("game --> %d", game);

  while (1) {
    read(sockfd, game, sizeof(int));
    if (game == 1) {
      break;
    }
  }
  if (game == 1) {

    puts("Game is launched !");

    gtk_init(&argc, &argv);
    clientData.baseMoney = 500;
    builder = gtk_builder_new_from_file("./glade/Interface.glade");
    win = GTK_WIDGET(gtk_builder_get_object(builder, "app_win"));
    builder = gtk_builder_new_from_file("./glade/Prisoner.glade");
    win = GTK_WIDGET(gtk_builder_get_object(builder, "app_prisoner"));

    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show(win);
    if (time_remaining > 0) {
      g_timeout_add(1000, (GSourceFunc)start_countdown, NULL);
      start_countdown();
    }
    gtk_main();
    return (EXIT_SUCCESS);
  }
}