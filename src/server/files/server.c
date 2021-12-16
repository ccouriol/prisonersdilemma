#include "../../../include/server/header_server/server.h"

connection_t *connections[MAXSIMULTANEOUSCLIENTS];
gameStructure *games[MAXSIMULTANEOUSCLIENTS];
clientStructure *tabClients[MAXSIMULTANEOUSCLIENTS];
int NBROUNDS;

// TODO: système de rounds sur le serveur, dans la structure game
// faire une lecture depuis fichier de config, et mettre dans variable globale
// puis mettre variable globale dans variable locale de la struct

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
  exit(60);
}

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

void initNBRounds() { NBROUNDS = atoi(read_config("rounds")); }

int initBaseMoney() { return atoi(read_config("basemoney")); }

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
  perror("Too much simultaneous clients\n");
  exit(15);
}

void removeClient(int IDClient) {

  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (i == IDClient) {
      tabClients[i] = NULL;
      return;
    }
  }
  perror("Client not in pool\n");
  exit(50);
}

void disconnectAllClients(gameStructure *game) {
  if (game == NULL) {
    return;
  }
  removeClient(game->iDClient1);
  removeClient(game->iDClient2);
}

/**
 * @brief Search for an available client
 *
 * @return int the position of an available client
 */
int verifyNbClients(int clientID) {

  int ret = -1;

  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if ((tabClients[i] != NULL)) {
      if ((tabClients[i]->idClient != clientID) &&
          (tabClients[i]->isInGame == false)) {
        ret = i;
      }
      return ret; // return the index of an available client
    }
  }
  printf("Client pas dans table\n");
  return ret;
}

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
    perror("No game initialized\n");
    exit(63);
  }
}

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

void closeAll(connection_t *connection, gameStructure *gameInfo,
              dataSentReceived *dataRecieved, dataSentReceived *dataToSend,
              clientStructure *client) {

  disconnectAllClients(gameInfo);
  removeGame(gameInfo);
  free(gameInfo);
  gameInfo = NULL;
  closeLocal(connection, dataRecieved, dataToSend, client);
}

void closeLocal(connection_t *connection, dataSentReceived *dataRecieved,
                dataSentReceived *dataToSend, clientStructure *client) {
  close(connection->sockfd);
  del(connection);
  free(connection);
  free(dataRecieved);
  free(dataToSend);
  free(client);
  pthread_exit(0);
}

void initDataToSend(dataSentReceived *dataToSend, clientStructure *client) {
  if (!dataToSend)
    pthread_exit(0);

  dataToSend->currentBet = 0;
  dataToSend->cooperate = false; // 1 collaborating     0 betraying
  dataToSend->totalMoney = client->money;
  dataToSend->gameEnded = false;
  dataToSend->nbRounds = NBROUNDS;
  dataToSend->gameStarted = true;
}

// TODO: upgrade for a better filename
void saveOnfile(gameStructure *gameInfo) {

  FILE *fich;
  int nbTotalTreason = gameInfo->c1NbTreason + gameInfo->c2NbTreason;
  int nbTotalCollab = gameInfo->c1NbCollab + gameInfo->c2NbCollab;

  fich = fopen("save_games", "a");
  if (fich == NULL) {
    perror("error opening file \n");
    exit(EXIT_FAILURE);
  }

  fseek(fich, 0, SEEK_END);

  fprintf(fich, "Game number: %d", gameInfo->idGame);
  fprintf(fich, "Number of collaboration for player 1: %d",
          gameInfo->c1NbCollab);
  fprintf(fich, "Number of treason for player 1: %d", gameInfo->c1NbTreason);
  fprintf(fich, "Number of collaboration for player 2: %d",
          gameInfo->c2NbCollab);
  fprintf(fich, "Number of treason for player 2: %d", gameInfo->c2NbTreason);

  fprintf(fich, "Total number of treason: %d", nbTotalTreason);
  fprintf(fich, "Total number of collaborations: %d", nbTotalCollab);

  fputs("\0", fich);
  fclose(fich);
}

void fill(clientStructure *client, dataSentReceived *dataRecieved) {

  // Reception and filling of the client structure
  client->money = dataRecieved->totalMoney;
  client->cooperate = dataRecieved->cooperate;
  client->bet = dataRecieved->currentBet;
  client->isFilled = true;
}

void profitsCalculation(clientStructure *client, gameStructure *gameInfo) {

  clientStructure *client1 = tabClients[client->idClient];
  clientStructure *client2 = NULL;

  if (client->idClient != gameInfo->iDClient2) {
    // client->idClient != gameInfo->iDClient1)
    client2 = tabClients[gameInfo->iDClient2];
  } else
    perror("Client non trouvé pour le calcul");

  if (!client1) {
    exit(50);
  }
  if (!client2) {
    exit(51);
  }

  // we wait for the clients to fill their data
  while (!(client1->isFilled) && !(client2->isFilled)) {
    sleep(1);
  }

  // On retire les sommes pariées des pactoles
  client1->money -= client1->bet;
  client2->money -= client1->bet;
  client1->money -= client1->bet;
  client2->money -= client2->bet;

  // If they both betray they gain half their bet
  if (client1->cooperate == 0 && client2->cooperate == client1->cooperate) {
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

bool computeAndSend(clientStructure *client, dataSentReceived *dataRecieved,
                    gameStructure *gameInfo, dataSentReceived *dataToSend) {
  bool hasGameEnded = false;

  fill(client, dataRecieved);
  // Only one of the clients should use this
  if ((gameInfo->iDClient1) > (gameInfo->iDClient2)) {
    if ((tabClients[gameInfo->iDClient1]->isFilled) &&
        (tabClients[gameInfo->iDClient2]->isFilled)) {
      profitsCalculation(client, gameInfo);
      gameInfo->nbrounds = gameInfo->nbrounds - 1;
      if ((gameInfo->nbrounds) == 0) {
        hasGameEnded = true;
      }
    }
  }
  dataToSend->totalMoney = client->money;
  dataToSend->cooperate = client->cooperate;
  dataToSend->currentBet = client->bet;
  client->isFilled = false;

  return hasGameEnded;
}

void *threadServeur(void *ptr) {

  // creation of the buffer to receive and send data
  dataSentReceived *dataRecieved = malloc(sizeof(dataSentReceived));
  dataSentReceived *dataToSend = malloc(sizeof(dataSentReceived));
  clientStructure *client = malloc(sizeof(clientStructure));

  bool hasGameEnded = false;
  int otherClientID = -1;
  size_t len, sizebufferData = sizeof(dataSentReceived);
  connection_t *connection = NULL;
  gameStructure *gameInfo = NULL;

  initNBRounds();

  if (!ptr)
    pthread_exit(0);
  connection = (connection_t *)ptr;
  printf("New incoming connection \n");

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
    perror("Error: No game initialized\n");
    exit(69);
  }

  // printf("GINFO:%p, ID:%d, J1:%d, J2:%d\n", gameInfo, gameInfo->idGame,
  //        gameInfo->iDClient1, gameInfo->iDClient2);
  // printf("JE suis:%d\n", client->idClient);

  initDataToSend(dataToSend, client);
  write(connection->sockfd, dataToSend, sizebufferData);

#if DEBUG
  printf("DEBUG-----------------------------------------------------------\n");
  printf("Data sent:\n");
  printf("CurrentBet: %lu \n", dataToSend->currentBet);
  printf("Choice: %d \n", dataToSend->cooperate);
  printf("TotalMoney: %lu \n", dataToSend->totalMoney);
  printf("IDGame: %d \n", dataToSend->iDGame);
  printf("Game Ended ? %d\n", dataToSend->gameEnded);
  printf("----------------------------------------------------------------\n");
#endif

  while (!hasGameEnded) {

    if ((len = read(connection->sockfd, dataRecieved, sizebufferData)) > 0) {
      hasGameEnded = computeAndSend(client, dataRecieved, gameInfo, dataToSend);
      write(connection->sockfd, dataToSend, sizebufferData);
    }
    if (len == 0) {
      printf("Client disconnected\n");
      if (client->canFree) {
        closeAll(connection, gameInfo, dataRecieved, dataToSend, client);
      } else
        closeLocal(connection, dataRecieved, dataToSend, client);
    }
    sleep(1);
  }
#if DEBUG
  printf("DEBUG-----------------------------------------------------------\n");
  printf("Data Received:\n");
  printf("CurrentBet: %lu \n", dataRecieved->currentBet);
  printf("Choice: %d \n", dataRecieved->cooperate);
  printf("TotalMoney: %lu \n", dataRecieved->totalMoney);
  printf("IDGame: %d \n", dataRecieved->iDGame);
  printf("Game Ended ? %d\n", dataRecieved->gameEnded);
  printf("----------------------------------------------------------------\n");
  printf("DEBUG-----------------------------------------------------------\n");
  printf("Data sent:\n");
  printf("CurrentBet: %lu \n", dataToSend->currentBet);
  printf("Choice: %d \n", dataToSend->cooperate);
  printf("TotalMoney: %lu \n", dataToSend->totalMoney);
  printf("IDGame: %d \n", dataToSend->iDGame);
  printf("Game Ended ? %d\n", dataToSend->gameEnded);
  printf("----------------------------------------------------------------\n");
#endif

  // saving on file, but only if the client's ID is an even number
  if ((client->idClient) > (gameInfo->iDClient2)) {
    saveOnfile(gameInfo);
    closeAll(connection, gameInfo, dataRecieved, dataToSend, client);
  } else
    closeLocal(connection, dataRecieved, dataToSend, client);
}
