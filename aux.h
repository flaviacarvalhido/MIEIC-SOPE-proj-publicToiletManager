#pragma once
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>

// Waits for given milliseconds
int mSleep(long milliseconds);


// Extracts data in given format
void extractData( const char * str, const char * format, ... );


struct Request{
    int request_number;
    pid_t pid;
    pthread_t tid;
    int duration;
    int placement;
};