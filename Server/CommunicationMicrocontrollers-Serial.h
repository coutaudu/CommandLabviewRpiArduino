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


#define TEMPO_TRY_AGAIN_OPEN_SERIAL 3
#define NB_MAX_TRY_AGAIN_OPEN_SERIAL 3
#define NB_MAX_SERIAL_DEVICES 3


// Init les 2 files discriptors pour la communication serie avec les 2 cartes arduino
int initSerials(int* fd);

// Ouvre et configure la ligne serie
int openSerial(char* serialFile);

// Cherche la disponibilité des devices.
// Par exemple au format /dev/ttyACM*
int detectAvailableDevices(char** devNames, char* basename, char* repertory);

// Prepare les parametres de communications serie.
// fd est le descripteur de fichier vers le pseudo fichier tty
// Il doit être ouvert.
int setSerialParameters(int fd);

// Interroge l'arduino au bout du fil pour obtenir son UID
// (NB: Le UID doit etre inscrit manuellement de l'EEPROM)
// Si le arduino est bien reconnu, son fileDescritor est inscrit
// dans le table fd en indice=UID.
int identifyArduinoOnSerial(int fdTemp, int* fd);

// Reçoit une commande (une réponse donc)) sur la ligne série.
// En binaire.
int receiveCommandSerial(command* cmd, int fdSerial);

// Envoie une commande sur la ligne série.
// En binaire.
int sendCommandSerial(command* cmd, int fdDestination);
// Gere la communication serie avec les micro-controleurs.
// Verifie la commande.
// Identifie le micro controlleur de destination
// Envoie la requete
// Recoit la reponse
int transmitCommandSerial(command* request, command* response, int* fdSerials);

#endif
