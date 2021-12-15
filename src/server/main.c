/**
 * @file main.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2021-11-24
 *
 * @copyright Copyright (c) 2021
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>

#include "../../include/server/header_server/server.h"

/*
 * File:   main.c
 * Author: aurelio
 *
 */

int main(int argc, char **argv) {
  int sockfd = -1;
  int index = 1;
  connection_t *connection;
  pthread_t thread;

  // init array
  init_sockets_array();
  // create socket
  sockfd = create_server_socket();

  // listen on port , stack size 50 for incoming connections
  if (listen(sockfd, 50) < 0) {
    fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
    return -5;
  }

  // Wait for connection
  while (true) {
    // accept incoming connections
    connection = (connection_t *)malloc(sizeof(connection_t));
    connection->sockfd =
        accept(sockfd, &connection->address, &connection->addr_len);
    connection->index = index++;
    if (connection->sockfd <= 0) {
      free(connection);
    } else {
      // start a new thread but do not wait for it
      // pthread_create(&thread, 0, threadProcess, (void *)connection);
      pthread_create(&thread, 0, threadServeur, (void *)connection);

      pthread_detach(thread);
    }
  }
  return (EXIT_SUCCESS);
}