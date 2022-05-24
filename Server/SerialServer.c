/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "SerialServer.h"

int main(){
    int fdSerials[2];
    int socketUDP;
    command request, response;
    struct sockaddr_in infosClientUDP;


    initLog("/tmp/SerialServer.log");
    
    socketUDP = initUDP();
    initSerials(fdSerials);

    if(TRACE) printf("\tReady: Wait for commands.\n");
    while (TRUE) {
	if ( getCommandUDP(&request, socketUDP, &infosClientUDP) < 0){
	    if (TRACE) printf(" Received command is invalid.\n");
	    response = errorCommand();
	} else {
	    transmitCommandSerial(&request, &response, fdSerials );
	    if (TRACE) logCommand(&request, &response);
	}
	sendResponseUDP(&response, socketUDP, &infosClientUDP);	
    };

    close(fdSerials[0]);
    close(fdSerials[1]);
    if(TRACE) printf("\tSerial closed by peer.\n");
    return 0;
}

int commandIsValid( command* cmd){
    int retval;
    printCommand(cmd);
    if ( cmd->Version == CURRENT_VERSION ) {
	 if ( cmd->Function == GET_ANALOG || cmd->Function == SET_DIGITAL ) {
	     retval = 0;
	 }else{
	     retval = -1;
	 }
    }else{
	retval = -2;
    }
    return retval;
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
	    time_t t = time(NULL);
	    dprintf(LOG_FILE_FD,"[%s]\t[ERROR] Arduino [%d] not connected.\n", ctime(&t), destinationArduinoUid);
    }
    if (retval == -2) {
	    time_t t = time(NULL);
	    dprintf(LOG_FILE_FD,"[%s]\t[ERROR] Pin [%d] not routable.\n", ctime(&t), destinationPin);
    }
    
    return retval;
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

int initSerials(int* fd){
    int fdTemp; 
    
    fdTemp = openSerial(SERIAL_FILE_0);
    identifyArduinoOnSerial(fdTemp,fd);
    fdTemp = openSerial(SERIAL_FILE_1);
    identifyArduinoOnSerial(fdTemp,fd);

    if(TRACE) printf("\n");
    return 0;    
}

int sendResponseUDP(command* cmd, int socketfd, struct sockaddr_in* infosClientUDP){
    if (sendto(socketfd, cmd, sizeof(cmd), 0,(const struct sockaddr*)infosClientUDP, sizeof(*infosClientUDP))<0){
	perror("sento");
    }
    return 0;
}

int getCommandUDP(command* cmd, int socket, struct sockaddr_in* infosClientUDP){
    int nbBytesReceived;
    socklen_t addrlen;
    
    if(DEBUG) printf("Get Command From UDP:\t");
    if(DEBUG) fflush(stdout);

    // man: (...) addrlen is a value-result argument (...)
    addrlen = sizeof(*infosClientUDP); 
    
    // Recevoir données. ! Appel bloquant ! 
    if ((nbBytesReceived = recvfrom(socket, cmd, sizeof(cmd), 0, (struct sockaddr *)infosClientUDP, &addrlen)) == -1) {
	perror("recvfrom()");
	return -2;
    }

    if (DEBUG) {
	struct in_addr ipAddr = ((struct sockaddr_in*)infosClientUDP)->sin_addr;
	char str[INET_ADDRSTRLEN];
	printf("IPsrc[%s]:%d\n", inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN ),((struct sockaddr_in*)infosClientUDP)->sin_port);
    }

    //    if(TRACE) printf("Received Command From UDP.\n");
    if ( cmd->Version != CURRENT_VERSION ){
	return -1;
    }

    return 0;
}

int initUDP(){
    struct sockaddr_in infosSocketServer;
    int socketReceptionUDP;

    if(TRACE) printf("\tOpen UDP Port[%5d]\n",PORT);
    if ((socketReceptionUDP=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	perror("socket");
	return -1;
    }
    if(TRACE) printf("\t\tSocket Creation Successful. Socket[%1d]\n",socketReceptionUDP);
    
    // Vide les structures.
    memset((char *) &infosSocketServer, 0, sizeof(infosSocketServer));	
    infosSocketServer.sin_family = AF_INET;
    infosSocketServer.sin_port = htons(PORT);
    infosSocketServer.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(socketReceptionUDP , (struct sockaddr*)&infosSocketServer, sizeof(infosSocketServer) ) == -1) {
	perror("bind");
	return -1;
    }
    if(TRACE) printf("\t\tSocket Bind Successful.\n\n");
    return socketReceptionUDP;
}


int logCommand(command* request, command* response){
    printf("\t");
    printCommand(request);
    printf("\t->\t");
    if (response->Version!=CURRENT_VERSION){
	printf("[Erreur Version]\n");
	return -1;
    }
    switch (response->Function){
    case INVALID_CMD:
	printf("[Erreur Commande Invalide]\n");
	return -1;
	break;
    case GET_ANALOG:
	printf("A%1u[%3u]\n",request->Argument[0], *((short*)(response->Argument)));
	break;
    case SET_DIGITAL:
	printf("D%1u[%3u]\n",request->Argument[0], response->Argument[0]);
	break;
    case GET_UID:
	printf("UID[%1u]\n",response->Argument[0]);
	break;
    default:
	printf("[Erreur Fonction Inconnue]\n");
	return -1;
    }
    return 0;
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

int initLog(char* filename){

    
    int fileDescriptor = -1;
    while ( (fileDescriptor = open(filename, O_RDWR | O_APPEND | O_CREAT , 00644))<0) {
	if(TRACE) printf("\t\tOpen[%s] Failed. Returned FileDescriptor [%d].\n", filename, fileDescriptor);
	if(TRACE) printf("\t\t[%s]\n", strerror(errno));
	sleep (1);
    }
    LOG_FILE_FD = fileDescriptor;

    time_t t = time(NULL);
    dprintf(LOG_FILE_FD,"[%s]\tSerialServer start.\n",ctime(&t));

    return 0;
}

    
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


void printCommand(command* cmd){
    printf("CMD: V[%1d] F[%1d] A0[%3d] A1[%3d]",cmd->Version, cmd->Function, cmd->Argument[0], cmd->Argument[1]);
}

// Assumes little endian
void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}

command requestUidCommand(){
    command cmd;
    cmd.Version  = CURRENT_VERSION;
    cmd.Function = GET_UID;
    cmd.Argument[0] = 0;
    cmd.Argument[1] = 0;
    return cmd;
}

command errorCommand(){
    command cmd;
    cmd.Version  = CURRENT_VERSION;
    cmd.Function = 0;
    cmd.Argument[0] = 0;
    cmd.Argument[1] = 0;

    return cmd;
}
