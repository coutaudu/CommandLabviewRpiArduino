/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "CommunicationMicrocontrollers-Serial.h"

/***********/
/* PRIVATE */
/* HEADER  */
/***********/

// Ouvre et configure la ligne serie
int openSerial(char* serialFile);

// Cherche la disponibilité des devices.
// Par exemple au format /dev/ttyACM*
int detectAvailableDevices(char* basename, char* repertory);

// Prepare les parametres de communications serie.
// fd est le descripteur de fichier vers le pseudo fichier tty
// Il doit être ouvert.
int setSerialParameters(int fd);

// Interroge l'arduino au bout du fil pour obtenir son UID
// (NB: Le UID doit etre inscrit manuellement de l'EEPROM)
// Si le arduino est bien reconnu, son fileDescritor est inscrit
// dans le table fdSerials en indice=UID.
int identifyMicrocontrollerUid(int fdTemp);

// Lit la commande dans le File Descriptor.
int readCommand(command* cmd, int fdSerial);

// Ecrit la commande dans le File Descriptor.
int writeCommand(command* cmd, int fd);


/**********************/
/* Macro              */
/**********************/
// Nom de fichier typique: "/dev/ttyACM1"=12+1 char. 
#define SIZE_MAX_FILE_NAME 32
#define TEMPO_TRY_AGAIN_OPEN_SERIAL 3
#define TEMPO_SET_SERIAL_PARAMETERS 3
#define NB_MAX_TRY_AGAIN_OPEN_SERIAL 3
#define NB_MAX_MICROCONTROLLER 4
// 255 car UID des boards arduino ecrit dans l'eeprom sur 8 bits.
#define MAX_MICROCONTROLLER_UID 255


/**********************/
/* Variables globales */
/**********************/
// TODO -> fichier .conf ?
char* targetRepertory="/dev/";
char* devicesBasename="ttyACM";
int nbDevicesFound=0;
char devicesNames[NB_MAX_MICROCONTROLLER][SIZE_MAX_FILE_NAME];
int microcontrollerFileDescriptorsTable[MAX_MICROCONTROLLER_UID];
char* microcontrollerUidFilenameLookupTable[MAX_MICROCONTROLLER_UID];

/**************/
/* PUBLIC     */
/* FUNCTIONS  */
/**************/

int initSerials(){
    DIR *targetRepertoryStream;
    struct dirent *fileInfosStruct;
    int strlenRepertory;
    int strlenBasename;
    int fd;
    int i;
    int microcontrollerUid;

    
    if (TRACE) printf("\tInitialize serial communication with microcontrollers:\n");

    // Initialise à -1 pour distinguer les file descriptor non-utilisé/invalides
    for (i=0; i<MAX_MICROCONTROLLER_UID; i++){
	microcontrollerFileDescriptorsTable[i]=-1;
    }
    
    // Parcours la liste des pseudos fichiers correspondant à des arduino branchés en USB
    // dans le répertoire targetRepertory (/dev/)
    // nom commencant par devicesBasename (ttyACM)
    strlenRepertory = strlen(targetRepertory);
    strlenBasename = strlen(devicesBasename);

    targetRepertoryStream = opendir(targetRepertory);
    if (targetRepertoryStream == NULL){
	if(TRACE) printf("\t\tFailed. Returned NULL.\n");
	if(TRACE) printf("\t\t[%s]\n", strerror(errno));
	return -1;
    } else { // targetRepertoryStream != NULL
	while ((fileInfosStruct = readdir(targetRepertoryStream)) != NULL) {
	    if ((strlen(fileInfosStruct->d_name)+strlenRepertory)>=SIZE_MAX_FILE_NAME){
		if(TRACE) printf("[ERR] Error: [%s] is too long for SIZE_MAX_FILE_NAME[%d].\n",fileInfosStruct->d_name,SIZE_MAX_FILE_NAME);
	    }
	    else if (strncmp(devicesBasename,fileInfosStruct->d_name,strlenBasename)==0){
		// TODO Verifier si deja present ou non
		// TODO inscrire dans slot vide de devicesNames
 		strcpy(&(devicesNames[nbDevicesFound][0]),targetRepertory);
		strcat((char*)&devicesNames[nbDevicesFound],fileInfosStruct->d_name);
		if (TRACE) printf("\t\tFound [%s]\n",((char*)&devicesNames[nbDevicesFound]));
		/*****/
		fd = openSerial(devicesNames[nbDevicesFound]);
		microcontrollerUid = identifyMicrocontrollerUid(fd);
		microcontrollerUidFilenameLookupTable[microcontrollerUid] = devicesNames[nbDevicesFound];
		if (TRACE) printf("\n");
		/*****/
		nbDevicesFound++;
	    }
	}
	closedir(targetRepertoryStream);
    }

    ////////////////////////////////////
    /* for (i=0; i<nbDevicesFound; i++){ */
    /* 	fd = openSerial(devicesNames[i]); */
    /* 	microcontrollerUid = identifyMicrocontrollerUid(fd); */
    /* 	microcontrollerUidFilenameLookupTable[microcontrollerUid] = devicesNames[i]; */
    /* } */

    if (TRACE) {
	printf("\n\tMicrocontroller UID to Filename Lookup Table:\n");
	for (i=0; i<MAX_MICROCONTROLLER_UID; i++){
	    fd = microcontrollerFileDescriptorsTable[i];
	    if ( fd != -1 ) {	    
		printf("\t\tMC UID: [%d] <-> Filename: [%s]\n",i,microcontrollerUidFilenameLookupTable[i]);
	    }
	}
	printf("\n");
    }

    return 0;    
}


int receiveCommandFromMicrocontroller_Serial(command* cmd, int microcontrollerUid){
    int n;
    int fd = microcontrollerFileDescriptorsTable[microcontrollerUid];
    n = readCommand(cmd,fd);
    return n;
}


int sendCommandToMicrocontroller_Serial(command* cmd, int microcontrollerUid){
    int n;
    int fd = microcontrollerFileDescriptorsTable[microcontrollerUid];
    n = writeCommand(cmd,fd);
    if ( n==-1 ) {
	if( errno==EIO ){
	    if (TRACE) printf ("\tMicrocontroller[%d]: %s (fd[%d]) has been unplugged.",microcontrollerUid,microcontrollerUidFilenameLookupTable[microcontrollerUid],fd) ;
	    // Close fd
	    if(close(fd) != 0) {
		if (TRACE) if (TRACE) printf("[ERR]  %i from close(%d): %s\n", fd, errno, strerror(errno));
	    } else {
		// Remove from microcontrollerFileDescriptorsTable
		microcontrollerFileDescriptorsTable[microcontrollerUid] = -1;
	    }
	    // Clear devicesNames entry
	    microcontrollerUidFilenameLookupTable[microcontrollerUid][0]='\0';
	    // Remove from microcontrollerUidFilenameLookupTable
	    microcontrollerUidFilenameLookupTable[microcontrollerUid] = NULL;
	} 
	if (DEBUG) if (TRACE) printf("[ERR]  %i from syscall write: %s\n", errno, strerror(errno));

    }
    return n;
}


int microcontrollerIsAvailable(int microcontrollerUid){
    return ( microcontrollerFileDescriptorsTable[microcontrollerUid] != -1 );
}

int closeSerials(){
    int i;
    int fd;
    int retval = 0;
    
    for (i=0; i<MAX_MICROCONTROLLER_UID; i++){
	fd=microcontrollerFileDescriptorsTable[i];
	if ( fd != -1 ) {
	    if(close(fd) != 0) {
		if (TRACE) printf("[ERR] Error %i from close(%d): %s\n", fd, errno, strerror(errno));
		retval = -1;
	    } else {
		microcontrollerFileDescriptorsTable[i] = -1;
	    }
	}
    }
    return retval;
}

/**************/
/* PRIVATE    */
/* FUNCTIONS  */
/**************/

int openSerial(char* serialFile){
    int fd = -1;
    int tryAgainCounter = 0;
    
    if(TRACE) printf("\t\tOpen Serial [%s]\n",serialFile);
    while ( (fd = open(serialFile, O_RDWR, 0))<0
	    && tryAgainCounter < NB_MAX_TRY_AGAIN_OPEN_SERIAL) {
	if(TRACE) printf("\t\t\tFailed. Returned FileDescriptor [%d].\n", fd);
	if(TRACE) printf("\t\t\t[%s]\n", strerror(errno));
	if(TRACE) printf("\t\t\tWill try again in %d seconds.\n",TEMPO_TRY_AGAIN_OPEN_SERIAL);
	tryAgainCounter++;
	sleep(TEMPO_TRY_AGAIN_OPEN_SERIAL);
    }
    if(TRACE) printf("\t\t\tOpen Serial successful.\n");
    if(TRACE) printf("\t\t\tFileDescriptor [%d]\n", fd);

    while (setSerialParameters(fd)!=0){
	if (TRACE) printf("[ERR] Error setting Serial Parameters.\n");
	sleep(TEMPO_TRY_AGAIN_OPEN_SERIAL);
    }

    //    if(TRACE) printf("\n");
    // TODO ce délai semble nécessaire pour laisser le temps à la ligne de demarrer
    // correctement, sinon le premier write est perdu...
    sleep(TEMPO_SET_SERIAL_PARAMETERS);
    return fd;
}


int identifyMicrocontrollerUid(int fdTemp){
    command request;
    command response;
    unsigned char microcontrollerUid;

    if (TRACE) printf("\t\t\tIdentifying... ");
    if (TRACE) fflush(stdout);
    request = requestUidCommand();
    writeCommand(&request, fdTemp);
    readCommand(&response, fdTemp);
    microcontrollerUid = response.Argument[0];
    microcontrollerFileDescriptorsTable[microcontrollerUid] = fdTemp;
    if (TRACE) printf("Microcontroller UID [%1u]\n",microcontrollerUid);

    return microcontrollerUid;
}

int readCommand(command* cmd, int fd){
    int n;
    if (DEBUG) printf("Start rcv  %lu bytes from fd[%d]:\n", (long unsigned)sizeof(command),fd);
    n = rio_readn(fd, cmd, sizeof(command));
    if (DEBUG) printf("Rcvd %d bytes:\t\t",n);
    if (DEBUG) printCommand(cmd);
    if (DEBUG) printf("\n");
    return n;
}

int writeCommand(command* cmd, int fd){
    int n;
    if (DEBUG) printf("Start send %lu bytes to fd[%d]:\n", (long unsigned)sizeof(command),fd);
    n = rio_writen(fd, cmd, sizeof(command));
    if (DEBUG) printf("Send %d bytes:\t",n);
    if (DEBUG) printCommand(cmd);
    if (DEBUG) printf("\n");
    return n;
}


int setSerialParameters(int fd){
    struct termios term;

    if(TRACE) printf("\t\t\tSet serial parameters FD[%2d]...", fd);

    if(tcgetattr(fd, &term) != 0) {
	if (TRACE) printf("\n[ERR] Error %i from tcgetattr: %s\n", errno, strerror(errno));
	return -1;
    }
    // TODO param en .conf ?
    if (cfsetspeed(&term, B115200) != 0 ){
	if (TRACE) printf("\n[ERR]  %i from cfsetspeed: %s\n", errno, strerror(errno));
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
	if (TRACE) printf("\n[ERR]  %i from tcsetattr: %s\n", errno, strerror(errno));
	return -1;
    }
    if(TRACE) printf(" OK.\n");
    return 0;
}




