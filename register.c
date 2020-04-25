#include "register.h"

void writeRegister(int i, pid_t pid, pid_t tid, int duration, int placement, action_type oper) {
    
    char to_print[100];

    snprintf(to_print, sizeof(to_print), "%ld ; %d ; %d ; %d ; %d ; %d ; %s", 
        time(NULL), i, pid, tid, duration, placement, action_type_string[oper]);

    
    printf("%s\n", to_print);

}

