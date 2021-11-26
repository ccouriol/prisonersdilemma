#include "srvcxnmanager.h"

connection_t *connections[MAXSIMULTANEOUSCLIENTS];
gameStructure *games[MAXSIMULTANEOUSCLIENTS];

void init_sockets_array() {
  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    connections[i] = NULL;
  }
}

void add(connection_t *connection) {
  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (connections[i] == NULL) {
      connections[i] = connection;
      return;
    }
  }
  perror("Too much simultaneous connections");
  exit(-5);
}

void del(connection_t *connection) {
  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (connections[i] == connection) {
      connections[i] = NULL;
      return;
    }
  }
  perror("Connection not in pool ");
  exit(-5);
}
/*
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_lock(&lock);
pthread_mutex_unlock(&lock);
 */

/**
 * Thread allowing server to handle multiple client connections
 * @param ptr connection_t
 * @return
 *//*
void *threadProcess(void *ptr) {
  char buffer_in[BUFFERSIZE];
  char buffer_out[BUFFERSIZE];

  int len;
  connection_t *connection;

  if (!ptr)
    pthread_exit(0);
  connection = (connection_t *)ptr;
  printf("New incoming connection \n");

  add(connection);

  // Welcome the new client
  printf("Welcome #%i\n", connection->index);
  sprintf(buffer_out, "Welcome #%i\n", connection->index);
  write(connection->sockfd, buffer_out, strlen(buffer_out));

  while ((len = read(connection->sockfd, buffer_in, BUFFERSIZE)) > 0) {

    if (strncmp(buffer_in, "bye", 3) == 0) {
      break;
    }
#if DEBUG
    printf(
        "DEBUG-----------------------------------------------------------\n");
    printf("len : %i\n", len);
    printf("Buffer : %.*s\n", len, buffer_in);
    printf(
        "----------------------------------------------------------------\n");
#endif
    strcpy(buffer_out, "\nServer Echo : ");
    strncat(buffer_out, buffer_in, len);

    if (buffer_in[0] == '@') {
      for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
        if (connections[i] != NULL) {
          write(connections[i]->sockfd, buffer_out, strlen(buffer_out));
        }
      }
    } else if (buffer_in[0] == '#') {
      int client = 0;
      int read = sscanf(buffer_in, "%*[^0123456789]%d ", &client);
      for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
        if (client == connections[i]->index) {
          write(connections[i]->sockfd, buffer_out, strlen(buffer_out));
          break;
        } // no client found ? : we dont care !!
      }
    } else {
      write(connection->sockfd, buffer_out, strlen(buffer_out));
    }

    // clear input buffer
    memset(buffer_in, '\0', BUFFERSIZE);
  }
  printf("Connection to client %i ended \n", connection->index);
  close(connection->sockfd);
  del(connection);
  free(connection);
  pthread_exit(0);
}
*/
int create_server_socket() {
  int sockfd = -1;
  struct sockaddr_in address;
  int port = 7799;

  /* create socket */
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd <= 0) {
    fprintf(stderr, "%s", ": error: cannot create socket\n");
    return -3;
  }

  /* bind socket to port */
  address.sin_family = AF_INET;
  // bind to all ip :
  // address.sin_addr.s_addr = INADDR_ANY;
  // ou 0.0.0.0
  // Sinon  127.0.0.1
  address.sin_addr.s_addr = inet_addr("0.0.0.0");
  address.sin_port = htons(port);

  /* prevent the 60 secs timeout */
  int reuse = 1;
  setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse,
             sizeof(reuse));

  /* bind */
  if (bind(sockfd, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) <
      0) {
    fprintf(stderr, "error: cannot bind socket to port %d\n", port);
    return -4;
  }

  return sockfd;
}

/**
 * @brief Search for an available client
 *
 * @return int the position of an available client
 */
clientStructure *verifyNbClients(int clientID) {

  clientStructure *ret = NULL;

  ret = malloc(sizeof(clientStructure));

  if (ret == NULL) {
    perror("Error initialazing client pointer");
    exit(-3);
  }

  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if ((connections[i] != NULL) &&
        (connections[i]->client.idClient != clientID) &&
        (connections[i]->client.isInGame == false)) {
      ret = (&(connections[i]->client));
      return ret; // return the index of an available client
    }
  }
  return ret;
  // otherwise return NULL because there is no other clients
}

gameStructure *initGame(clientStructure *client1, clientStructure *client2) {

  gameStructure *game = NULL;
  game = malloc(sizeof(gameStructure));
  if (game == NULL) {
    perror("Error initialazing game pointer");
    exit(-3);
  }

  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (games[i] == NULL && client1 != NULL && client2 != NULL) {
      games[i]->client1 = client1;
      games[i]->client2 = client2;
      games[i]->idPartie = i;
      game = games[i];
      client1->isInGame = true;
      client2->isInGame = true;
      return game;
    }
  }
  perror("No game initialized");
  exit(-7);
}

void *threadServeur(void *ptr) {
  char buffer_in[BUFFERSIZE];
  char buffer_out[BUFFERSIZE];
  testStruct *buffer_out2 = malloc(sizeof(testStruct));

  int lenMsgIn;
  clientStructure *clientAddr;
  gameStructure *iDGame;
  connection_t *connection;

  testStruct *test = malloc(sizeof(testStruct));
  test->index = 1;
  sprintf(test->msg, "Ceci est un test");

  if (!ptr)
    pthread_exit(0);
  connection = (connection_t *)ptr;
  printf("New incoming connection \n");

  add(connection);
  connection->client.idClient = connection->index;
  connection->client.isInGame = false;

  printf("Welcome #%i\n", connection->client.idClient);
  // sprintf(buffer_out, "Welcome #%i\n", connection->client.idClient);
  memcpy(buffer_out2, test, sizeof(testStruct));
  // sprintf(buffer_out2, (const char *)&test, sizeof(testStruct));
  write(connection->sockfd, buffer_out, sizeof(buffer_out));

  // Verification of the number of clients available, minus this client's ID
  // and creation of the game if there is enough clients
  while (!(connection->client.isInGame)) {
    clientAddr = verifyNbClients(connection->client.idClient);
    if (clientAddr != NULL) {
      iDGame = initGame(clientAddr, &(connection->client));
    }
  }

  // tant que le client ne ferme pas la connexion
  while ((lenMsgIn = read(connection->sockfd, buffer_in, BUFFERSIZE)) > 0) {

    if (strncmp(buffer_in, "bye", 3) == 0) {
      break;
    }
    // réception données

    // puis traitement et renvoi
  }

  // ecriture sur fichier avant fermeture
  close(connection->sockfd);
  del(connection);
  free(connection);
  free(iDGame);
  free(clientAddr);
  pthread_exit(0);
}

void calculgains(gameStructure *iDGame) {
  // On retire les sommes pariées des pactoles
  iDGame->client1->pactole -= iDGame->client1->sommePariée;
  iDGame->client2->pactole -= iDGame->client2->sommePariée;
  // Si les deux joueurs trahissent...
  if (iDGame->client1->choix == 1 && iDGame->client2->choix == 1) {
    iDGame->client1->pactole += iDGame->client1->sommePariée / 2;
    iDGame->client2->pactole += iDGame->client2->sommePariée / 2;
    iDGame->c1NbTrahison += 1;
    iDGame->c2NbTrahison += 1;
  }
  // Si le joueur 1 trahit le joueur 2...
  else if (iDGame->client1->choix == 1 && iDGame->client2->choix == 0) {
    iDGame->client1->pactole += iDGame->client2->sommePariée;
    iDGame->c1NbTrahison += 1;
    iDGame->c2NbCollab += 1;
  }
  // Si le joueur 2 trahit le joueur 1...
  else if (iDGame->client1->choix == 0 && iDGame->client2->choix == 1) {
    iDGame->client2->pactole += iDGame->client1->sommePariée;
    iDGame->c1NbCollab += 1;
    iDGame->c2NbTrahison += 1;
  }
  // Si les deux joueurs collaborent...
  else {
    iDGame->client1->pactole += iDGame->client1->sommePariée;
    iDGame->client2->pactole += iDGame->client2->sommePariée;
    iDGame->c1NbCollab += 1;
    iDGame->c2NbCollab += 1;
  }
  // calcule les gains et les renvois
  // N'est pas chargée de les envoyer sur le socket
  // prends en entrée une structure
  // P-E choper les choix dans la sous structure, et les placer dans la
  // structure mere ? pour ensuite renvoyer les choix au bon client cf numero de
  // partie pour vérifier les clients
}

void ecritureResultats(/*struct machin*/) {
  // ici, mutex+ écriture fichier, mutex car potentiellement concurrent.
  // appelée à la fin de la partie avant la fermeture du thread.
}
