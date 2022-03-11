/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

// @A0=14
#define ANALOG_PIN_0 A0


// VARIABLES GLOBALES PERSISTENTES
int DELAI = 1000;

/****************************************************/
// FONCTIONS
// Ecrit  le menu d'interface CLI sur la ligne serie.
void printMenu();

// Lit une commande sur la ligne serie.
int lectureSerial();

// Execute une commande
void executeCommande(int cmd);

// Lit tout les entrees analogiques et écrit les valeurs sur la ligne série
// Format Human readable
void getAllAnalogPinsHR();
// Format CSV 
void getAllAnalogPinsCSV();


/****************************************************/
// Exécuté à la mise sous tension de l'arduino.
void setup() {
  Serial.begin(9600);           //  setup serial
}

// Exécuté en boucle à l'infini.
void loop() {

  int commande=-1;
      
  commande = lectureSerial();
  executeCommande(commande);
  
  delay(DELAI);
}
/****************************************************/

void getAllAnalogPins(){
    int analogPinAddress;
    int analogPinIndex;
    int val[6] ;  // variable to store the value read

    for(analogPinIndex=0;analogPinIndex<=5;analogPinIndex++){
	analogPinAddress = ANALOG_PIN_0 + analogPinIndex;
	Serial.println(analogPinIndex);
	val[analogPinIndex] = analogRead(analogPinAddress);  // read the input pin
	Serial.println((String)"Pin["+analogPinIndex+"]<"+val[analogPinIndex]+">");
    }
    Serial.println((String)"-----------------"); 
    
}

void executeCommande(int cmd){
    switch (cmd){
    case '1':
	getAllAnalogPins();
	break;
    case -1:
	return;
	break;	    
    default:
	Serial.println((String)"Commande ["+cmd+"] inconnue.");	    
    }
}
    
void printMenu(){
    Serial.println       ("/**********************************************************/");
    Serial.println((String)" Tapez votre commande:");
    Serial.println((String)"\t[1] Lire valeur analogique.");
    Serial.println       ("/**********************************************************/");
}

int lectureSerial() {
    int retval = -1;
    char inChar;
    while (Serial.available()>0) {
	inChar = (char)Serial.read();
	if (inChar<' ' || inChar>'~' || inChar=='?' ) {
	    printMenu();
	}
	if (inChar>=' ' && inChar<='~') {
	    Serial.println((String)"\tSerial.read["+((char)inChar)+"]");
	}
	if (inChar=='1') { retval=inChar;}
    }
    return retval;
}
