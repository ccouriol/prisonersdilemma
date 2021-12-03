#include "../../../include/server/network/srvcxnmanager.h"

connection_t *connections[MAXSIMULTANEOUSCLIENTS];
gameStructure *games[MAXSIMULTANEOUSCLIENTS];
clientStructure *clients[MAXSIMULTANEOUSCLIENTS];

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

void addclient(clientStructure *client) {

  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (clients[i] == NULL) {
      client->idClient = i;
      client->isInGame = false;
      clients[i] = client;
      return;
    }
  }
  perror("Too much simultaneous clients");
  exit(-5);
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
    if ((clients[i] != NULL) && (clients[i]->idClient != clientID) &&
        (clients[i]->isInGame == false)) {
      ret = (clients[i]);
      return ret; // return the index of an available client
    }
  }
  return ret = NULL;
}

gameStructure *initGame(clientStructure *client1, clientStructure *client2) {

  gameStructure *game = NULL;
  game = malloc(sizeof(gameStructure));
  if (game == NULL) {
    perror("Error initialazing game pointer");
    exit(-3);
  }

  game->client1 = client1;
  game->client2 = client2;
  game->c1NbCollab = 0;
  game->c1NbTreason = 0;
  game->c2NbCollab = 0;
  game->c2NbTreason = 0;

  for (int i = 0; i < MAXSIMULTANEOUSCLIENTS; i++) {
    if (games[i] == NULL && client1 != NULL && client2 != NULL) {
      game->idGame = i;
      client1->isInGame = true;
      client2->isInGame = true;
      games[i] = game;
      return game;
    }
  }
  perror("No game initialized");
  exit(-7);
}

void profitsCalculation(gameStructure *gameInfo) {

  clientStructure *client1 = gameInfo->client1;
  clientStructure *client2 = gameInfo->client2;

  // On retire les sommes pariées des pactoles
  client1->money -= client1->bet;
  client2->money -= client1->bet;
  gameInfo->client1->money -= gameInfo->client1->bet;
  gameInfo->client2->money -= gameInfo->client2->bet;

  // If they both betray they gain half their bet
  if (client1->cooperate == 0 && client2->cooperate == client1->cooperate) {
    client1->money += client1->bet / 2;
    client2->money += client1->bet / 2;
    gameInfo->client1->money += gameInfo->client1->bet / 2;
    gameInfo->client2->money += gameInfo->client2->bet / 2;

    gameInfo->c1NbTreason += 1;
    gameInfo->c2NbTreason += 1;
  }
  // If C1 betray C2, C1 gain C2's bet
  else if (client1->cooperate == 0 && client2->cooperate == 1) {
    client1->money += client2->bet;
    gameInfo->client1->money += gameInfo->client2->bet;

    gameInfo->c1NbTreason += 1;
    gameInfo->c2NbCollab += 1;
  }
  // If C2 betray C12, C2 gain C1's bet
  else if (client1->cooperate == 1 && client2->cooperate == 0) {
    client2->money += client1->bet;

    gameInfo->client2->money += gameInfo->client1->bet;

    gameInfo->c1NbCollab += 1;
    gameInfo->c2NbTreason += 1;
  }
  // If they both collaborate
  else {
    client1->money += client1->bet;
    client2->money += client2->bet;

    gameInfo->client1->money += gameInfo->client1->bet;
    gameInfo->client2->money += gameInfo->client2->bet;

    gameInfo->c1NbCollab += 1;
    gameInfo->c2NbCollab += 1;
  }
}

// TODO: upgrade for a better filename
void saveOnfile(gameStructure *gameInfo) {

  FILE *fich;
  int nbTotalTreason = gameInfo->c1NbTreason + gameInfo->c2NbTreason;
  int nbTotalCollab = gameInfo->c1NbCollab + gameInfo->c2NbCollab;

  fich = fopen("NOMFICH", "a");
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

void *threadServeur(void *ptr) {

  // creation of the buffer to receive and send data
  dataSentReceived *dataRecieved = malloc(sizeof(dataSentReceived));
  dataSentReceived *dataToSend = malloc(sizeof(dataSentReceived));

  bool hasGameEnded = false;
  clientStructure *otherClientAddr;
  clientStructure *client = NULL;
  gameStructure *gameInfo = NULL;
  connection_t *connection;

  if (!ptr)
    pthread_exit(0);
  connection = (connection_t *)ptr;
  printf("New incoming connection \n");

  client = malloc(sizeof(clientStructure));
  if (client == NULL) {
    perror("No client initialized");
  }

  add(connection);
  addclient(client);

  printf("Welcome #%i\n", client->idClient);

  // Verification of the number of clients available, minus this client's ID
  // and creation of the game if there is enough clients
  while (!(client->isInGame)) {
    otherClientAddr = verifyNbClients(client->idClient);
    if (otherClientAddr != NULL) {
      gameInfo = initGame(otherClientAddr, client);
    }
  }

  if (gameInfo == NULL) {
    perror("No game initialized");
  } else { // sendind the game ID to tell the client the game has started
    dataToSend->currentBet = 0;
    dataToSend->moneyGainLost = 0;
    dataToSend->cooperate = NULL; // 1 collaborer     0 trahir
    dataToSend->totalMoney = 0;
    dataToSend->iDGame = gameInfo->idGame;
    dataToSend->gameEnded = NULL;
    write(connection->sockfd, dataToSend, sizeof(dataSentReceived));
  }
#if DEBUG
  bool cooperate; // 1 collaborer     0 trahir
  unsigned long totalMoney;
  int iDGame;
  bool gameEnded;
  printf("DEBUG-----------------------------------------------------------\n");
  printf("Data sent:\n");
  printf("CurrentBet: %d \n", dataToSend->currentBet);
  printf("Money Lost: %d \n", dataToSend->moneyGainLost);
  printf("Choice: %d \n", dataToSend->cooperate);
  printf("TotalMoney: %d \n", dataToSend->totalMoney);
  printf("IDGame: %d \n", dataToSend->iDGame);
  printf("Game Ended ? %d", dataToSend->gameEnded);
  printf("----------------------------------------------------------------\n");
#endif

  while (!hasGameEnded) {
    read(connection->sockfd, dataRecieved, (sizeof(dataSentReceived)));
#if DEBUG
    bool cooperate; // 1 collaborer     0 trahir
    unsigned long totalMoney;
    int iDGame;
    bool gameEnded;
    printf(
        "DEBUG-----------------------------------------------------------\n");
    printf("Data Received:\n");
    printf("CurrentBet: %d \n", dataToSend->currentBet);
    printf("Money Lost: %d \n", dataToSend->moneyGainLost);
    printf("Choice: %d \n", dataToSend->cooperate);
    printf("TotalMoney: %d \n", dataToSend->totalMoney);
    printf("IDGame: %d \n", dataToSend->iDGame);
    printf("Game Ended ? %d", dataToSend->gameEnded);
    printf(
        "----------------------------------------------------------------\n");
#endif

    hasGameEnded = dataRecieved->gameEnded;
    computeAndSend(client, dataRecieved, gameInfo, dataToSend);

  

  close(connection->sockfd);
  del(connection);
  free(connection);
  free(gameInfo);
  free(otherClientAddr);
  free(dataRecieved);
  free(dataToSend);
  pthread_exit(0);
}

void calculgains(gameStructure *iDGame) {
    //On retire les sommes pariées des pactoles
    iDGame->client1->pactole-=iDGame->client1->sommePariée;
    iDGame->client2->pactole-=iDGame->client2->sommePariée;

    //Si les deux joueurs trahissent...
    if (iDGame->client1->choix==0 && iDGame->client2->choix==0){     
      iDGame->client1->pactole+=iDGame->client1->sommePariée/2;    
      iDGame->client2->pactole+=iDGame->client2->sommePariée/2;
      iDGame->c1NbTrahison+=1;
      iDGame->c2NbTrahison+=1;
    }  
    //Si le joueur 1 trahit le joueur 2...
    else if (iDGame->client1->choix==0 && iDGame->client2->choix==1){
      iDGame->client1->pactole+=iDGame->client2->sommePariée; 
      iDGame->c1NbTrahison+=1;
      iDGame->c2NbCollab+=1;
    }
    //Si le joueur 2 trahit le joueur 1...
    else if (iDGame->client1->choix==1 && iDGame->client2->choix==0){
      iDGame->client2->pactole+=iDGame->client1->sommePariée; 
      iDGame->c1NbCollab+=1;
      iDGame->c2NbTrahison+=1;
    }
    //Si les deux joueurs collaborent...
    else {
      iDGame->client1->pactole+=iDGame->client1->sommePariée;    
      iDGame->client2->pactole+=iDGame->client2->sommePariée;
      iDGame->c1NbCollab+=1;
      iDGame->c2NbCollab+=1;
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


// TODO: ouverture/fermeture fichier pour sauvegarde scores
// TESTER envois C/S et S/C avec des structures
