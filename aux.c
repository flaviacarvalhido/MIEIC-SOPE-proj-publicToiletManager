#include "aux.h"


void extractData( const char * str, const char * format, ... ) {
  va_list args;
  va_start (args, format);
  vsscanf (str, format, args);
  va_end (args);
}


int mSleep(long milliseconds){

    struct timespec ts;
    int res;

    if (milliseconds < 0)
        return -1;

    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res);

    return res; // 0 if success, != 0 otherwise
}