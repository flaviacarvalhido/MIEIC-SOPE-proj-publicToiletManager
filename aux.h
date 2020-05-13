#pragma once
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <stdbool.h>
#include <ctype.h>

// Waits for given milliseconds
int mSleep(long milliseconds);


// Extracts data in given format
void extractData( const char * str, const char * format, ... );

struct command parser(int argc, char *argv[]);

struct Request{
    int request_number;
    pid_t pid;
    pthread_t tid;
    int duration;
    int placement;
};

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

