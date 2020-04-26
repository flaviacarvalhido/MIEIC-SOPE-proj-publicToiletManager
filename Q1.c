#include "register.h"
#include "parser.h"
#include <pthread.h>


int main(int argc, char *argv[]){

    struct command c;
    int fd;
    char fifoname[100];

    c=parser(argc,argv);

    if(c.error){
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./Q1 <-t nsecs> fifoname\n");
        exit(4);
    }


    snprintf(fifoname, sizeof(fifoname), "/tmp/%s", c.fifoname);

    printf("Making FIFO...\n");

    mkfifo(fifoname, 0660);

    printf("Opening...\n");

    fd = open(fifoname, O_RDONLY, 00444);

    printf("FD: %d\n", fd);

    char data_received[100];

    ssize_t bytes_read = read(fd, data_received, 100);

    printf("data_received: %s\n", data_received);

    close(fd);

    
    return 0;
}





