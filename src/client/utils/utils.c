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
  int i=0;
  int len;
  int sockfd = *((int *)ptr);
  dataSentReceived *sending;
  dataSentReceived *receiving;
  s_clientData clientData;

  while (i < 180) {
    sending = malloc(sizeof(dataSentReceived));
    receiving = malloc(sizeof(dataSentReceived));

    if (len = read(sockfd, receiving, sizeof(dataSentReceived)) > 0) {
      clientData.baseMoney = receiving->totalMoney;
      clientData.gameOn = receiving->gameStarted;
      clientData.roundRemaining = receiving->round;
    }

    if (receiving->gameEnded == true) {
      break;
    }

    int i = 0;
    while (i < 10000) {
      sleep(1);
      printf("PROCESS %d\n", i);
      i++;
    }
  }
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

  return sockfd;
}
