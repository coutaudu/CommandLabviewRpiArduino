/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/


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
// En binaire (au lieu de ASCII)
int sendInteger(int value);
void blinkBuiltIn();
void turnOffBuiltIn();
