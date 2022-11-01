/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "TemperatureControlRouter.h"


int main(){
    int fdSerials[NB_MAX_SERIAL_DEVICES];
    command request, response;
    
    logInit("/tmp/TemperatureControlRouter.log");
    
    initUDP();
    initSerials(fdSerials);

    if(TRACE) printf("\tReady: Wait for commands.\n");
    while (TRUE) {
	if ( receiveCommandFromClient_UDP(&request) < 0){
	    if (TRACE) printf(" Received command is invalid.\n");
	    response = errorCommand();
	} else {
	    transmitCommandSerial(&request, &response, fdSerials );
	    if (TRACE) traceCommand(&request, &response);
	}
	sendResponseToClient_UDP(&response);	
    };

    // closeSerials
    // TODO For i in 0 NB_MAX_SERIAL_DEVICES : close
    close(fdSerials[0]);
    close(fdSerials[1]);
    if(TRACE) printf("\tSerial closed by peer.\n");
    return 0;
}


int routeCommand(command* cmd, int* fdSerials){
    int retval;
    unsigned char destinationPin;
    int destinationArduinoUid = -1;
    destinationPin = cmd->Argument[0];
    if ( destinationPin < 6 ) {
	cmd->Argument[0] -= 0;
	destinationArduinoUid = 0;
	retval = fdSerials[destinationArduinoUid];
    } else if ( destinationPin < 12 ){
	cmd->Argument[0] -= 6;
	destinationArduinoUid = 1;
	retval = fdSerials[destinationArduinoUid];
    }else{
	retval = -2;
    }

    
    if (retval == -1) {
	logPrint(ERROR_ARDUINO_NOT_CONNECTED,destinationArduinoUid);
    }
    
    if (retval == -2) {
	logPrint(ERROR_PIN_NOT_ROUTABLE,(int)destinationPin);
    }
    
    return retval;
}



