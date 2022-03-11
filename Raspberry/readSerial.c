/************************/
/*    COUTAUD Ulysse    */
/*    2022              */
/*    TODO GPL          */
/************************/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <termios.h>
#include "csapp.h"

#define TRUE 1==1
#define FALSE 0==2
#define LOG TRUE
#define SERIAL_FILE "/dev/ttyACM0"
#define TEMPO_TRY_AGAIN_OPEN_SERIAL 3

int main(){
  char buf[MAXLINE];
  int serial_port;
  int n;
  rio_t riobuf;

  /*******************
   * OPEN            *
   *******************/
  if(LOG) printf("\tOpen Serial [%s]\n",SERIAL_FILE);
  while ( (serial_port=open(SERIAL_FILE, O_RDONLY,0))<0) {
    if(LOG) printf("\t\tFailed. Returned FileDescriptor [%d].\n", serial_port);
    if(LOG) printf("\t\t[%s]\n", strerror(errno));
    if(LOG) printf("\t\tWill try again in %d seconds.\n", TEMPO_TRY_AGAIN_OPEN_SERIAL);
    sleep(TEMPO_TRY_AGAIN_OPEN_SERIAL);
  }


  /*******************
   * READ LOOP       *
   *******************/
  if (serial_port > 0) {
    if(LOG) printf("\tOpen Serial successful. FileDescriptor[%d]\n", serial_port);
    if(LOG) printf("\tInitialize Buffurized RIO.\n");
    Rio_readinitb(&riobuf, serial_port);

    if(LOG) printf("\tRead Serial and print to standard ouput until closed.\n");    
    while ( (n = Rio_readlineb(&riobuf, buf, MAXLINE)) > 0 ) {
      /* on ecrit sur l'ecran */
      Rio_writen(1, buf, n);
    }

  /*******************
   *                 *
   *******************/
    if(LOG) printf("\tSerial closed by peer.\n");
  }  
  return 0;
  
}
