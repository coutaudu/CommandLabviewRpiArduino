/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "shared.h"
#include "arduino.h"

//////////////////////
// @A0=14
#define ANALOG_PIN_0 A0
#define DEBUG 1==2
#define INTERVAL_BUILTIN_LED_BLINK 500
#define NB_ANALOG_PINS 6

// VARIABLES GLOBALES PERSISTENTES
int DELAI = 100;
unsigned long previousMillis=0;
int builtInLedState = LOW;

/****************************************************/
// Exécuté à la mise sous tension de l'arduino.
void setup() {
    initSerial();
    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(11, OUTPUT);
}

// Exécuté en boucle à l'infini.
void loop() {
    command cmd;

    if(getCommand(&cmd)==0){
	executeCommand(cmd);
    }else{
	cmd = getErrorCommand();
	sendCommand(&cmd);
	blinkPin11();
    }
    delay(DELAI);
}


int getAnalogPin(unsigned char analogPinIndex){
    int analogPinAddress;
    int analogValue;
    command cmd;
    
    // Prepare commande "réponse" à envoyer
    cmd.Function = GET_ANALOG;

    if(analogPinIndex<NB_ANALOG_PINS){
	analogPinAddress = ANALOG_PIN_0 + analogPinIndex;
	analogValue = analogRead(analogPinAddress);
	cmd.Version = 1;
	*((int*)(cmd.Argument)) = analogValue;
    } else {
	// Si la pin demandée n'existe pas, on renvoie la commande d'erreur.
	cmd = getErrorCommand();
    }
    
    sendCommand(&cmd);
    return analogValue;
}



int executeCommand(command cmd){

    switch (cmd.Function){
    case GET_ANALOG:
	//	Serial.println((String)"Commande GET<"+cmd.Argument+"> sizeof(short)"+sizeof(short));
      	getAnalogPin(cmd.Argument[0]);
	return 0;
	break;
    default:
	//Serial.write((String)"Commande ["+cmd.Function+"] inconnue.");
	return -1;
    }    
}


int getCommand(command* cmd){
    cmd->Version  = getNextUnsignedChar();
    cmd->Function = getNextUnsignedChar();
    cmd->Argument[0] = getNextUnsignedChar();
    cmd->Argument[1] = getNextUnsignedChar();
	
    return checkCommand(cmd);
}

void blinkPin11(){
    int thisPin=11;
    for (int brightness = 0; brightness < 255; brightness+=5) {
	analogWrite(thisPin, brightness);
	delay(2);
    }
    // fade the LED on thisPin from brightest to off:
    for (int brightness = 255; brightness >= 0; brightness-=5) {
	analogWrite(thisPin, brightness);
	delay(2);
    }
}


void blinkBuiltIn(){
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= INTERVAL_BUILTIN_LED_BLINK) {
	previousMillis = currentMillis;
	if (builtInLedState == LOW) {
	    builtInLedState = HIGH;
	} else {
	    builtInLedState = LOW;
	}
	digitalWrite(LED_BUILTIN, builtInLedState);
    }
}

void turnOffBuiltIn(){
    digitalWrite(LED_BUILTIN, LOW);
}

/****************************************************/

unsigned char getNextUnsignedChar(){
    int in;
    
    while (Serial.available()<=0) {
	blinkBuiltIn();
    }
    in =  Serial.read();
    return (unsigned char)in;
}

int checkCommand(command* cmd){
    if (cmd->Version!=1) {return -1;}
    if (cmd->Function>2) {return -2;}
    return 0;   
}


int sendInteger(int value){
    int nbBytesSent;
    Serial.write((char*)(&value),2); // sizeof(Int)==2 sur Arduino Uno.
    Serial.flush();

    return nbBytesSent;
}

int sendCommand(command* cmd){
    int nbBytesSent;
    nbBytesSent = Serial.write((char*)cmd,sizeof(command)); 
    Serial.flush();

    return nbBytesSent;
}

command getErrorCommand(){
    command cmd;
    cmd.Version  = 1;
    cmd.Function = 0;
    cmd.Argument[0] = 0;
    cmd.Argument[1] = 0;

    return cmd;
}

void initSerial(){
    Serial.begin(115200,SERIAL_8N1);
    while (!Serial) {
	; // wait for serial port to connect. Needed for native USB
    }
    //Vide la ligne série.
    while (Serial.available()>0) {
	Serial.read();
    }
}
