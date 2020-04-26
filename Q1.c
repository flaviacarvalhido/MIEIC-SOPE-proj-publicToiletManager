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

    char data_received[100];

    ssize_t bytes_read = read(fd, data_received, sizeof(data_received));

    //printf("data_received: %s\n", data_received);

    time_t endwait;
    time_t seconds = c.nsecs; // end loop after this time has elapsed

    endwait = start + seconds;

    while (start < endwait)
    {
        /* Do stuff while waiting */
        start = time(NULL);
        sleep(1);
        //printf("loop time is : %s", ctime(&start));
    }

    //printf("end time is %s", ctime(&endwait));




    close(fd);

    remove(fifoname); 

    
    return 0;
}





