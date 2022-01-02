/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * File:   srvcxnmanager.h
 * Author: aurelio
 *
 */

#include "../../lib/libutil.h"
#include <arpa/inet.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#ifndef SERVER_H
#define SERVER_H

#define BUFFERSIZE 2048
#define MAXSIMULTANEOUSCLIENTS 100
#define MAXSIMULTANEAOUSGAMES 50

typedef struct gameStructure {
  int idGame;
  int c1NbTreason;
  int c1NbCollab;
  int c2NbTreason;
  int c2NbCollab;
  int iDClient1;
  int iDClient2;
  int nbrounds;
  bool isCalcFinished;
  bool hasGameEned;
} gameStructure;

// client envoie choix, mise et pactole
// serveur envoie Nouveau Pactole
typedef struct clientStructure {
  bool isInGame;
  int idClient;
  bool cooperate;
  unsigned long bet;
  unsigned long money;
  bool isFilled;
  gameStructure *gameP;
  bool canFree;
} clientStructure;

typedef struct {
  int sockfd;
  struct sockaddr address;
  int addr_len;
  int index;
} connection_t;

typedef struct dataSentReceived {
  unsigned long currentBet;
  bool cooperate; // 1 collaborer     0 trahir
  bool gameEnded;
  bool gameStarted;
} dataSentReceived;

void init_sockets_array();
void add(connection_t *connection);
void del(connection_t *connection);
int create_server_socket();

// void *threadProcess(void *ptr);

void createClient(clientStructure *client);
void removeClient(int IDClient);
void disconnectAllClients(gameStructure *game);
int verifyNbClients(int clientID);
void initNBRounds();
int initBaseMoney();
void initGame(int client1ID, int client2ID);
void removeGame(gameStructure *iDGame);
void closeAll(connection_t *connection, gameStructure *gameInfo,
              dataSentReceived *dataRecieved, dataSentReceived *dataToSend,
              clientStructure *client);
void closeLocal(connection_t *connection, dataSentReceived *dataRecieved,
                dataSentReceived *dataToSend, clientStructure *client);
void initDataToSend(dataSentReceived *dataToSend, clientStructure *client);
void saveOnfile(gameStructure *gameInfo);
void *threadServeur(void *ptr);
void fill(clientStructure *client, dataSentReceived *dataRecieved);
bool computeAndSend(clientStructure *client, dataSentReceived *dataRecieved,
                    gameStructure *gameInfo, dataSentReceived *dataToSend);
void profitsCalculation(clientStructure *client, gameStructure *gameInfo);
void init_clients_array();

#endif /* SERVER_H */
