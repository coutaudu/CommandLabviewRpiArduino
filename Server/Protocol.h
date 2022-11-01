/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#ifndef PROTOCOL_H
#define PROTOCOL_H
#include "TemperatureControlRouter.h"
#include "../Arduino/shared.h"

// Retourne 0 (TRUE) si la commande est valide
//     Cad la commande est valide et la pin de destination est connue.
// Retourne -1 si la commande est inconnue
// Retourne -2 si la version est incorrecte.
int commandIsValid(command* cmd);

void printCommand(command* cmd);

// Genere et retourne la commande pour interroger l'arduino sur sont uid.
command requestUidCommand();

// Genere et retourne la commande d'erreur.
command errorCommand();


#endif
