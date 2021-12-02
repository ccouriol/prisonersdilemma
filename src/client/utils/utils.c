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
#include "../../../include/client/utils/utils.h"
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
  // Il faut réserver un espace mémoire pour les structures d'échanges
  char buffer_in[BUFFERSIZE];
  char buffer_out[BUFFERSIZE];
  testStruct *envoie = malloc(sizeof(testStruct));
  testStruct *received = malloc(sizeof(testStruct));

  int sockfd = *((int *)ptr);

  /*Remplissage de la structure*/
  envoie->coop = false;
  envoie->chiffre = 159;
  strcat(envoie->msg, "Message de client");

  // On lit sur le flux d'entrée
  //            socket - structure de réception - taille
  read(sockfd, received, sizeof(testStruct));

  printf("%s\n", received->msg);
  printf("%d\n", received->chiffre);
  printf("%d\n", received->coop);

  // On écrit la structure sans passer par référence sur le flux de sortie
  write(sockfd, envoie, sizeof(testStruct));

  close(sockfd);
}

/*!
 * \fn void *threadIsGame(void *ptr)
 * \author GABETTE Cédric
 * \version 0.1
 * \date  26/11/2021
 * \brief Thread for collecting the id of the game and acknowledge it to the
 * server \remarks None \param ptr
 */
void *threadIsGame(void *ptr) {
  char buffer_in[BUFFERSIZE];
  char buffer_out[BUFFERSIZE];
  char idGame[BUFFERSIZE];
  char IsGame;
  char ask = '?';
  long int len;
  int sockfd = *((int *)ptr);

  while ((len = read(sockfd, buffer_in, BUFFERSIZE)) != 0) {
    if (recv(sockfd, idGame, BUFFERSIZE, 0) < 0) {
      puts("recv failed");
    }
    puts("replied recieved");
  }

  if (send(sockfd, idGame, strlen(idGame), 0) < 0) {
    puts("Send failed");
  }
  puts("Send recieved");

  close(sockfd);
  printf("client pthread ended, len=%ld\n", len);
}

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
  puts("connecté");

  return sockfd;
}
