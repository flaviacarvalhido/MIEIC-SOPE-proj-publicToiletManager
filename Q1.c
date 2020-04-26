#include "register.h"
#include "parser.h"


int main(int argc, char *argv[]){

    struct command c;
    int fd;
    char fifoname[100];
    time_t start = time(NULL);

    c=parser(argc,argv);

    if(c.error){
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./Q1 <-t nsecs> fifoname\n");
        exit(4);
    }


    snprintf(fifoname, sizeof(fifoname), "/tmp/%s", c.fifoname);

    mkfifo(fifoname, 0660);

    fd = open(fifoname, O_RDONLY, 00444);

    time_t endwait;
    time_t seconds = c.nsecs;

    endwait = start + seconds;

    while (start < endwait)
    {
        char data_received[100];
        ssize_t bytes_read = read(fd, data_received, sizeof(data_received));

        start = time(NULL);
    }

    close(fd);

    remove(fifoname); 

    
    return 0;
}





