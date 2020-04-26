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

    mkfifo(fifoname, 0660);

    fd = open(fifoname, O_RDONLY, 00444);

    char data_received[100];

    ssize_t bytes_read = read(fd, data_received, sizeof(data_received));

    printf("data_received: %s\n", data_received);

    close(fd);

    remove(fifoname); 

    
    return 0;
}





