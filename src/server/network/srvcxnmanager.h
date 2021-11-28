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
#include <binn.h>
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
  bool choix;
  int sommePariée;
  int pactole;
} clientStructure;

typedef struct {
  int sockfd;
  struct sockaddr address;
  int addr_len;
  int index;
  struct clientStructure client;
} connection_t;

typedef struct roundStructure {
  int client1Choice;
  int client2Choice;
  int client1Bet;
  int client2Bet;
  int client1Result;
  int client2Result;
} roundStructure;

typedef struct gameStructure {
  int idPartie;
  int c1NbTrahison;
  int c1NbCollab;
  int c2NbTrahison;
  int c2NbCollab;
  clientStructure *client1;
  clientStructure *client2;
  struct roundStructure roundResults;
} gameStructure;

void init_sockets_array();
void add(connection_t *connection);
void del(connection_t *connection);
void *threadProcess(void *ptr);
gameStructure *initGame(clientStructure *client1, clientStructure *client2);
clientStructure *verifyNbClients(int clientID);
void *threadServeur(void *ptr);

int create_server_socket();

#endif /* SRVCXNMANAGER_H */
