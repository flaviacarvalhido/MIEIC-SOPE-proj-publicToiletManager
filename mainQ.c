#include "register.h"
#include "parser.h"

int main(int argc, char *argv[]){

    struct command c;

    c=parser(argc,argv);

    if(c.error){
        printf("Error: wrong arguments provided!\n");
        printf("Usage: Q1 <-t nsecs> fifoname\n");
        exit(4);
    }

    pid_t pid, tid;

    pid = getpid();
    tid = gettid();

    action_type action = IAMIN;

    writeRegister(1, pid_t pid, pid_t tid, 3, 1, IAMIN);
    
    return 0;
}





