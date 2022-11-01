/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "CommunicationClients-UDP.h"

int socketUDP;
struct sockaddr_in infosClientUDP;

int initUDP(){
    struct sockaddr_in infosSocketServer;

    if(TRACE) printf("\tOpen UDP Port[%5d]\n",PORT);
    if ((socketUDP=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
	perror("socket");
	return -1;
    }
    if(TRACE) printf("\t\tSocket Creation Successful. Socket[%1d]\n",socketUDP);
    
    // Vide les structures.
    memset((char *) &infosSocketServer, 0, sizeof(infosSocketServer));	
    infosSocketServer.sin_family = AF_INET;
    infosSocketServer.sin_port = htons(PORT);
    infosSocketServer.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(socketUDP , (struct sockaddr*)&infosSocketServer, sizeof(infosSocketServer) ) == -1) {
	perror("bind");
	return -1;
    }
    if(TRACE) printf("\t\tSocket Bind Successful.\n\n");
    return socketUDP;
}


int sendResponseToClient_UDP(command* cmd){
    if (sendto(socketUDP, cmd, sizeof(cmd), 0,(const struct sockaddr*)&infosClientUDP, sizeof(infosClientUDP))<0){
	perror("sento");
    }
    return 0;
}

int receiveCommandFromClient_UDP(command* cmd){
    int nbBytesReceived;
    socklen_t addrlen;
    
    if(DEBUG) printf("Get Command From UDP:\t");
    if(DEBUG) fflush(stdout);

    // man: (...) addrlen is a value-result argument (...)
    addrlen = sizeof(infosClientUDP); 
    
    // Recevoir données. ! Appel bloquant ! 
    if ((nbBytesReceived = recvfrom(socketUDP, cmd, sizeof(cmd), 0, (struct sockaddr *)&infosClientUDP, &addrlen)) == -1) {
	perror("recvfrom()");
	return -2;
    }

    if (DEBUG) {
	struct in_addr ipAddr = ((struct sockaddr_in*)&infosClientUDP)->sin_addr;
	char str[INET_ADDRSTRLEN];
	printf("IPsrc[%s]:%d\n", inet_ntop( AF_INET, &ipAddr, str, INET_ADDRSTRLEN ),((struct sockaddr_in*)&infosClientUDP)->sin_port);
    }

    //    if(TRACE) printf("Received Command From UDP.\n");
    if ( cmd->Version != CURRENT_VERSION ){
	return -1;
    }

    return 0;
}
