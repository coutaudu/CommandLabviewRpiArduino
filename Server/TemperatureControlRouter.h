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
#define DEBUG TRUE

extern int LOG_FILE_FD;


// Retourne -1 si l'arduino n'a pas pu être connecté.
// Retourne -2 si la pin cible est inconnue.
// Retourne le fd destination si OK.
int routeCommand(command* cmd, int* fdSerials);





#endif
