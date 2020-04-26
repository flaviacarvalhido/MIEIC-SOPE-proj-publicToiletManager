#include "aux.h"

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