/*!
 * \file server.c
 * \author Louis Morand
 * \version 0.1
 * \date 20/12/2021
 * \brief Main function of the server
 * \remarks None
 *
 *
 *
 */

/*! Importation of librairies*/
#include "../../../include/server/header_server/server.h"

connection_t *connections[MAXSIMULTANEOUSCLIENTS];
gameStructure *games[MAXSIMULTANEOUSCLIENTS];
clientStructure *tabClients[MAXSIMULTANEOUSCLIENTS];
int NBROUNDS;

// TODO: faire la lecture de port ?
/*!
 * \fn void init_sockets_array()
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief init the array of sockets
 * \remarks None
 */
void init_sockets_array() {
  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    connections[i] = NULL;
  }
}

void init_clients_array() {
  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    tabClients[i] = NULL;
  }
}

/*!
 * \fn void add(connection_t *connection)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief add a connection to the array of sockets
 * \remarks None
 * \param connection
 */
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

/*!
 * \fn void del(connection_t *connection)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief delete a connection from the array of sockets
 * \remarks None
 * \param connection
 */
void del(connection_t *connection) {
  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (connections[i] == connection) {
      connections[i] = NULL;
      return;
    }
  }
  perror("Connection not in pool");
  exit(60);
}

/*!
 * \fn int create_server_socket()
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief create a server socket
 * \remarks None
 * \return
 */
int create_server_socket() {
  int sockfd = -1;
  struct sockaddr_in address;
  // int port = 7799;
  char *addr;
  int port;

  addr = read_config("ip");
  port = atoi(read_config("port"));
  // printf("IP:%s\n", addr);
  // printf("port:%d\n", port);

  if (verifyIP(addr)) {
    perror("There is something wrong with the IP, check it's validity in the "
           "config file");
    exit(EXIT_FAILURE);
  }

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
  // address.sin_addr.s_addr = inet_addr("0.0.0.0");
  // address.sin_port = htons(port);
  address.sin_addr.s_addr = inet_addr(addr);
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

/*!
 * \fn void initNBRounds()
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief init the number of rounds
 * \remarks None
 */
void initNBRounds() { NBROUNDS = atoi(read_config("rounds")); }

/*!
 * \fn int initBaseMoney()
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief init the base money
 * \remarks None
 * \return
 */
int initBaseMoney() { return atoi(read_config("basemoney")); }

/*!
 * \fn void createClient(clientStructure *client)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief add a client to the array of clients
 * \remarks None
 * \param client
 */
void createClient(clientStructure *client) {
  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (tabClients[i] == NULL) {
      client->idClient = i;
      client->isInGame = false;
      client->isFilled = false;
      client->canFree = false;
      client->money = initBaseMoney();
      tabClients[i] = client;
      return;
    }
  }
  perror("Too much simultaneous clients");
  exit(15);
}

/*!
 * \fn void removeClient(int IDClient)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief remove a client from the array of clients
 * \remarks None
 * \param IDClient
 */
void removeClient(int IDClient) {
  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (i == IDClient) {
      tabClients[i] = NULL;
      return;
    }
  }
  perror("Client not in pool");
  exit(50);
}

/*!
 * \fn void disconnectAllClients(gameStructure *game)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief Remove all clients from the game array
 * \remarks Should only be called once by game
 * \param game
 */
void disconnectAllClients(gameStructure *game) {
  if (game == NULL) {
    return;
  }
  removeClient(game->iDClient1);
  removeClient(game->iDClient2);
}

/*!
 * \fn int verifyNbClients(int clientID)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief Search for an available client
 * \remarks None
 * \param clientID
 * \return int the position of an available client
 */
int verifyNbClients(int clientID) {
  int ret = -1;

  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if ((tabClients[i] != NULL)) {
      if ((tabClients[i]->idClient != clientID) &&
          (tabClients[i]->isInGame == false)) {
        ret = i;
      }
    }
  }
  return ret;
}

/*!
 * \fn void initGame(int client1ID, int client2ID)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief init a game structure, if there is at least 2 clients connected
 * \remarks Set the canFree boolean, which determines which client can call
 * specific functions to close games, write files, and call disconnectAll
 * \param client1ID
 * \param client2ID
 */
void initGame(int client1ID, int client2ID) {
  gameStructure *game = NULL;

  game = malloc(sizeof(gameStructure));

  if (game != NULL) {

    game->iDClient1 = client1ID;
    game->iDClient2 = client2ID;
    game->c1NbCollab = 0;
    game->c1NbTreason = 0;
    game->c2NbCollab = 0;
    game->c2NbTreason = 0;
    game->nbrounds = NBROUNDS;
    game->isCalcFinished = false;
    game->hasGameEned = false;

    if ((client1ID) > (client2ID)) {
      tabClients[client1ID]->canFree = true;
    } else
      tabClients[client2ID]->canFree = true;

    for (int i = 0; i < MAXSIMULTANEAOUSGAMES; i++) {
      if (games[i] == NULL && (game->iDClient1 >= 0) &&
          (game->iDClient2 >= 0)) {
        game->idGame = i;
        tabClients[client1ID]->isInGame = true;
        tabClients[client2ID]->isInGame = true;
        games[i] = game;
        tabClients[client1ID]->gameP = game;
        tabClients[client2ID]->gameP = game;
        return;
      }
    }
  } else {
    perror("No game initialized");
    exit(63);
  }
}

/*!
 * \fn void removeGame(gameStructure *gameInfo)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief remove a game from the array of games
 * \remarks None
 * \param gameInfo
 */
void removeGame(gameStructure *gameInfo) {
  if (gameInfo == NULL) {
    perror("Game not in pool");
    exit(80);
  }
  for (int i = 0; i < MAXSIMULTANEAOUSGAMES; i++) {
    if (games[i] == gameInfo) {
      games[i] = NULL;
      return;
    }
  }
  return;
}

/*!
 * \fn void closeAll(connection_t *connection, gameStructure *gameInfo,
 * dataSentReceived *dataRecieved, dataSentReceived *dataToSend, clientStructure
 * *client)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief close all the connections and free the memory
 * \remarks Should be used by one of the two clients only
 * \param connection
 * \param gameInfo
 * \param dataRecieved
 * \param dataToSend
 * \param client
 */
void closeAll(connection_t *connection, gameStructure *gameInfo,
              dataSentReceived *dataRecieved, dataSentReceived *dataToSend,
              clientStructure *client) {

  disconnectAllClients(gameInfo);
  removeGame(gameInfo);
  free(gameInfo);
  gameInfo = NULL;
  closeLocal(connection, dataRecieved, dataToSend, client);
}

/*!
 * \fn void closeLocal(connection_t *connection, dataSentReceived *dataRecieved,
 * dataSentReceived *dataToSend, clientStructure *client)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief close the local connection
 * \remarks Should be used by the client not calling closeAll
 * \param connection
 * \param dataRecieved
 * \param dataToSend
 * \param client
 */
void closeLocal(connection_t *connection, dataSentReceived *dataRecieved,
                dataSentReceived *dataToSend, clientStructure *client) {
  close(connection->sockfd);
  del(connection);
  free(connection);
  free(dataRecieved);
  free(dataToSend);
  free(client);
  puts("Fin du thread");
  pthread_exit(0);
}

/*!
 * \fn void initDataToSend(dataSentReceived *dataToSend)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief init the data structure to send to the client, and tell them the game
 * has started
 * \remarks None
 * \param dataToSend
 */
void initDataToSend(dataSentReceived *dataToSend) {
  if (!dataToSend)
    pthread_exit(0);

  dataToSend->currentBet = 0;
  dataToSend->cooperate = false; // 1 collaborating     0 betraying
  dataToSend->gameEnded = false;
  dataToSend->gameStarted = true;
}

// TODO: data written are incorrect values shoudn't be 0
/*!
 * \fn void saveOnfile(gameStructure *gameInfo)
 * \author Louis Morand
 * \version 0.1
 * \date  20/12/2021
 * \brief save the game data on a file
 * \remarks The file name is in the format "day-month-year-time"
 * \param gameInfo
 */
void saveOnfile(gameStructure *gameInfo) {
  puts("GEN FICHIER");

  if (!gameInfo) {
    puts("Erreur recption gameInfo");
  }

  FILE *fich;
  int nbTotalTreason = gameInfo->c1NbTreason + gameInfo->c2NbTreason;
  int nbTotalCollab = gameInfo->c1NbCollab + gameInfo->c2NbCollab;

  time_t t = time(NULL);
  struct tm tm = *localtime(&t);
  char date[25];
  strftime(date, sizeof(date), "%d-%m-%Y-%T.log", &tm);

  fich = fopen(date, "w+");
  if (fich == NULL) {
    perror("error opening file");
    exit(EXIT_FAILURE);
  }

  // fseek(fich, 0, SEEK_END);

  fprintf(fich, "Game number: %d\n", (gameInfo->idGame) + 1);
  fprintf(fich, "Number of collaboration for player 1: %d\n",
          gameInfo->c1NbCollab);
  fprintf(fich, "Number of treason for player 1: %d\n", gameInfo->c1NbTreason);
  fprintf(fich, "Number of collaboration for player 2: %d\n",
          gameInfo->c2NbCollab);
  fprintf(fich, "Number of treason for player 2: %d\n", gameInfo->c2NbTreason);

  fprintf(fich, "Total number of treason: %d\n", nbTotalTreason);
  fprintf(fich, "Total number of collaborations: %d\n", nbTotalCollab);

  fputs("\0", fich);
  fclose(fich);
  puts("File generation finished");
}

/*!
 * \fn void fill(clientStructure *client, dataSentReceived *dataRecieved)
 * \author Louis Morand
 * \version 0.8
 * \date  20/12/2021
 * \brief fill the dataRecieved structure with the data received from the client
 * \remarks None
 * \param client
 * \param dataRecieved
 */
void fill(clientStructure *client, dataSentReceived *dataRecieved) {
  client->cooperate = dataRecieved->cooperate;
  client->bet = dataRecieved->currentBet;
  client->isFilled = true;
}

/*!
 * \fn void profitsCalculation(clientStructure *client, gameStructure *gameInfo)
 * \author Louis Morand
 * \version 0.2
 * \date  20/12/2021
 * \brief calculate the profits of the game
 * \remarks None
 * \param client
 * \param gameInfo
 */
void profitsCalculation(clientStructure *client, gameStructure *gameInfo) {
  clientStructure *client1 = tabClients[client->idClient];
  clientStructure *client2 = NULL;

  if (!client1) {
    exit(50);
  }

  if (gameInfo->iDClient1 != gameInfo->iDClient2) {
    // client->idClient != gameInfo->iDClient1)
    client2 = tabClients[gameInfo->iDClient2];
  } else
    perror("Client not found for calculation");

  if (!client2) {
    exit(51);
  }

  // we wait for the clients to fill their data
  while (!(client1->isFilled) && !(client2->isFilled)) {
    sleep(1);
  }

  // On retire les sommes pari??es des pactoles
  client1->money -= client1->bet;
  client2->money -= client2->bet;

  // If they both betray they gain half their bet
  if (client1->cooperate == 0 && client2->cooperate == 1) {
    client1->money += client1->bet / 2;
    client2->money += client1->bet / 2;

    gameInfo->c1NbTreason += 1;
    gameInfo->c2NbTreason += 1;
  }
  // If C1 betray C2, C1 gain C2's bet
  else if (client1->cooperate == 0 && client2->cooperate == 1) {
    client1->money += client2->bet;

    gameInfo->c1NbTreason += 1;
    gameInfo->c2NbCollab += 1;
  }
  // If C2 betray C1, C2 gain C1's bet
  else if (client1->cooperate == 1 && client2->cooperate == 0) {
    client2->money += client1->bet;

    gameInfo->c1NbCollab += 1;
    gameInfo->c2NbTreason += 1;
  }
  // If they both collaborate
  else {
    client1->money += client1->bet;
    client2->money += client2->bet;

    gameInfo->c1NbCollab += 1;
    gameInfo->c2NbCollab += 1;
  }
}

/*!
 * \fn bool computeAndSend(clientStructure *client, dataSentReceived
 * *dataRecieved, gameStructure *gameInfo, dataSentReceived *dataToSend)
 * \author Louis Morand, Mathis Francfort
 * \version 0.6
 * \date  20/12/2021
 * \brief compute the data to send to the client
 * \remarks None
 * \param client
 * \param dataRecieved
 * \param gameInfo
 * \param dataToSend
 * \return true is the game is finished
 */
bool computeAndSend(clientStructure *client, dataSentReceived *dataRecieved,
                    gameStructure *gameInfo, dataSentReceived *dataToSend) {

  fill(client, dataRecieved);
  // Only one of the clients should use this
  if ((gameInfo->iDClient1) > (gameInfo->iDClient2)) {

    if ((tabClients[gameInfo->iDClient1]->isFilled) &&
        (tabClients[gameInfo->iDClient2]->isFilled)) {
      profitsCalculation(client, gameInfo);
      gameInfo->nbrounds = gameInfo->nbrounds - 1;
      if ((gameInfo->nbrounds) == 0) {
        gameInfo->hasGameEned = true;
      }
      gameInfo->isCalcFinished = true;
    }
  }

  sleep(1);

  dataToSend->cooperate = client->cooperate;
  dataToSend->currentBet = client->bet;
  if (gameInfo->hasGameEned) {
    dataToSend->gameEnded = true;
  }
  client->isFilled = false;
  gameInfo->isCalcFinished = false;

  return (gameInfo->hasGameEned);
}

/*!
 * \fn void *threadServeur(void *ptr)
 * \author Louis Morand
 * \version 0.4
 * \date  20/12/2021
 * \brief thread that will be used to serve the clients. Can react to a client's
 * connection, end games, and compute the data sent.
 * \remarks None
 * \param ptr
 */
void *threadServeur(void *ptr) {

  // creation of the buffer to receive and send data
  dataSentReceived *dataRecieved = malloc(sizeof(dataSentReceived));
  dataSentReceived *dataToSend = malloc(sizeof(dataSentReceived));
  clientStructure *client = malloc(sizeof(clientStructure));

  bool hasGameEnded = false;
  int otherClientID = -1;
  ssize_t len;
  size_t sizebufferData = sizeof(dataSentReceived);
  connection_t *connection = NULL;
  gameStructure *gameInfo = NULL;

  initNBRounds();

  if (!ptr)
    pthread_exit(0);
  connection = (connection_t *)ptr;
  puts("New incoming connection");

  if (!client)
    pthread_exit(0);

  if (!dataRecieved)
    pthread_exit(0);

  if (!dataToSend)
    pthread_exit(0);

  add(connection);
  createClient(client);
  printf("Welcome #%i\n", client->idClient);

  // Verification of the number of clients available, minus this client's ID
  // and creation of the game if there is enough clients
  while (!(client->isInGame)) {
    otherClientID = verifyNbClients(client->idClient);

    if (otherClientID != -1) {
      if ((client->idClient) > (otherClientID)) {
        initGame((client->idClient), otherClientID);
        break;
      }
    }
  }
  gameInfo = client->gameP;

  if (gameInfo == NULL) {
    perror("Error: No game initialized");
    exit(69);
  }

  initDataToSend(dataToSend);
  write(connection->sockfd, dataToSend, sizebufferData);

#if DEBUG
  printf("DEBUG-----------------------------------------------------------\n");
  printf("Data sent:\n");
  printf("CurrentBet: %lu \n", dataToSend->currentBet);
  printf("Choice: %d \n", dataToSend->cooperate);
  printf("Game Ended ? %d\n", dataToSend->gameEnded);
  printf("----------------------------------------------------------------\n");
#endif

  do {
    len = read(connection->sockfd, dataRecieved, sizebufferData);
#if DEBUF
    printf(
        "DEBUG-----------------------------------------------------------\n");
    printf("Data Received:\n");
    printf("CurrentBet: %lu \n", dataRecieved->currentBet);
    printf("Choice: %d \n", dataRecieved->cooperate);
    printf(
        "----------------------------------------------------------------\n");
#endif

    if (len > 0) {
      hasGameEnded = computeAndSend(client, dataRecieved, gameInfo, dataToSend);

      write(connection->sockfd, dataToSend, sizebufferData);

#if DEBUG
      printf(
          "DEBUG-----------------------------------------------------------\n");
      printf("Data sent:\n");
      printf("CurrentBet: %lu \n", dataToSend->currentBet);
      printf("Choice: %d \n", dataToSend->cooperate);
      printf("Game Ended ? %d\n", dataToSend->gameEnded);
      printf(
          "----------------------------------------------------------------\n");
#endif
    }
    if (len == 0) {
      perror("Client disconnected");
      if (client->canFree) {
        if (hasGameEnded)
          saveOnfile(gameInfo);
        closeAll(connection, gameInfo, dataRecieved, dataToSend, client);
      } else
        closeLocal(connection, dataRecieved, dataToSend, client);
    }

  } while (!hasGameEnded);

  if (hasGameEnded) {
    if (client->canFree) {
      saveOnfile(gameInfo);
      closeAll(connection, gameInfo, dataRecieved, dataToSend, client);
    } else
      closeLocal(connection, dataRecieved, dataToSend, client);
  }
  pthread_exit(0);
}
