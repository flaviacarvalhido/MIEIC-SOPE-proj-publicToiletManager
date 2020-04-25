
#include "register.h"
#include "parser.h"

int main(int argc, char *argv[]){

    struct command c;

    c=parser(argc,argv);

    if(c.error){
        printf("Error: wrong arguments provided.");
        exit(4);
    }


    
    return 0;
}