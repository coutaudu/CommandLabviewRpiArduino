/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

//DEFITION STRUCTURES ET TYPES
typedef struct command{
    unsigned char Version; // Version 1. 2-255 RFU. 0 Error
    unsigned char Function; // 1 getAnalog, 2 setDigital. 3-255 RFU.
    unsigned char Argument[2];
}command;

#define INVALID_CMD 0
#define GET_ANALOG 1
#define SET_DIGITAL 2

#define TRUE 1==1
#define FALSE 0==2

#define NB_ANALOG_PINS 6
#define NB_DIGITAL_PWM_PINS 6

#define CURRENT_VERSION 1
