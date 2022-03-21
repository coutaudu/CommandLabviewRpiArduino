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

#include "csapp.h"

#include "../Arduino/shared.h"

#define LOG   TRUE
#define DEBUG FALSE
#define SERIAL_FILE_0 "/dev/ttyACM0"
#define SERIAL_FILE_1 "/dev/ttyACM1"
#define TEMPO_TRY_AGAIN_OPEN_SERIAL 3



// Ouvre et configure la ligne serie
int openSerial(char* serialFile);

// Ouvre et configure la reception UDP
int openUDP();

// Init les 2 files discriptors pour la communication serie avec les 2 cartes arduino
int initFileDescriptorSerialLine(int* fd);

int identifyArduinoOnSerial(int fdTemp, int* fd);

// Prepare les parametres de communications serie.
// fd est le descripteur de fichier vers le pseudo fichier tty
// Il doit être ouvert.
int setSerialParameters(int fd);

// UDP Interface
int getCommandUDP(command* cmd, int socket);

// Envoie réponse de l'arduino a client source de la requete.
int sendResponseToClientUDP(command* cmd);

// Envoie une commande sur la ligne série.
// En binaire.
int sendCommand(command* cmd, int fdDestination);

// Reçoit une commande (une réponse donc)) sur la ligne série.
// En binaire.
int receiveCommand(command* cmd, int fdSerial);

void printCommand(command* cmd);

// Traite la réponse
int logCommand(command* request, command* response);

// Genere et retourne la commande pour interroger l'arduino sur sont uid.
command requestUidCommand();
