#pragma once 
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <pthread.h>    
    

typedef enum {IWANT, RECVD, ENTER, IAMIN, TIMUP, TOO_LATE, CLOSD, FAILD, GAVUP} action_type;
static char* action_type_string[] = {"IWANT", "RECVD", "ENTER", "IAMIN", "TIMUP", "2LATE", "CLOSD", "FAILD", "GAVUP"};

void writeRegister(int i, pid_t pid, pthread_t tid, int duration, int placement, action_type oper);
