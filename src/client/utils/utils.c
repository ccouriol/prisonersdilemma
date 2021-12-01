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
  char buffer_in[BUFFERSIZE];
  int sockfd = *((int *)ptr);
  long int len;
  while ((len = read(sockfd, buffer_in, BUFFERSIZE)) != 0) {
    if (strncmp(buffer_in, "exit", 4) == 0) {
      break;
    }

    printf("receive %ld chars\n", len);
    printf("%.*s\n", (int)len, buffer_in);
  }
  close(sockfd);
  printf("client pthread ended, len=%ld\n", len);
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