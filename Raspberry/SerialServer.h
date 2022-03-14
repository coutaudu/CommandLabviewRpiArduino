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
#include "csapp.h"

#include "../Arduino/shared.h"

#define LOG TRUE
#define SERIAL_FILE_1 "/dev/ttyACM0"
#define SERIAL_FILE_2 "/dev/ttyACM1"
#define TEMPO_TRY_AGAIN_OPEN_SERIAL 3


// Ouvre et configure la ligne serie
int openSerial(char* serialFile);

// Command Line Interface
int CLI(command* cmd); 

int commandGetAnalog(command* cmd);
