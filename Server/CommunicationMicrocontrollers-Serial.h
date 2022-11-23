/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#ifndef USB_SERIAL_DEVICES_H
#define USB_SERIAL_DEVICES_H

#include "TemperatureControlRouter.h"
#include "../Arduino/shared.h"
#include <dirent.h> 
#include <termios.h>

/***********/
/* PUBLIC  */
/***********/

// Init les x Files Descriptors pour la communication serie avec les 2 cartes arduino
int initSerials();

// Reçoit une commande (une réponse donc)) du micro controller sur la ligne série.
// En binaire.
int receiveCommandFromMicrocontroller_Serial(command* cmd, int microcontrollerUid);

// Envoie une commande au microcontroller sur la ligne série.
// En binaire.
int sendCommandToMicrocontroller_Serial(command* cmd, int microcontrollerUid);

/***********/
/* PRIVATE */
/***********/

// Dans le fichier .c

#endif
