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

  int sockfd = *((int *)ptr);
  int len;
  int i = 0;
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

    sleep(1);
    printf("PROCESS %d\n", i);
    i++;
  }

  puts("ThreadProcess ended");
  close(sockfd);
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

/*!
 * \fn int verifyIP(char *string_ip)
 * \author Clément Couriol
 * \version 0.1
 * \date  01/12/2021
 * \brief Returns 0 if the ip is valid, 1 if not
 * \remarks None
 * \param string_ip
 * \return
 */
int verifyIP(char *string_ip) {
  regex_t regex;
  int reti;
  char *rgx = "^(\\b25[0-5]|\\b2[0-4][0-9]|\\b[01]?[0-9][0-9]?)(\\.(25[0-5]|2["
              "0-4][0-9]|[01]?[0-9][0-9]?)){3}$";

  reti = regcomp(&regex, rgx, REG_EXTENDED);
  if (reti)
    return EXIT_FAILURE;

  reti = regexec(&regex, string_ip, 0, NULL, 0);
  if (!reti)
    return EXIT_SUCCESS;
  else if (reti == REG_NOMATCH)
    return EXIT_FAILURE;
  else
    return EXIT_FAILURE;

  regfree(&regex);
}