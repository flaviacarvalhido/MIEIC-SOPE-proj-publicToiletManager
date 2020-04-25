#include "parser.h"


struct command parser(int argc, char *argv[])
{
    struct command c;

    if(strcmp(argv[0], "U1") == 0 || strcmp(argv[0], "u1") == 0){
        c.isU=true;
    }

    if(strcmp(argv[0], "U2") == 0 || strcmp(argv[0], "u2") == 0){
        c.isU=true;
    }

    if(strcmp(argv[0], "Q1") == 0 || strcmp(argv[0], "q1") == 0){
        c.isQ=true;
    }

    if(strcmp(argv[0], "Q2") == 0 || strcmp(argv[0], "q2") == 0){
        c.isQ2=true;
    }

    for(int i = 1; i<argc;i++){
        
        if(strcmp(argv[i],"-t")==0){
            
            i++;
            if (atoi(argv[i]))
                c.nsecs = atoi(argv[i]);
            else
            {
                c.error = true;
                return c;
            }
            continue;
        }

        if(strcmp(argv[i], "-n") == 0 && c.isQ2){
            
            i++;
            if (atoi(argv[i]))
                c.nthreads = atoi(argv[i]);
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
            }else{
                c.error=true;
                return c;
            }
            continue;
        }
        
        c.fifoname = argv[i];
        
    }

    if(argc!=4 && c.isU){
        c.error=true;
    }

    if(!c.isQ && !c.isU && !c.isQ2){
        c.error=true;
    }

    return c;

}













