#include "register.h"

int writeRegister(int i, pid_t pid, pid_t tid, int duration, int placement, action_type oper) {
    
    char* to_print;

    snprintf(to_print, sizeof(to_print), "%d ; %d ; %d ; %d ; %d ; %d - %s\n", 
        time(NULL), i, pid, tid, duration, placement, action_type_string[oper]);

    
    printf("%s\n", to_print);

}

