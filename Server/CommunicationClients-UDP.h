/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#ifndef UDP_IP_NETWORK_H
#define UDP_IP_NETWORK_H
#include "TemperatureControlRouter.h"
#include "../Arduino/shared.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include "csapp.h"


// Ouvre et configure la reception UDP
int initUDP();


// UDP Interface
int getCommandUDP(command* cmd, int socket, struct sockaddr_in* infosSocketClient);

// Envoie réponse de l'arduino a client source de la requete.
int sendResponseUDP(command* cmd, int socket, struct sockaddr_in* infosSocketClient);



#endif
