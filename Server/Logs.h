/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#ifndef LOGS_H
#define LOGS_H
#include <time.h>
#include "Protocol.h"

#define ERROR_ARDUINO_NOT_CONNECTED 1
#define ERROR_PIN_NOT_ROUTABLE 2

// Initialise le fichier de log du process
int logInit(char* filename);

// Note la commande dans les logs.
int logCommand(command* request, command* response);

// Inscrit une erreur dans le fichier de log
int logPrint(int errorCode, int arg);

#endif
