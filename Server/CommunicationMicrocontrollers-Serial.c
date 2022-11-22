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

char devicesNames[NB_MAX_SERIAL_DEVICES][SIZE_MAX_FILE_NAME];

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
    if(TRACE) printf("\t\tOpen Serial successful.\n");
    if(TRACE) printf("\t\tFileDescriptor [%d]\n", fileDescriptor);

    while (setSerialParameters(fileDescriptor)!=0){
	printf("Error setting Serial Parameters.\n");
	sleep(TEMPO_TRY_AGAIN_OPEN_SERIAL);
    }

    if(TRACE) printf("\n");

    // TODO ce délai semble nécessaire pour laisser le temps à la ligne de demarrer
    // correctement, sinon le premier write est perdu...
    sleep(3);
    return fileDescriptor;
}


int receiveCommandFromMicrocontroller_Serial(command* cmd, int fdSerial){
    int n;
    if (DEBUG) printf("Start rcv %lu bytes:\n", (long unsigned)sizeof(command));
    n = rio_readn(fdSerial, cmd, sizeof(command));
    if (DEBUG) printf("Rcvd %d bytes:\t\t",n);
    if (DEBUG) printCommand(cmd);
    return n;
}


int sendCommandToMicrocontroller_Serial(command* cmd, int fdSerial){
    int n;
    n = rio_writen(fdSerial, cmd, sizeof(command));
    if (DEBUG) printf("Send %d bytes:\t",n);
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

    if (TRACE) printf("\tIdentifying Micro-Controller over FileDescriptor [%1u].\n",fdTemp);
    request = requestUidCommand();
    sendCommandToMicrocontroller_Serial(&request, fdTemp);
    receiveCommandFromMicrocontroller_Serial(&response, fdTemp);
    uidArduino = response.Argument[0];
    // Filtre resultat par sureté: id affectés 0 et 1.
    if (uidArduino>1) {
	if (TRACE) printf("UID Arduino non reconnu.\n");
	return -1;
    }
    fd[uidArduino] = fdTemp;
    if (TRACE) printf("\t\tArduinoID[%1u]\n",uidArduino);

    return 0;
}

void printDevicesNamesTable(){
    int i;
    printf("\tDetected Devices:\n");
    for (i=0; i<nbDevicesFound;i++){
	printf("\t\t[%s]\n",((char*)&devicesNames[i]));
    }
}

int detectAvailableDevices(char* basename, char* repertory){
    DIR *targetRepertoryStream;
    struct dirent *fileInfosStruct;

    /* printf("devicesNames--------[%u]---\n",((unsigned int)devicesNames)); */
    /* printf("devicesNames[0]-----[%u]---\n",((unsigned int)devicesNames[0])); */
    /* printf("&devicesNames[0][0]-[%u]---\n",((unsigned int)&devicesNames[0][0])); */
    /* printf("&devicesNames[0]----[%u]---\n",((unsigned int)&devicesNames[0])); */
    /* printf("&devicesNames-------[%u]---\n",((unsigned int)&devicesNames)); */
    
    targetRepertoryStream = opendir(repertory);
    if (targetRepertoryStream == NULL){
	if(TRACE) printf("\t\tFailed. Returned NULL.\n");
	if(TRACE) printf("\t\t[%s]\n", strerror(errno));
	return -1;
    } else { // targetRepertoryStream != NULL
	while ((fileInfosStruct = readdir(targetRepertoryStream)) != NULL) {
	    if (strncmp(basename,fileInfosStruct->d_name,strlen(basename))==0){
		// printf("[%s]\n",fileInfosStruct->d_name);
 		strcpy(&(devicesNames[nbDevicesFound][0]),repertory);
		strcat((char*)&devicesNames[nbDevicesFound],fileInfosStruct->d_name);
		// printf("---[%s]---\n",((char*)&devicesNames[nbDevicesFound]));
		nbDevicesFound++;
	    }
	}
	if(TRACE) printDevicesNamesTable();
	closedir(targetRepertoryStream);
	return 0;
    }
}

int initSerials(int* fd){
    int fdTemp;
    int i;
    detectAvailableDevices(devicesBasename,targetRepertory);
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

