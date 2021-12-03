/*!
 * \file utils.c
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

/*!
 * \fn void *threadProcess(void *ptr)
 * \author GABETTE Cédric
 * \version 0.1
 * \date  26/11/2021
 * \brief Thread for connection
 * \remarks None
 * \param ptr
 */
void *threadProcess(void *ptr) {

  int len;

  dataSentReceived *sending = malloc(sizeof(dataSentReceived));
  dataSentReceived *receiving = malloc(sizeof(dataSentReceived));

  int sockfd = *((int *)ptr);
  sending->totalMoney = 500;

  write(sockfd, sending, sizeof(dataSentReceived));
  read(sockfd, receiving, sizeof(dataSentReceived));

  close(sockfd);
  puts("client pthread ended");

  return 0;
}

void *threadGame(void *ptr) { return 0; }

/*!
 * \fn int open_connection()
 * \author GABETTE Cédric
 * \version 0.1
 * \date  26/11/2021
 * \brief
 * \remarks None
 * \return
 */
int open_connection() {
  int sockfd;

  struct sockaddr_in serverAddr;
  uint16_t port = 7799;

  // Create the socket.
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  // Configure settings of the server address
  // Address family is Internet
  serverAddr.sin_family = AF_INET;
  // Set port number, using htons function
  serverAddr.sin_port = htons(port);
  // Set IP address to localhost
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  // Connect the socket to the server using the address
  if (connect(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) !=
      0) {
    puts("Fail to connect to server");
    exit(-1);
  };

  return sockfd;
}

int count = 0;

int roundFunction() {

  void sendData();

  if (count == ROUND_MAX) {
    puts("End of the game");
    exit(-1);
  }
  count++;
  return count;
}

void sendData(void *ptr) {
  dataSentReceived *sending = malloc(sizeof(dataSentReceived));
  int len;
  int sockfd = *((int *)ptr);
  puts("End of round, sending data");
  len = write(sockfd, sending, sizeof(dataSentReceived));
  if (len < 0)
    perror("Cannot send data");
}