#include "SerialServer.h"
#include "../Arduino/shared.h"

#define LOG TRUE
#define SERIAL_FILE_1 "/dev/ttyACM0"
#define SERIAL_FILE_2 "/dev/ttyACM1"
#define TEMPO_TRY_AGAIN_OPEN_SERIAL 3

int main(){
    char buf[MAXLINE];
    int fileDescriptorSerialPort1;
    // int fileDescriptorSerialPort2;
    int n;
    rio_t riobuf1;
    //    rio_t riobuf2;

    /*******************
     * OPEN            *
     *******************/
    fileDescriptorSerialPort1 = openSerial(SERIAL_FILE_1);
    //    fileDescriptorSerialPort2 = openSerial(SERIAL_FILE_2);

    /*    
	  command cmd;
	  cmd.Version  = 1;
	  cmd.Function = GET_ANALOG;
	  cmd.Argument = 0;
    */

    /*******************
     * READ LOOP       *
     *******************/
    //    if (fileDescriptorSerialPort1 > 0) {
    if(LOG) printf("\tInitialize Buffurized RIO.\n");
    Rio_readinitb(&riobuf1, fileDescriptorSerialPort1);
    //    Rio_readinitb(&riobuf2, fileDescriptorSerialPort2);
    
    if(LOG) printf("\tRead Serial and print to standard ouput until closed.\n");

    printf("Send first CMD.\n");
    //    Rio_writen(fileDescriptorSerialPort1, (void*)&cmd, sizeof(cmd));
    printf("OK.\n");
    while ( (n = Rio_readlineb(&riobuf1, buf, MAXLINE)) > 0 ) {
	printf("n=%d\n",n);
	/* on ecrit sur l'ecran */
	Rio_writen(1, buf, n);
	//	Rio_writen(fileDescriptorSerialPort1, (void*)&cmd, sizeof(cmd));
    }
      
    /*******************
     *                 *
     *******************/
    if(LOG) printf("\tSerial closed by peer.\n");
    //    }  
    return 0;
  
}


int openSerial(char* serialFile){
    int fileDescriptor;
    struct termios term;

    if(LOG) printf("\tOpen Serial [%s]\n",serialFile);
    while ( (fileDescriptor = open(serialFile, O_RDWR,0))<0) {
	if(LOG) printf("\t\tFailed. Returned FileDescriptor [%d].\n", fileDescriptor);
	if(LOG) printf("\t\t[%s]\n", strerror(errno));
	if(LOG) printf("\t\tWill try again in %d seconds.\n", TEMPO_TRY_AGAIN_OPEN_SERIAL);
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
    term.c_lflag &= ~ICANON; // Désactive mode CANONIQUE, cad n'attend le retour a la ligne
    term.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    term.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable any special handling of received bytes
    term.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes (e.g. newline chars)
    term.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    if (tcsetattr(fileDescriptor, TCSANOW, &term) != 0) {
	printf("Error %i from tcsetattr: %s\n", errno, strerror(errno));
    }
    
    return fileDescriptor;
  
}
