#pragma once
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <ctype.h>



struct command
{
    bool isU;
    bool isQ;
    bool isQ2;
    int nsecs;
    int nplaces;
    int nthreads;
    char * fifoname;
    bool error;
};




