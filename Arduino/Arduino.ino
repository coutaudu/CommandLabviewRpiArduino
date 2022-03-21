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


// VARIABLES GLOBALES PERSISTENTES
int DELAI = 100;
unsigned long previousMillis=0;
int builtInLedState = LOW;

/****************************************************/
// Exécuté à la mise sous tension de l'arduino.
void setup() {
    initSerial();
    initPWMPins();
    pinMode(LED_BUILTIN, OUTPUT);

    //    setBoardUID(1);
}

// Exécuté en boucle à l'infini.
void loop() {
    command cmd;
    
    if(getCommand(&cmd)==0){
	executeCommand(cmd);
    }else{
	cmd = getErrorCommand();
	sendCommand(&cmd);
    }
    delay(DELAI);
}



int getBoardUID(){
    command cmd;

    cmd.Version = CURRENT_VERSION;
    cmd.Function = GET_UID;
    cmd.Argument[0] = (unsigned char) EEPROM.read(EEPROM_UID_ADDRESS);
//    cmd.Argument[1] = 0; //EEPROM.read(EEPROM_UID_ADDRESS);

    sendCommand(&cmd);


    return 0  ;
}

int getAnalogPin(unsigned char analogPinIndex){
    int analogPinAddress;
    int analogValue;
    int retval;
    command cmd;
    
    if(analogPinIndex<NB_ANALOG_PINS){
	analogPinAddress = ANALOG_PIN_0 + analogPinIndex;
	analogValue = analogRead(analogPinAddress);
	cmd.Version = CURRENT_VERSION;
	cmd.Function = GET_ANALOG;
	*((int*)(cmd.Argument)) = analogValue;
	retval = analogValue;
    } else {
	// Si la pin demandée n'existe pas, on renvoie la commande d'erreur.
	cmd = getErrorCommand();
	retval = -1;
    }
   
    sendCommand(&cmd);
    return retval;
}

int setDigitalPin(unsigned char digitalPinIndex, unsigned char value){
    int pin;
    int retval;
    command cmd;
    
    pin = addressPWMPin(digitalPinIndex);
    if ( pin != -1){
	analogWrite(addressPWMPin(digitalPinIndex), (int)value);
	cmd.Version = CURRENT_VERSION;
	cmd.Function = SET_DIGITAL;
	cmd.Argument[0] = digitalPinIndex;
	cmd.Argument[1] = value;
	retval = 0;
    }else {
	cmd = getErrorCommand();
	retval = -1;
    }

    sendCommand(&cmd);
    return retval;
}
    

int executeCommand(command cmd){
    switch (cmd.Function){
    case GET_ANALOG:
      	getAnalogPin(cmd.Argument[0]);
	return 0;
	break;
    case SET_DIGITAL:
	setDigitalPin(cmd.Argument[0],cmd.Argument[1]);
	return 0;
	break;
    case GET_UID:
	getBoardUID();
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
    if (cmd->Version!=CURRENT_VERSION) {return -1;}
    if (
	cmd->Function!=GET_ANALOG
	&& cmd->Function!=SET_DIGITAL
	&& cmd->Function!=GET_UID
	) {return -2;}
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
    cmd.Version  = CURRENT_VERSION;
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

int addressPWMPin(unsigned char indexPWMPin){
    switch (indexPWMPin){
    case 0:
	return 11;
	break;
    case 1:
	return 10;
	break;
    case 2:
	return 9;
	break;
    case 3:
	return 6;
	break;
    case 4:
	return 5;
	break;
    case 5:
	return 3;
	break;
    default:
	return -1;
	break;
    }
}

void initPWMPins(){
    int i;
    for  (i = 0; i<NB_DIGITAL_PWM_PINS; i++){
	pinMode(addressPWMPin(i), OUTPUT);
    }
}

int setBoardUID(unsigned char uid){
    EEPROM.update(EEPROM_UID_ADDRESS,uid);
}
