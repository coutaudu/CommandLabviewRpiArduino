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
int DELAI = 5000;

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
    
    int val = 666;
    Serial.write((char)(val>>0));
    Serial.write((char)(val>>8));
    Serial.flush();
    if(((char)(val>>0))==0) Serial.println("A\n");
    if(((char)(val>>8))==0) Serial.println("B\n");

    /* command cmd; */
    /* if(getCommand(&cmd)==0){ */
    /* 	if (DEBUG) Serial.write("getCommand OK\n"); */
    /* 	executeCommand(cmd); */
    /* }else{ */
    /* 	unsigned int err = 0xFFFF; */
    /* 	Serial.write((char*)(&err),sizeof(int)); */
    /* 	Serial.flush(); */
    /* 	if (DEBUG) Serial.write("getCommand NOT OK\n"); */
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

#define INTERVAL_BUILTIN_LED_BLINK 500
unsigned long previousMillis=0;
int builtInLedState = LOW;
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
    int n;
    if (DEBUG) Serial.write("E");
    
    while (Serial.available()<=0) {
	blinkPin11();
    }
    in =  Serial.read();
    return (unsigned char)in;
}

int checkCommand(command* cmd){
    if (cmd->Version!=1) {return -1;}
    if (cmd->Function>2) {return -2;}
    return 0;   
}

int getCommand(command* cmd){
    cmd->Version  = getNextUnsignedChar();
    cmd->Function = getNextUnsignedChar();
    cmd->Argument = getNextUnsignedChar();
	
    return checkCommand(cmd);
}

int executeCommand(command cmd){

    switch (cmd.Function){
    case GET_ANALOG:
	//	Serial.println((String)"Commande GET<"+cmd.Argument+"> sizeof(short)"+sizeof(short));
      	getAnalogPin(cmd.Argument);
	return 0;
	break;
    default:
	//Serial.write((String)"Commande ["+cmd.Function+"] inconnue.");
	return -1;
    }    
}

int sendInteger(int value){
    Serial.write(((char*)&value),2); // sizeof(Int)==2 sur Arduino Uno.
    Serial.flush();
}

int getAnalogPin(unsigned char analogPinIndex){
    int analogPinAddress;
    int analogValue;

    analogPinAddress = ANALOG_PIN_0 + analogPinIndex;
    analogValue = analogRead(analogPinAddress);
    sendInteger(analogValue);
    
    return analogValue;
}

