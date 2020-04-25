#pragma once 
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>


typedef enum {IWANT, RECVD, ENTER, IAMIN, TIMUP, TOO_LATE, CLOSD, FAILD, GAVUP} action_type;
static char* action_type_string[] = {"IWANT", "RECVD", "ENTER", "IAMIN", "TIMUP", "2LATE", "CLOSD", "FAILD", "GAVUP"};


int register(pid_t pid, pid_t tid, int duration, int placement, action_type action);

