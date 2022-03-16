/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "SerialServer.h"

int main(){
    int fileDescriptorSerialPort1;
    // int fileDescriptorSerialPort2;
    int socketUDP;
    command request, response;
    int exit;

    fileDescriptorSerialPort1 = openSerial(SERIAL_FILE_1);
   //    fileDescriptorSerialPort2 = openSerial(SERIAL_FILE_2);    

    socketUDP = openUDP();
    exit = getCommandUDP(&request, socketUDP);
    //    exit = getCommandCLI(&request);
    while (!exit) {
	sendCommand(&request, fileDescriptorSerialPort1);
	receiveCommand(&response, fileDescriptorSerialPort1);
	handleCommand(&request, &response);
	exit = getCommandUDP(&request, socketUDP);
	// exit = getCommandCLI(&request);
	
    };

    close(fileDescriptorSerialPort1);
    if(LOG) printf("\tSerial closed by peer.\n");
    return 0;
}

int getCommandUDP(command* cmd, int socket){
    int nbBytesReceived;
    socklen_t addrlen;
    struct sockaddr_in infosSocketClient;
    char bufferReception[MAXBUF];


    if(LOG) printf("Get Command From UDP.\n");

    // man: (...) addrlen is a value-result argument (...)
    addrlen = sizeof(infosSocketClient); 
    
    // Recevoir données. ! Appel bloquant ! 
    if (
	(nbBytesReceived = recvfrom(socket,
				    bufferReception,
				    sizeof(command),
				    0,
				    (struct sockaddr *) &infosSocketClient,
				    &addrlen))
	== -1) {
	perror("recvfrom()");
    }
    return 0;
}

int openUDP(){
    struct sockaddr_in infosSocketServer;
    int socketReceptionUDP;

    if(LOG) printf("\tOpen UDP Port[%5d]\n",PORT);

    if ((socketReceptionUDP=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	perror("socket");
	return -1;
    }
    if(LOG) printf("\t\tSocket Creation Successful. Socket[%1d]\n",socketReceptionUDP);
    
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
    if(LOG) printf("\t\tSocket Bind Successful.\n");
    
    if(LOG) printf("\n");
    return socketReceptionUDP;

}

int handleCommand(command* request, command* response){
    if (response->Version!=CURRENT_VERSION){
	printf("\t\t\t[Erreur Version]\n");
	return -1;
    }
    switch (response->Function){
    case INVALID_CMD:
	printf("\t\t\t[Erreur Commande Invalide]\n");
	break;
    case GET_ANALOG:
	printf("\t\t\tA%1u[%3u]\n",request->Argument[0], *((short*)(response->Argument)));
	break;
    case SET_DIGITAL:
	printf("\t\t\tD%1u[%3u]\n",request->Argument[0], response->Argument[0]);
	break;
    default:
	printf("\t\t\t[<Erreur Fonction Inconnue]\n");
    }
    return 0;
}


int receiveCommand(command* cmd, int fdSerial){
    int n;
    n = rio_readn(fdSerial, cmd, sizeof(command));
    if (DEBUG) printf("Rcvd %d bytes: ",n);
    if (DEBUG) printCommand(cmd);
    return n;
}
int sendCommand(command* cmd, int fdSerial){
    int n;
    n = rio_writen(fdSerial, cmd, sizeof(command));
    if (DEBUG) printf("Send %d bytes: ",n);
    if (DEBUG) printCommand(cmd);
    return n;
}


int getCommandCLI(command* cmd){
    int exit = FALSE;
    int valide = FALSE;
    char choice;

    while(!valide) {
	printf("\n\t[0] Exit.\n");
	printf("\t[1] Read Analog.\n");
	printf("\t[2] Write Digital PWM.\n"); 
	scanf(" %c",&choice); // Attention l'espace de le % est important.
	switch (choice){
	case '0':
	    exit = TRUE;
	    valide = TRUE;
	    break;
	case '1':
	    commandGetAnalog(cmd);
	    valide = TRUE;
	    break;
	case '2':
	    commandSetDigitalPWM(cmd);
	    valide = TRUE;
	    break;
	default :
	    printf("\t[Commande Invalide]\n");
	    break;
	}
	scanf("%*[^\n]"); // Vide le buffer d'entree
    }
    return exit;
}

int commandSetDigitalPWM(command* cmd){
    char pin;
    unsigned char value;

    printf("\t[2]\tWhich Pin ? [0-5]\n");
    scanf(" %c",&pin);
    printf("\t[2]\tValue ? [0-255]\n");
    scanf(" %hhu",&value);
    printf("\t[2]\tD[%c]<-%3d\n",pin,value);
    cmd->Version  = CURRENT_VERSION;
    cmd->Function = SET_DIGITAL;
    cmd->Argument[0] = pin - '0';
    cmd->Argument[1] = value;
    //    printf("[%d]\n",(int)(cmd->Argument[0]));
    return 0;
    
}

int commandGetAnalog(command* cmd){
    char entry;
    
    cmd->Version  = CURRENT_VERSION;
    cmd->Function = GET_ANALOG;
    printf("\t[1]\tWhich Pin ? [0-5]\n");
    scanf(" %c",&entry);
    cmd->Argument[0] = entry - '0';
    //    printf("[%d]\n",(int)(cmd->Argument[0]));
    
    return 0;
}

int openSerial(char* serialFile){
    int fileDescriptor;
    struct termios term;

    if(LOG) printf("\n\tOpen Serial [%s]\n",serialFile);
    while ( (fileDescriptor = open(serialFile, O_RDWR,0))<0) {
	if(LOG) printf("\t\tFailed. Returned FileDescriptor [%d].\n", fileDescriptor);
	if(LOG) printf("\t\t[%s]\n", strerror(errno));
	if(LOG) printf("\t\tWill try again in %d seconds.\n",TEMPO_TRY_AGAIN_OPEN_SERIAL);
	sleep(TEMPO_TRY_AGAIN_OPEN_SERIAL);
    }
    if(LOG) printf("\tOpen Serial successful. FileDescriptor[%d]\n", fileDescriptor);

    // Met en place les parametres série:
    // Serial.begin(115200,SERIAL_8N1);
    if(tcgetattr(fileDescriptor, &term) != 0) {
	printf("Error %i from tcgetattr: %s\n", errno, strerror(errno));
    }

    term.c_lflag &= ~ECHO;
    cfsetspeed(&term, B115200);
    term.c_cflag &= ~PARENB;
    term.c_cflag &= ~CSTOPB;
    term.c_cflag &= ~CSIZE;
    term.c_cflag |= CS8;
    term.c_cflag &= ~CRTSCTS; // Disable RTS/CTS
    term.c_cflag |= CREAD | CLOCAL;
    // Désactive mode CANONIQUE, cad n'attend le retour a la ligne
    term.c_lflag &= ~ICANON; 
    term.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    // Disable any special handling of received bytes
    term.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); 
    // Prevent special interpretation of output bytes (e.g. newline chars)
    term.c_oflag &= ~OPOST;
    // Prevent conversion of newline to carriage return/line feed
    term.c_oflag &= ~ONLCR; 

    if (tcsetattr(fileDescriptor, TCSANOW, &term) != 0) {
	printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
    if(LOG) printf("\n");

    
    return fileDescriptor;
  
}


void printCommand(command* cmd){
    printf(" CMD: V[%1d] F[%1d] A0[%3d] A1[%3d]\n",cmd->Version, cmd->Function, cmd->Argument[0], cmd->Argument[1]);
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
