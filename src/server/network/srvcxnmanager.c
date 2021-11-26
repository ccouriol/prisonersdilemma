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
 */
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

int create_server_socket() {
  int sockfd = -1;
  struct sockaddr_in address;
  int port = 7799;

  /* create socket */
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sockfd <= 0) {
    fprintf(stderr, "%s: error: cannot create socket\n");
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

  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if ((connections[i] != NULL) &&
        (connections[i]->client.idClient != clientID) &&
        (connections[i]->client.isInGame == true)) {
      ret = (&(connections[i]->client));
      return ret; // return the index of an available client
    }
  }
  return ret;
  // otherwise return NULL because there is no other clients
}

void *threadServeur(void *ptr) {
  char buffer_in[BUFFERSIZE];
  char buffer_out[BUFFERSIZE];

  int lenMsgIn;
  clientStructure *clientAddr = NULL;
  connection_t *connection;
  if (!ptr)
    pthread_exit(0);
  connection = (connection_t *)ptr;
  printf("New incoming connection \n");

  add(connection);
  connection->client.idClient = connection->index;

  // message de début de connection, peut etre envoyer num client ?
  printf("Welcome #%i\n", connection->index);
  sprintf(buffer_out, "Welcome #%i\n", connection->index);
  write(connection->sockfd, buffer_out, sizeof(buffer_out));

  // Verification of the number of clients available, minus this client's ID
  while (clientAddr == NULL && !(connection->client.isInGame)) {
    clientAddr = verifyNbClients(connection->client.idClient);
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
  pthread_exit(0);
}

int initGame(clientStructure *client1, clientStructure *client2) {

  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (games[i] == NULL) {
      games[i]->client1 = client1;
      games[i]->client2 = client2;
    }
  }
}

void calculgains() {
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
