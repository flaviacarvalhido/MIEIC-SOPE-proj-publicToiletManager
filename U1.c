#include "register.h"
#include "parser.h"

int main(int argc, char *argv[]){

    struct command c;

    c=parser(argc,argv);

    if(c.error){
        printf("Error: wrong arguments provided!\n");
        printf("Usage: U1 <-t nsecs> fifoname\n");
        exit(4);
    }
    return 0;
}