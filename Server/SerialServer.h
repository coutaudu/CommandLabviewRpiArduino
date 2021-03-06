/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

#include "csapp.h"

#include "../Arduino/shared.h"

#define TRACE   TRUE
#define DEBUG TRUE
#define SERIAL_FILE_0 "/dev/ttyACM0"
#define SERIAL_FILE_1 "/dev/ttyACM1"
#define TEMPO_TRY_AGAIN_OPEN_SERIAL 3
#define NB_MAX_TRY_AGAIN_OPEN_SERIAL 3

int LOG_FILE_FD;

// Ouvre et configure la ligne serie
int openSerial(char* serialFile);

// Ouvre et configure la reception UDP
int initUDP();

// Init les 2 files discriptors pour la communication serie avec les 2 cartes arduino
int initSerials(int* fd);

// Interroge l'arduino au bout du fil pour obtenir son UID
// (NB: Le UID doit etre inscrit manuellement de l'EEPROM)
// Si le arduino est bien reconnu, son fileDescritor est inscrit
// dans le table fd en indice=UID.
int identifyArduinoOnSerial(int fdTemp, int* fd);

// Prepare les parametres de communications serie.
// fd est le descripteur de fichier vers le pseudo fichier tty
// Il doit être ouvert.
int setSerialParameters(int fd);

// UDP Interface
int getCommandUDP(command* cmd, int socket, struct sockaddr_in* infosSocketClient);

// Envoie réponse de l'arduino a client source de la requete.
int sendResponseUDP(command* cmd, int socket, struct sockaddr_in* infosSocketClient);

// Gere la communication serie avec les micro-controleurs.
// Verifie la commande.
// Identifie le micro controlleur de destination
// Envoie la requete
// Recoit la reponse
int transmitCommandSerial(command* request, command* response, int* fdSerials);

// Retourne 0 (TRUE) si la commande est valide
//     Cad la commande est valide et la pin de destination est connue.
// Retourne -1 si la commande est inconnue
// Retourne -2 si la version est incorrecte.
int commandIsValid( command* cmd);

// Retourne -1 si l'arduino n'a pas pu être connecté.
// Retourne -2 si la pin cible est inconnue.
// Retourne le fd destination si OK.
int routeCommand(command* cmd, int* fdSerials);

// Envoie une commande sur la ligne série.
// En binaire.
int sendCommandSerial(command* cmd, int fdDestination);

// Reçoit une commande (une réponse donc)) sur la ligne série.
// En binaire.
int receiveCommandSerial(command* cmd, int fdSerial);

void printCommand(command* cmd);

// Traite la réponse
int logCommand(command* request, command* response);

// Genere et retourne la commande pour interroger l'arduino sur sont uid.
command requestUidCommand();

// Genere et retourne la commande d'erreur.
command errorCommand();


// Initialise le fichier de log du process
int initLog(char* filename);

