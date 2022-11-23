/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#ifndef SERIAL_SERVER_H
#define SERIAL_SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "CommunicationMicrocontrollers-Serial.h"
#include "CommunicationClients-UDP.h"
#include "Protocol.h"
#include "Logs.h"
#include "csapp.h"
#include "../Arduino/shared.h"

#define TRACE TRUE
#define DEBUG FALSE

#define NB_PINS_BY_BOARD 6

extern int LOG_FILE_FD;

// Retourne -1 si l'arduino n'a pas pu être connecté.
// Retourne -2 si la pin cible est inconnue.
// Retourne le uid destination si OK.
int routeCommand(command* cmd);

// Gere la communication serie avec les micro-controleurs.
// Verifie la commande.
// Identifie le micro controlleur de destination
// Envoie la requete
// Recoit la reponse
int handleCommand(command* request, command* response);

// Présume 6 pins par microcontroller
// Microcontroller UID[0] => Pins [0..5]
// Microcontroller UID[1] => Pins [6..11]
// Etc ...
int buildRoutingTable();

#endif
