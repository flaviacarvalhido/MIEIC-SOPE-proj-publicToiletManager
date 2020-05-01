#include "register.h"

void writeRegister(int i, pid_t pid, pthread_t tid, int duration, int placement, action_type oper) {
    char* action_type_string[] = {"IWANT", "RECVD", "ENTER", "IAMIN", "TIMUP", "2LATE", "CLOSD", "FAILD", "GAVUP"};
    char to_print[100];

    snprintf(to_print, sizeof(to_print), "%ld ; %d ; %d ; %lu ; %d ; %d ; %s", 
        time(NULL), i, pid, tid, duration, placement, action_type_string[oper]);

    
    printf("%s\n", to_print);

}

