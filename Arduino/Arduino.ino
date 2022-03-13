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

// VARIABLES GLOBALES PERSISTENTES
int DELAI = 2000;

/****************************************************/
// Exécuté à la mise sous tension de l'arduino.
void setup() {
    Serial.begin(115200,SERIAL_8N1);//  setup serial
    while (!Serial) {
	; // wait for serial port to connect. Needed for native USB
    }
    pinMode(11, OUTPUT);
	
}

// Exécuté en boucle à l'infini.
void loop() {
    command cmd;
    Serial.write("LOOP\r\n");
    blinkPin11();
    /* if(getCommand(&cmd)==0){ */
    /* 	if (DEBUG) Serial.write("getCommand OK"); */
    /* 	executeCommand(cmd); */
    /* }else{ */
    /* 	if (DEBUG) Serial.write("getCommand NOT OK"); */
    /* } */

    delay(DELAI);
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
/****************************************************/
unsigned char getNextUnsignedChar(){
    int in;
    int n;
    if (DEBUG) Serial.write("E");
    
    while (Serial.available()<=0) {
	blinkPin11();
    }
    in =  Serial.read();
    delay(5);
    if (DEBUG) Serial.write("\nin[");
    if (DEBUG) if ( in<=126 && in>=' ') Serial.write(in);
    if (DEBUG) Serial.write("]\n");
    if (DEBUG) Serial.write("F");
    return (unsigned char)in;
}

int checkCommand(command* cmd){
    if (cmd->Version!=1) {return -1;}
    if (cmd->Function>2) {return -2;}
    return 0;   
}

int getCommand(command* cmd){
    if (DEBUG) Serial.write("A");
    cmd->Version  = getNextUnsignedChar();
    if (DEBUG) Serial.write("B");
    cmd->Function = getNextUnsignedChar();
    if (DEBUG) Serial.write("C");
    cmd->Argument = getNextUnsignedChar();
    if (DEBUG) Serial.write("D");
	
    return checkCommand(cmd);
}

int executeCommand(command cmd){

    switch (cmd.Function){
    case GET_ANALOG:
	if (DEBUG) Serial.write("F");
	//	Serial.write((String)"Commande GET<"+cmd.Argument+">");
	getAnalogPin(cmd.Argument);
	return 0;
	break;
    default:
	//Serial.write((String)"Commande ["+cmd.Function+"] inconnue.");
	return -1;
    }    
}

int getAnalogPin(unsigned char analogPinIndex){
    int analogPinAddress;
    int analogValue;

    //    if (DEBUG) Serial.write();
    analogPinAddress = ANALOG_PIN_0 + analogPinIndex;
    analogValue = analogRead(analogPinAddress);
    //    if (DEBUG) Serial.write((String)"A"+analogPinIndex+", "+analogValue+",");
    //    if (DEBUG) Serial.write();

    return analogValue;
}

