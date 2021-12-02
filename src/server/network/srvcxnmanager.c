#include "../../../include/server/network/srvcxnmanager.h"

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
  return ret = NULL;
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
      games[i] = game;
      games[i]->client1 = client1;
      games[i]->client2 = client2;
      games[i]->idPartie = i;
      client1->isInGame = true;
      client2->isInGame = true;
      return game;
    }
  }
  perror("No game initialized");
  exit(-7);
}

void profitsCalculation(gameStructure *gameInfo) {
    clientStructure *client1 = &(gameInfo.client1);
    clientStructure *client2 = &(gameInfo.client2);

    //On retire les sommes pariées des pactoles
    client1.money -= client1.bet;
    client2.money -= client1.bet;
    gameInfo->client1->pactole-=gameInfo->client1->sommePariée;
    gameInfo->client2->pactole-=gameInfo->client2->sommePariée;

    //If they both betray they gain half their bet
    if (client1.cooperate == 0 && client2.cooperate == client1.cooperate ){  
      client1.money += client1.bet/2;
      client2.money += client1.bet/2;   
      gameInfo->client1->pactole+=gameInfo->client1->sommePariée/2;    
      gameInfo->client2->pactole+=gameInfo->client2->sommePariée/2;

      gameInfo->c1NbTreason += 1;
      gameInfo->c2NbTreason += 1;
    }  
    //If C1 betray C2, C1 gain C2's bet
    else if (client1.cooperate == 0 && client2.cooperate == 1){
      client1.money += client2.bet;
      gameInfo->client1->pactole+=gameInfo->client2->sommePariée; 

      gameInfo->c1NbTreason += 1;
      gameInfo->c2NbCollab += 1;
    }
    //If C2 betray C12, C2 gain C1's bet
    else if (client1.cooperate == 1 && client2.cooperate == 0){
      client2.money +=client1.bet;

      gameInfo->client2->pactole+=gameInfo->client1->sommePariée;

      gameInfo->c1NbCollab+=1;
      gameInfo->c2NbTreason+=1;
    }
    //If they both collaborate
    else {
      client1.money += client1.bet;
      client2.money += client2.bet;

      gameInfo->client1->pactole+=gameInfo->client1->sommePariée;    
      gameInfo->client2->pactole+=gameInfo->client2->sommePariée;

      gameInfo->c1NbCollab+=1;
      gameInfo->c2NbCollab+=1;
    }
}

//TODO: upgrade for a better filename
void saveOnfile(gameStructure *gameInfo){

  FILE *fich;
  int nbTotalTreason = gameInfo->c1NbTreason + gameInfo->c2NbTreason;
  int nbTotalCollab = gameInfo->c1NbCollab + gameInfo->c2NbCollab;


  fich = fopen("NOMFICH", "a");
  if(fich == NULL){
    perror("error opening file \n"); 
    exit(EXIT_FAILURE);
  }

  fseek(fich, 0, SEEK_END);

  fprintf(fich, "Game number: %d", games.);
  fprintf(fich, "Number of collaboration for player 1: %d", gameInfo->c1NbCollab);
  fprintf(fich, "Number of treason for player 1: %d", gameInfo->c1NbTreason);
  fprintf(fich, "Number of collaboration for player 2: %d", gameInfo->c2NbCollab);
  fprintf(fich, "Number of treason for player 2: %d", gameInfo->c2NbTreason);

  fprintf(fich, "Total number of treason: %d", nbTotalTreason);
  fprintf(fich, "Total number of collaborations: %d", nbTotalCollab);

  fputc(fich, "\0");
  fclose(fich);
}

void *threadServeur(void *ptr) {
  char buffer_in[BUFFERSIZE];
  char buffer_out[BUFFERSIZE];

  //creation of the buffer to receive and send data
  dataSentReceived *dataRecieved = malloc(sizeof(dataSentReceived));
  dataSentReceived *dataToSend = malloc(sizeof(dataSentReceived));

  int moneyGainLost;
  int lenMsgIn;
  bool hasGameEnded = false;
  clientStructure *otherClientAddr;
  gameStructure *gameInfo = NULL;
  connection_t *connection;
  
  if (!ptr)
    pthread_exit(0);
  connection = (connection_t *)ptr;
  printf("New incoming connection \n");

  add(connection);
  connection->client.idClient = connection->index;
  connection->client.isInGame = false;

  printf("Welcome #%i\n", connection->client.idClient);

  // Verification of the number of clients available, minus this client's ID
  // and creation of the game if there is enough clients
  while (!(connection->client.isInGame)) {
    otherClientAddr = verifyNbClients(connection->client.idClient);
    if (otherClientAddr != NULL) {
      gameInfo = initGame(otherClientAddr, &(connection->client));
    }
  }

  if (gameInfo == NULL) {
    perror("No game initialized");
  }else{//sendind the game ID to tell the client the game has started
    dataToSend->iDGame = gameInfo->idGame;
    write(connection->sockfd, dataToSend, sizeof(dataSentReceived));
  }

  while(!hasGameEnded){
    read(connection->sockfd, buffer_in, (sizeof(dataSentReceived)));
    dataRecieved = buffer_in;
    memset(buffer_in, 0, sizeof(buffer_in));
    hasGameEnded = dataRecieved.gameEnded;

    // Reception and filling of the client structure
    connection->client.money = dataRecieved->totalMoney;
    connection->client.choice = dataRecieved.cooperate;
    connection->client.bet = dataRecieved->currentBet;

  // puis traitement et renvoi
    profitsCalculation(gameInfo);
    dataToSend->totalMoney = connection->client.money;
    dataToSend->cooperate = connection->client.choice;
    dataToSend->currentBet = connection->client.bet;
    if(gameInfo->client1->idClient == connection->client.idClient){
      dataToSend->moneyGainLost = gameInfo->client1->bet;
    }else dataToSend->moneyGainLost = gameInfo->client2->bet;

    write(connection->sockfd,dataToSend, sizeof(dataSentReceived));
  }
  //saving on file, but only if the client's ID is an odd number
  if(connection->index %2 == 0){
    saveOnfile(gameInfo);
  }

  close(connection->sockfd);
  del(connection);
  free(connection);
  free(gameInfo);
  free(otherClientAddr);
  free(dataRecieved);
  free(dataToSend);
  pthread_exit(0);
}

//TODO: upgrade dor a better filename and location
//TODO: functions descriptor