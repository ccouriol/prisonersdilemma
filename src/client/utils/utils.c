/*!
 * \file utils.c
 * \author GABETTE Cédric
 * \version 0.1
 * \date 26/11/2021
 * \brief Configuration of thread
 * \remarks None
 *
 *
 *
 */

/*! Importation of librairies*/
#include "../../../include/client/game/utils.h"
#include "../../../include/client/game/main.h"

bool isGameStarted;
bool isGameFinished;

/*!
 * \fn void *threadProcess(void *ptr)
 * \author GABETTE Cédric, louis MORAND
 * \version 0.1
 * \date  26/11/2021
 * \brief Thread for connection
 * \remarks None
 * \param ptr
 */
void *threadProcess(void *ptr) {
  int len;
  int sockfd = *((int *)ptr);
  s_clientData clientData;
  dataSentReceived *receiving;
  receiving = malloc(sizeof(dataSentReceived));

  do {

    if ((len = read(sockfd, receiving, sizeof(dataSentReceived))) > 0) {
      isGameStarted = receiving->gameStarted;
      isGameFinished = receiving->gameEnded;
    }

    if (len <= 0) {
      printf("Flux broken\n");
      closeAll();
      pthread_exit(0);
    }
#if DEBUG
    printf("RECEIVING----------------------------------------\n");
    printf("Status game : %d\n", isGameStarted);
    printf("FINISH ? %d\n", isGameFinished);
    printf("END RECEIVING------------------------------------\n");
#endif

    if (receiving->gameEnded) {
      puts("Close");
      pthread_exit(0);
    }
  } while (!receiving->gameEnded);
}

/*!
 * \fn int open_connection()
 * \author GABETTE Cédric
 * \version 0.1
 * \date  26/11/2021
 * \brief Establish connection with server through a socket
 * \remarks None
 * \return
 */
int open_connection() {
  int sockfd;

  struct sockaddr_in serverAddr;
  uint16_t port = 7799;

  char *addr = read_config("ip");
  printf("IP:%s\n", addr);
  int port2 = atoi(read_config("port"));
  printf("port:%d\n", port2);

  // Create the socket.
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  // Configure settings of the server address
  // Address family is Internet
  serverAddr.sin_family = AF_INET;
  // Set port number, using htons function
  // serverAddr.sin_port = htons(port);
  serverAddr.sin_port = htons(port2);
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
