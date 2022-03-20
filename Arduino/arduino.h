/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include <EEPROM.h>

// Arduino Uno EEPROM de 0 à 512 
#define EEPROM_UID_ADDRESS 0;

// Lit l'octet suivant sur la ligne série.
unsigned char getNextUnsignedChar();

// Vérifie la validité de la commande.
int checkCommand(command* cmd);
    
// Lit une commande sur la ligne serie.
int getCommand(command* cmd);

// Execute la commande.
int executeCommand(command cmd);

// Lit une valeur sur les pins "analog".
int getAnalogPin(unsigned char analogPinIndex);

// Envoie un entier sur la ligne série.
// En binaire (au lieu de ASCII).
int sendInteger(int value);

// Envoie une commande sur la ligne série.
// En binaire.
int sendCommand(command* cmd);

// Fait clignoter la led bultin toutes les INTERVAL_BUILTIN_LED_BLINK ms
// N'est pas bloquant, c'est à dire compatible avec polling actif.
void blinkBuiltIn();
void turnOffBuiltIn();

// Générer la commande d'erreur.
command getErrorCommand();

// Initialise la connexion serie;
void initSerial();

void initPWMPins();

// Traduit une addresse de Pin PWM symbolique en son addresse réelle.
int addressPWMPin(unsigned char indexPWMPin);

// Initialise l'ID unique de la board.
// Attention !! On grave la valeur dans l'eeprom.
// L'eeprom s'use à chaque écriture (environ 100 000 max)
int setBoardUID(unsigned char uid);

unsigned char getBoardUID();

