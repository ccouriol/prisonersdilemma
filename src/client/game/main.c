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

unsigned int timer_id = 0;
int elapsed_time = 0;

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
 * \fn void on_button_click()
 * \author Clément Couriol
 * \version 0.1
 * \date  17/11/2021
 * \brief Function called when button "Yes" is clicked
 * \remarks None
 */
void on_button_click() {
  puts("Button 'Yes' clicked\n");
  GtkEntry *texte = GTK_ENTRY(gtk_builder_get_object(builder, "texte"));
  gchar *data = (gchar *)gtk_entry_get_text(texte);
  GtkEntry *echo = GTK_ENTRY(gtk_builder_get_object(builder, "echo"));
  gtk_entry_set_text(echo, data);
}

/*!
 * \fn int timer_handler()
 * \author Clément Couriol
 * \version 0.1
 * \date  17/11/2021
 * \brief
 * \remarks None
 * \return
 */
int timer_handler() {
  elapsed_time++;
  char txt[100];
  printf("timer running, time : %d\n", elapsed_time);
  GtkLabel *timelabel =
      GTK_LABEL(gtk_builder_get_object(builder, "time_display"));
  snprintf(txt, 100, "%04i", elapsed_time);
  gtk_label_set_text(timelabel, txt);
  return 1;
}

/*!
 * \fn void on_toogle(GtkWidget *widget, gpointer data)
 * \author Clément Couriol
 * \version 0.1
 * \date  17/11/2021
 * \brief Management of toggle_one button with stop/start of a timer
 * \remarks None
 * \param widget
 * \param data
 */
void on_toogle(GtkWidget *widget, gpointer data) {
  GtkToggleButton *btn = GTK_TOGGLE_BUTTON(widget);
  gchar *name = (gchar *)gtk_widget_get_name(widget);
  gboolean status = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widget));
  printf("toggle %s pressed, status : %d\n", name, status);
  // Activation du timer s'il etait arrete
  if (timer_id == 0) {
    timer_id = g_timeout_add(1000, (GSourceFunc)timer_handler, NULL);
  } else {
    g_source_remove(timer_id);
    timer_id = 0;
  }
}

/*!
 * \fn void on_cancel()
 * \author Clément Couriol
 * \version 0.1
 * \date  17/11/2021
 * \brief
 * \remarks None
 */
void on_cancel() {
  GtkWidget *message_dialog = gtk_message_dialog_new(
      NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_WARNING, GTK_BUTTONS_OK_CANCEL,
      "This action will cause the universe to stop existing.");
  // cf : http://refspecs.linuxbase.org/gtk/2.6/gtk/GtkMessageDialog.html
  // https://developer.gnome.org/gnome-devel-demos/stable/messagedialog.c.html.en
  int pressed = gtk_dialog_run(GTK_DIALOG(message_dialog));
  if (pressed == GTK_RESPONSE_OK) {
    puts("OK Pressed");
    puts("quitting");
    gtk_widget_destroy(message_dialog);
    gtk_main_quit();
  } else {
    puts("CANCEL Pressed");
    gtk_widget_destroy(message_dialog);
  }
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

  int sockfd;
  int status = 0;
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

  pthread_create(&thread, 0, threadData, &sockfd);

  return (EXIT_SUCCESS);

  gtk_init(&argc, &argv);
  builder = gtk_builder_new_from_file("./glade/Interface.glade");
  win = GTK_WIDGET(gtk_builder_get_object(builder, "app_win"));
  gtk_builder_connect_signals(builder, NULL);
  gtk_widget_show(win);
  gtk_main();
  return (EXIT_SUCCESS);
}
