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


#ifndef SRVCXNMANAGER_H
#define SRVCXNMANAGER_H

#define BUFFERSIZE 2048
#define MAXSIMULTANEOUSCLIENTS 100


typedef struct {
  int sockfd;
  struct sockaddr address;
  int addr_len;
  int index;
} connection_t;

// client envoie choix, mise et pactole
//serveur envoie Nouveau Pactole
typedef struct structureClient{
  bool isInGame;
  int idClient;
  bool choix;
  int sommePariée;
  int pactole;
} structureClient;

typedef struct structureClient{
  int idPartie;
  int c1NbTrahison;
  int c1NbCollab;
  int c2NbTrahison;
  int c2NbCollab;
  struct structureClient client;
} instanceJeu;

void init_sockets_array();
void add(connection_t *connection);
void del(connection_t *connection);
void *threadProcess(void *ptr);
int create_server_socket();

#endif /* SRVCXNMANAGER_H */
