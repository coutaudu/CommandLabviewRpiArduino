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
int receiveCommandFromClient_UDP(command* cmd);

// Envoie réponse de l'arduino a client source de la requete.
int sendResponseToClient_UDP(command* cmd);



#endif
