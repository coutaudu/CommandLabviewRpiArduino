/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "TemperatureControlRouter.h"


int main(){
    command request, response;
    
    logInit("/tmp/TemperatureControlRouter.log");
    
    initUDP();
    initSerials();

    if(TRACE) printf("\tReady: Wait for commands.\n");
    while (TRUE) {
	if ( receiveCommandFromClient_UDP(&request) < 0){
	    response = errorCommand();
	} else {
	    handleCommand(&request, &response);
	    if (TRACE) traceCommand(&request, &response);
	}
	sendResponseToClient_UDP(&response);	
    };

    // closeSerials
    // TODO Funciton For i in 0 NB_MAX_SERIAL_DEVICES : close
    //close(fdSerials[0]);
    //close(fdSerials[1]);
    if(TRACE) printf("\tSerial closed by peer.\n");
    return 0;
}

int handleCommand(command* request, command* response){
    int arduinoUidTmp;
    if ( commandIsValid(request) < 0){
	printf("Command is not valid.\n");
	return -1;
    }
    if ( (arduinoUidTmp = routeCommand(request)) < 0 ){
	printf("Failed routing command.\n");
	return -1;
    }
    if ( sendCommandToMicrocontroller_Serial(request,arduinoUidTmp) < 0 ){
	printf("Failed to send request command.\n");
	return -1;
    }
    if ( receiveCommandFromMicrocontroller_Serial(response,arduinoUidTmp) < 0 ){
	printf("Failed to send response command.\n");
	return -1;
    }
    return 0; 
}


int routeCommand(command* cmd){
    unsigned char destinationPin;
    int destinationArduinoUid = -1;
    destinationPin = cmd->Argument[0];

    // TODO remplacer par table de routage
    if ( destinationPin < 6 ) {
	cmd->Argument[0] -= 0;
	destinationArduinoUid = 0;
    } else if ( destinationPin < 12 ){
	cmd->Argument[0] -= 6;
	destinationArduinoUid = 1;
    }else{
	destinationArduinoUid = -2;
    }

    if (destinationArduinoUid == -1) {
	logPrint(ERROR_ARDUINO_NOT_CONNECTED,destinationArduinoUid);
    }
    
    if (destinationArduinoUid == -2) {
	logPrint(ERROR_PIN_NOT_ROUTABLE,(int)destinationPin);
    }
    
    return destinationArduinoUid;
}



