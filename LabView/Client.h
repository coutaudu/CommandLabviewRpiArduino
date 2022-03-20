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

#include "../Arduino/shared.h"

/**********/
// Command Line Interface
int getCommandCLI(command* cmd); 

// Recoit instruction via interface console.
int commandGetAnalog(command* cmd);

// Recoit instruction via interface console.
int commandSetDigitalPWM(command* cmd);

// Recoit instruction via interface console.
int commandGetUID(command* cmd);

/**********/



int initUDP();
