#include "register.h"
#include "parser.h"
#include <pthread.h>


int main(int argc, char *argv[]){

    struct command c;

    c=parser(argc,argv);

    if(c.error){
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./Q1 <-t nsecs> fifoname\n");
        exit(4);
    }
    
    return 0;
}





