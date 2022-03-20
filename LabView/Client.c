/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/
#include "Client.h"
  
// Driver code
int main() {   
    int sockfd;
    command cmd;
    int exit;

    sockfd = initUDP();
	
    exit = getCommandCLI(&cmd);
    while (!exit) {
	send(sockfd, &cmd, sizeof(command), 0);
	exit = getCommandCLI(&cmd);
    };
     
    // close the descriptor
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
    struct sockaddr_in servaddr;
    int sockfd;
    
    // clear servaddr
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);
    servaddr.sin_family = AF_INET;
      
    // create datagram socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
      
    // connect to server
    if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
	printf("\n Error : Connect Failed \n");
	exit(0);
    }

    return sockfd;
}
