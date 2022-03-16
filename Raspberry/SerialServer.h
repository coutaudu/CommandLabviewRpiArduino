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
#define SERIAL_FILE_1 "/dev/ttyACM0"
#define SERIAL_FILE_2 "/dev/ttyACM1"
#define TEMPO_TRY_AGAIN_OPEN_SERIAL 3
#define PORT 7191   // Port d'écoute.


// Ouvre et configure la ligne serie
int openSerial(char* serialFile);

// Ouvre et configure la reception UDP
int openUDP();

// UDP Interface
int getCommandUDP(command* cmd, int socket);

// Command Line Interface
int getCommandCLI(command* cmd); 

// Recois instruction via interface console.
int commandGetAnalog(command* cmd);

// Recois instruction via interface console.
int commandSetDigitalPWM(command* cmd);

// Envoie une commande sur la ligne série.
// En binaire.
int sendCommand(command* cmd, int fdDestination);

// Reçoit une commande (une réponse donc)) sur la ligne série.
// En binaire.
int receiveCommand(command* cmd, int fdSerial);

void printCommand(command* cmd);

// Traite la réponse
int handleCommand(command* request, command* response);
