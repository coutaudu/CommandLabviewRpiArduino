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

