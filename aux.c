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

struct command parser(int argc, char *argv[])
{
    struct command c;

    c.error=false;
    c.isU = false;
    c.isQ = false;
    c.isQ2 = false;
    c.fifoname="";
    c.nplaces = 0;
    c.nthreads = 0;

    if( (strcmp(argv[0], "./U1") == 0) || (strcmp(argv[0], "./u1") == 0) ){
        c.isU=true;
    }

    if( (strcmp(argv[0], "./U2") == 0) || (strcmp(argv[0], "./u2") == 0) ){
        c.isU=true;
    }

    if( (strcmp(argv[0], "./Q1") == 0) || (strcmp(argv[0], "./q1") == 0) ){
        c.isQ=true;
    }

    if( (strcmp(argv[0], "./Q2") == 0) || (strcmp(argv[0], "./q2") == 0) ){
        c.isQ2=true;
    }

    if(argc != 4 && c.isU){
        c.error=true;
        return c;
    }

    if(argc > 8 && c.isQ)
    {
        c.error = true;
        return c;
    }

    for(int i = 1; i<argc;i++){
        
        if(strcmp(argv[i],"-t")==0){
            
            i++;
            if (atoi(argv[i]))
            {
                c.nsecs = atoi(argv[i]);
                if (c.nsecs <= 0)
                {
                    c.error = true;
                    return c;
                }
            }
                 
            else
            {
                c.error = true;
                return c;
            }
            continue;
        }

        if(strcmp(argv[i], "-n") == 0 && c.isQ2){
            
            i++;
            if (atoi(argv[i])){
                c.nthreads = atoi(argv[i]);
                if (c.nthreads <= 0)
                {
                    c.error = true;
                    return c;
                }
            }
            else
            {
                c.error = true;
                return c;
            }
            continue;
        }

        if(strcmp(argv[i],"-l")==0 && c.isQ2){
            
            i++;
            if(atoi(argv[i])){
                c.nplaces = atoi(argv[i]);
                if (c.nplaces <= 0)
                {
                    c.error = true;
                    return c;
                }
            }else{
                c.error=true;
                return c;
            }
            continue;
        }
        
        c.fifoname = argv[i];
        
    }

    if( (strcmp(c.fifoname, "") == 0) ) {
        c.error=true;
    }

    if(c.nplaces < 0 || c.nthreads <0){
        c.error=true;
    }
    
    return c;

}



