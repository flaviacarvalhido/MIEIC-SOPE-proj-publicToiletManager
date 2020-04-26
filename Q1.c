#include "register.h"
#include "parser.h"
#include "aux.h"
#include <stdarg.h>

void extractData( const char * str, const char * format, ... )
{
  va_list args;
  va_start (args, format);
  vsscanf (str, format, args);
  va_end (args);
}


int fd_channels[10000];

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

    // Receives requests until closed 


    while (start < endwait)
    {
        char data_received[100];
        char fifo_private[3000];
        int request_number, duration, pos;
        pid_t pid;
        pthread_t tid;

        ssize_t bytes_read = read(fd, data_received, sizeof(data_received));

        //read tid of request
        extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &request_number, &pid, &tid, &duration, &pos);

        snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", pid, tid);
        
        do {
            fd_channels[request_number]= open(fifo_private, O_WRONLY, 00222);
            if (fd_channels[request_number] == -1) // Se ainda não tiver sido criado pelo reader
                sleep(1);
        } while (fd_channels[request_number] == -1);

        char string_test[100] = "Talas\n";

        write(fd, string_test, sizeof(string_test));

        //answer the request(place, wait, etc.)
        

        printf("DATA RECEIVED: %s\n", data_received);

        start = time(NULL);
    }

    close(fd);

    remove(fifoname); 

    
    return 0;
}





