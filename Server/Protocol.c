/******************/
/* COUTAUD Ulysse */
/* L3P AII LYON1  */
/* 2022		  */
/******************/

#include "Protocol.h"

int commandIsValid(command* cmd){
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
