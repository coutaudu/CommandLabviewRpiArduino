/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "CommunicationMicrocontrollers-Serial.h"

// TODO -> conf
char* targetRepertory="/dev/";
char* devicesBasename="ttyACM";
int nbDevicesFound=0;

char devicesNames[NB_MAX_SERIAL_DEVICES][512];


int openSerial(char* serialFile){
    int fileDescriptor = -1;
    int tryAgainCounter = 0;
    
    if(TRACE) printf("\n\tOpen Serial [%s]\n",serialFile);
    while ( (fileDescriptor = open(serialFile, O_RDWR, 0))<0
	    && tryAgainCounter < NB_MAX_TRY_AGAIN_OPEN_SERIAL) {
	if(TRACE) printf("\t\tFailed. Returned FileDescriptor [%d].\n", fileDescriptor);
	if(TRACE) printf("\t\t[%s]\n", strerror(errno));
	if(TRACE) printf("\t\tWill try again in %d seconds.\n",TEMPO_TRY_AGAIN_OPEN_SERIAL);
	tryAgainCounter++;
	sleep(TEMPO_TRY_AGAIN_OPEN_SERIAL);
    }
    if(TRACE) printf("\t\tOpen Serial successful. FileDescriptor[%d]\n", fileDescriptor);

    while (setSerialParameters(fileDescriptor)!=0){
	printf("Error setting Serial Parameters.\n");
	sleep(TEMPO_TRY_AGAIN_OPEN_SERIAL);
    }
	   
    // TODO ce délai semble nécessaire pour laisser le temps à la ligne de demarrer
    // correctement, sinon le premier write est perdu...
    sleep(3);
    return fileDescriptor;
}


int receiveCommandSerial(command* cmd, int fdSerial){
    int n;
    if (DEBUG) printf("Start rcv %lu bytes:\n", (long unsigned)sizeof(command));
    n = rio_readn(fdSerial, cmd, sizeof(command));
    if (DEBUG) printf("Rcvd %d bytes:\t\t",n);
    if (DEBUG) printCommand(cmd);
    return n;
}


int sendCommandSerial(command* cmd, int fdSerial){
    int n;
    n = rio_writen(fdSerial, cmd, sizeof(command));
    if (DEBUG) printf("Send %d bytes:\t\t",n);
    if (DEBUG) printCommand(cmd);
    return n;
}


int setSerialParameters(int fd){
    struct termios term;
    if(tcgetattr(fd, &term) != 0) {
	printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
	return -1;
    }
    if (cfsetspeed(&term, B115200) != 0 ){
	printf("Error %i from cfsetspeed: %s\n", errno, strerror(errno));
	return -1;
    }
    // Explications disponibles ici: https://blog.mbedded.ninja/programming/operating-systems/linux/linux-serial-ports-using-c-cpp/
    term.c_lflag &= ~ ( ECHO | ECHONL | ISIG | IEXTEN | ICANON );
    term.c_cflag &= ~ ( PARENB | CSTOPB | CSIZE | CRTSCTS );
    term.c_cflag |= (CS8 | CREAD | CLOCAL) ;
    term.c_iflag &= ~(IXON|IXOFF|IXANY|IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);
    term.c_oflag &= ~ ( OPOST | ONLCR ); 
    term.c_cc[VTIME] = 10;
    term.c_cc[VMIN] = 0;
    /****************************************************/
    // cfmakeraw(&term); Serai potentiellemnt suffisant
    /****************************************************/
    if (tcsetattr(fd, TCSANOW, &term) != 0) {
	printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
	return -1;
    }
    return 0;
}


int identifyArduinoOnSerial(int fdTemp, int* fd){
    command request;
    command response;
    unsigned char uidArduino;

    request = requestUidCommand();
    sendCommandSerial(&request, fdTemp);
    receiveCommandSerial(&response, fdTemp);
    uidArduino = response.Argument[0];
    // Filtre resultat par sureté: id affectés 0 et 1.
    if (uidArduino>1) {
	if (TRACE) printf("UID Arduino non reconnu.\n");
	return -1;
    }
    fd[uidArduino] = fdTemp;
    if (TRACE) printf("\t\tArduino[%1u] <-> fd<%1u>\n",uidArduino,fd[uidArduino]);

    return 0;
}


int detectAvailableDevices(char** devNames, char* basename, char* repertory){
    DIR *targetRepertoryStream;
    struct dirent *fileInfosStruct;

    /* printf("devNames--------[%u]---\n",((unsigned int)devNames)); */
    /* printf("devNames[0]-----[%u]---\n",((unsigned int)devNames[0])); */
    /* printf("&devNames[0][9]-[%u]---\n",((unsigned int)&devNames[0][9])); */
    /* printf("&devNames[0]----[%u]---\n",((unsigned int)&devNames[1])); */
    /* printf("&devNames-------[%u]---\n",((unsigned int)&devNames)); */
    
    targetRepertoryStream = opendir(repertory);
    if (targetRepertoryStream == NULL){
	if(TRACE) printf("\t\tFailed. Returned NULL.\n");
	if(TRACE) printf("\t\t[%s]\n", strerror(errno));
	return -1;
    } else { // targetRepertoryStream != NULL
	while ((fileInfosStruct = readdir(targetRepertoryStream)) != NULL) {
	    if (strncmp(basename,fileInfosStruct->d_name,strlen(basename))==0){
		
		strcpy((char*)&devNames[nbDevicesFound],repertory);
		strcat((char*)&devNames[nbDevicesFound],fileInfosStruct->d_name);
		printf("---[%s]---\n",((char*)&devNames[nbDevicesFound]));
		nbDevicesFound++;
	    }
	}
	closedir(targetRepertoryStream);
	return 0;
    }
}

int initSerials(int* fd){
    int fdTemp;
    int i;
    detectAvailableDevices((char**)devicesNames,devicesBasename,targetRepertory);
    /* printf("devicesNames--------[%u]---\n",((unsigned int)devicesNames)); */
    /* printf("devicesNames[0]-----[%u]---\n",((unsigned int)devicesNames[0])); */
    /* printf("&devicesNames[0][1]-[%u]---\n",((unsigned int)&devicesNames[0][1])); */
    /* printf("&devicesNames[0]----[%u]---\n",((unsigned int)&devicesNames[1])); */
    /* printf("&devicesNames-------[%u]---\n",((unsigned int)&devicesNames)); */

    for (i=0; i<nbDevicesFound; i++){
	   fdTemp = openSerial(devicesNames[0]);
	   identifyArduinoOnSerial(fdTemp,fd);
    }

    if(TRACE) printf("\n");
    return 0;    
}

int transmitCommandSerial(command* request, command* response, int* fdSerials){
    int fdTmp;
    if ( commandIsValid(request) < 0){
	printf("Command is not valid.\n");
	return -1;
    }
    if ( (fdTmp = routeCommand(request, fdSerials)) < 0 ){
	printf("Pin is not known.\n");
	return -1;
    }
    if ( sendCommandSerial(request, fdTmp) < 0 ){
	printf("Failed to send request command.\n");
	return -1;
    }
    if ( receiveCommandSerial(response, fdTmp) < 0 ){
	printf("Failed to send response command.\n");
	return -1;
    }
    return 0; 
}
