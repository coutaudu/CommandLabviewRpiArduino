/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/
#include "Client.h"

struct sockaddr_in SerialServerAddress;

// Driver code
int main() {   
    int sockfd;
    command cmd;
    int exit;
    socklen_t len;
    
    sockfd = initUDP();
	
    exit = getCommandCLI(&cmd);
    while (!exit) {
	sendto(sockfd, &cmd, sizeof(command), 0,(const struct sockaddr*)&SerialServerAddress, sizeof(SerialServerAddress));
	recvfrom(sockfd, &cmd, sizeof(command), 0, (struct sockaddr*)&SerialServerAddress,&len);
	printCommand(&cmd);
	exit = getCommandCLI(&cmd);
    };
     
    close(sockfd);
}


int getCommandCLI(command* cmd){
    int exit = FALSE;
    int valide = FALSE;
    char choice;

    while(!valide) {
	printf("\n\t[0] Exit.\n");
	printf("\t[1] Read Analog.\n");
	printf("\t[2] Write Digital PWM.\n");
	printf("\t[3] Get UID.\n"); 
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
	case '3':
	    commandGetUID(cmd);
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


int commandGetUID(command* cmd){
    cmd->Version  = CURRENT_VERSION;
    cmd->Function = GET_UID;
    cmd->Argument[0] = 0;
    cmd->Argument[1] = 0;
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


int initUDP(){
    // struct sockaddr_in SerialServerAddress;
    //    struct sockaddr_in ClientAddr;
    int sockfd;
    
    // clear SerialServerAddress
    bzero(&SerialServerAddress, sizeof(SerialServerAddress));
    SerialServerAddress.sin_addr.s_addr = inet_addr("192.168.1.201");
    //    SerialServerAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    SerialServerAddress.sin_port = htons(PORT);
    SerialServerAddress.sin_family = AF_INET;

    // create datagram socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	perror("socket");
	return -1;
    }


    return sockfd;
}


void printCommand(command* cmd){
    printf(" CMD: V[%1d] F[%1d] A0[%3d] A1[%3d]\n",cmd->Version, cmd->Function, cmd->Argument[0], cmd->Argument[1]);
}
