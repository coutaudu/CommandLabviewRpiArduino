/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

//DEFITION STRUCTURES ET TYPES
typedef struct command{
    unsigned char Version; // Version 0. 1-255 RFU.
    unsigned char Function; // 0 getAnalog, 1 setDigital. 2-255 RFU.
    unsigned char Argument; 
}command;

