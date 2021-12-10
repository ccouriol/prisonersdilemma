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

#include <arpa/inet.h>
// #include <binn.h>
// https://github.com/liteserver/binn
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#ifndef SRVCXNMANAGER_H
#define SRVCXNMANAGER_H

#define BUFFERSIZE 2048
#define MAXSIMULTANEOUSCLIENTS 100
#define MAXSIMULTANEAOUSGAMES 50

// client envoie choix, mise et pactole
// serveur envoie Nouveau Pactole
typedef struct clientStructure {
  bool isInGame;
  int idClient;
  bool cooperate;
  unsigned long bet;
  unsigned long money;
} clientStructure;

typedef struct {
  int sockfd;
  struct sockaddr address;
  int addr_len;
  int index;
} connection_t;

typedef struct gameStructure {
  int idGame;
  int c1NbTreason;
  int c1NbCollab;
  int c2NbTreason;
  int c2NbCollab;
  int iDClient1;
  int iDClient2;
} gameStructure;

typedef struct dataSentReceived {
  unsigned long currentBet;
  unsigned long moneyGainLost;
  bool cooperate; // 1 collaborer     0 trahir
  unsigned long totalMoney;
  int iDGame;
  bool gameEnded;
  bool gameStarted;
} dataSentReceived;

void init_sockets_array();
int create_server_socket();
void add(connection_t *connection);
void del(connection_t *connection);

// void *threadProcess(void *ptr);

void createClient(clientStructure *client);
void addclient(clientStructure *client);
int verifyNbClients(int clientID);
void removeClient(int IDClient);
void disconnectAllClients(gameStructure *game);
void removeGame(gameStructure *iDGame);

gameStructure *initGame(int client1ID, int client2ID);
void profitsCalculation(clientStructure *client, gameStructure *gameInfo);
void saveOnfile(gameStructure *gameInfo);
void computeAndSend(clientStructure *client, dataSentReceived *dataRecieved,
                    gameStructure *gameInfo, dataSentReceived *dataToSend);

void *threadServeur(void *ptr);

void closeAll(connection_t *connection, gameStructure *gameInfo,
              dataSentReceived *dataRecieved, dataSentReceived *dataToSend,
              clientStructure *client);

#endif /* SRVCXNMANAGER_H */
