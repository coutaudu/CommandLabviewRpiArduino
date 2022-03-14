/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "SerialServer.h"

int main(){
    char buf[MAXLINE];
    int fileDescriptorSerialPort1;
    // int fileDescriptorSerialPort2;
    int n;
    rio_t riobuf1;
    //    rio_t riobuf2;
    command cmd;
    int exit;
   
    fileDescriptorSerialPort1 = openSerial(SERIAL_FILE_1);
    //    fileDescriptorSerialPort2 = openSerial(SERIAL_FILE_2);    
    
    if(LOG) printf("\tInitialize Buffurized RIO.\n");
    Rio_readinitb(&riobuf1, fileDescriptorSerialPort1);
    //    Rio_readinitb(&riobuf2, fileDescriptorSerialPort2);
    
    if(LOG) printf("\tRead Serial and print to standard ouput until closed.\n");
    
    exit = CLI(&cmd);
    while (!exit) {
	Rio_writen(fileDescriptorSerialPort1, (void*)&cmd, sizeof(cmd));
	n = Rio_readnb(&riobuf1, buf, sizeof(cmd));
	printf("n=%d\n",n);
	printf("<<%u>>\n",*((short*)buf));
	exit = CLI(&cmd);
    };

    close(fileDescriptorSerialPort1);
    if(LOG) printf("\tSerial closed by peer.\n");
    return 0;
  
}


int CLI(command* cmd){
    int exit = FALSE;
    int valide = FALSE;
    char entry;

    while(!valide) {
	printf("\t[0] Exit.\n");
	printf("\t[1] Read Analog Value.\n");
	scanf(" %c",&entry); // Attention l'espace de le % est important.
	switch (entry){
	case '0':
	    exit = TRUE;
	    valide = TRUE;
	    break;
	case '1':
	    commandGetAnalog(cmd);
	    valide = TRUE;
	    break;
	default :
	    printf("\t[Commande Invalide]\n.");
	    break;
	}
	scanf("%*[^\n]"); // Vide le buffer d'entree
    }
    return exit;
}

int commandGetAnalog(command* cmd){
    char entry;
    cmd->Version  = 1;
    cmd->Function = GET_ANALOG;
    printf("\t[1]\tWhich Pin ? [0-5]\n");
    scanf(" %c",&entry);
    cmd->Argument[0] = entry - '0';

    return 0;
}

int openSerial(char* serialFile){
    int fileDescriptor;
    struct termios term;

    if(LOG) printf("\tOpen Serial [%s]\n",serialFile);
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
    
    return fileDescriptor;
  
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
