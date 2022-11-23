/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "Logs.h"

int LOG_FILE_FD;

int traceCommand(command* request, command* response){
    int retval=0;
    
    printf("\t");
    if (response->Version!=CURRENT_VERSION){
	printf("[Erreur Version]\n");
	retval=-1;
    } else {
	switch (response->Function){
	case INVALID_CMD:
	    printf("[Erreur Commande Invalide:%2hhu:%hhd]\n",response->Function,response->Argument[0]);
	    retval=-1;
	    break;
	case GET_ANALOG:
	    printf("GET_ANALOG [%1u]:%4u\n",request->Argument[0], *((short*)(response->Argument)));
	    break;
	case SET_DIGITAL:
	    printf("SET_DIGITAL[%1u]:%4u\n",request->Argument[0], response->Argument[1]);
	    break;
	case GET_UID:
	    printf("GET_UID    [%1u]\n",response->Argument[0]);
	    break;
	default:
	    printf("[Erreur Fonction Inconnue:%hhu]\n",response->Function);
	    retval=-1;
	}
    }
    printf("\t\tRequest:  ");
    printCommand(request);
    printf("\n\t\tResponse: ");
    printCommand(response);
    printf("\n\n");
    return retval;
}


int logInit(char* filename){
    int fileDescriptor = -1;
    while ( (fileDescriptor = open(filename, O_RDWR | O_APPEND | O_CREAT , 00644))<0) {
	if(TRACE) printf("\t\tOpen[%s] Failed. Returned FileDescriptor [%d].\n", filename, fileDescriptor);
	if(TRACE) printf("\t\t[%s]\n", strerror(errno));
	sleep (1);
    }
    LOG_FILE_FD = fileDescriptor;

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    dprintf(LOG_FILE_FD,"[%.19s]\tTemperatureControlRouter start.\n",asctime(tm));

    return 0;
}

int logPrint(int errorCode, int arg){
    time_t t = time(NULL);
    switch ( errorCode ) {
    case ERROR_ARDUINO_NOT_CONNECTED:
	dprintf(LOG_FILE_FD,"[%.19s]\t[ERROR] Arduino [%d] not connected.\n", ctime(&t), arg);
	break;
    case ERROR_PIN_NOT_ROUTABLE:
	dprintf(LOG_FILE_FD,"[%.19s]\t[ERROR] Pin [%d] not routable.\n", ctime(&t), arg);
	break;
    default:
	dprintf(LOG_FILE_FD,"[%.19s]\t[ERROR] UNKNOWN [%d].\n", ctime(&t), arg);
	return -1;
    }
    return 0;
}
