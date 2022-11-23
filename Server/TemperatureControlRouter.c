/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "TemperatureControlRouter.h"

// 255 car UID des pins définit sur sur 8 bits dans le format des commandes.
#define MAX_PIN_UID 255

int pinUidToMicrocontrollerUidRoutingTable[MAX_PIN_UID];

int main(){
    command request, response;
    
    logInit("/tmp/TemperatureControlRouter.log");
    
    initUDP();
    initSerials();
    buildRoutingTable();
    
    if(TRACE) printf("\tReady: Wait for commands.\n\n");
    while (TRUE) {
	if (DEBUG) printf("&response[%lu]\n",(long unsigned)&request);
	if ( receiveCommandFromClient_UDP(&request) < 0){
	    response = errorCommand();
	} else {
	    handleCommand(&request, &response);
	    if (TRACE) traceCommand(&request, &response);
	}
	sendResponseToClient_UDP(&response);	
    };

    if(TRACE) printf("\tSerial closed by peer.\n");
    return 0;
}

int handleCommand(command* request, command* response){
    int microcontrollerUidTmp;
    if ( commandIsValid(request) < 0){
	if (TRACE) printf("\t\tCommand is not valid.\n");
	*response = errorCommand();
	response->Argument[0] = -1;
	return -1;
    }
    if ( (microcontrollerUidTmp = routeCommand(request)) < 0 ){
	if (TRACE) printf("\t\tFailed routing command.\n");
	*response = errorCommand();
	response->Argument[0] = -2;
	return -2;
    }
    if ( sendCommandToMicrocontroller_Serial(request,microcontrollerUidTmp) < 0 ){
	if (TRACE) printf("\t\tFailed to send request command.\n");
	*response = errorCommand();
	response->Argument[0] = -3;
	return -3;
    }
    if ( receiveCommandFromMicrocontroller_Serial(response,microcontrollerUidTmp) < 0 ){
	if (TRACE) printf("\t\tFailed to receive response command.\n");
	*response = errorCommand();
	response->Argument[0] = -4;
	return -4;
    }
    return 0; 
}

int buildRoutingTable(){
    int i;
    int microcontrollerUidTmp;
    
    if (TRACE) printf ("\t Building routing table:\n");
    for (i=0; i<MAX_PIN_UID; i++) {
	microcontrollerUidTmp=i/NB_PINS_BY_BOARD;
	if ( microcontrollerIsAvailable(microcontrollerUidTmp) ){ 
	    pinUidToMicrocontrollerUidRoutingTable[i]=microcontrollerUidTmp;
	} else {
	    pinUidToMicrocontrollerUidRoutingTable[i]=-1;
	}
    }
    if (TRACE) {
        printf ("\t\tPin UID: |");
	for (i=0; i<MAX_PIN_UID; i++) {
	    if (pinUidToMicrocontrollerUidRoutingTable[i]>=0) printf("%2d|",i);
	}
	printf ("\n");
	printf ("\t\tMC  UID: |");
	for (i=0; i<MAX_PIN_UID; i++) {
	    if (pinUidToMicrocontrollerUidRoutingTable[i]>=0) printf("%2d|",pinUidToMicrocontrollerUidRoutingTable[i]);
	}
	printf ("\n\n");
    }
    return 0;
}

int routeCommand(command* cmd){
    unsigned char destinationPin;
    int destinationMicrocontrollerUid = -1;
    destinationPin = cmd->Argument[0];
    
    destinationMicrocontrollerUid=pinUidToMicrocontrollerUidRoutingTable[destinationPin];
    cmd->Argument[0] %= NB_PINS_BY_BOARD;
    if (TRACE)	printf ("\tRoute command:\tPin UID [%3d] -> Microcontroller UID [%3d]\n", destinationPin, destinationMicrocontrollerUid);
    
    // TODO Gérer les débordements de pin UID dans module ? 
    if (destinationMicrocontrollerUid == -1) {
	logPrint(ERROR_ARDUINO_NOT_CONNECTED,destinationMicrocontrollerUid);
    }

    /* // DEPRECATED */
    /* if (destinationMicrocontrollerUid == -2) { */
    /* 	logPrint(ERROR_PIN_NOT_ROUTABLE,(int)destinationPin); */
    /* } */
    
    return destinationMicrocontrollerUid;
}



