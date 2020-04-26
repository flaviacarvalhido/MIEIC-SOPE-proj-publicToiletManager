#include "register.h"
#include "parser.h"
#include "aux.h"

int fd;
int fd_channels[10000];


void * thread_function(void * arg){
    char fifo_data[100];
    char fifo_private[3000];

    int request_number = *((int *) arg);

    pthread_t tid = pthread_self();
    int dur = rand() % (5 + 1 - 1) + 1;   //randomizer
    
    snprintf(fifo_data, sizeof(fifo_data), "[ %d, %d, %lu, %d, %d ]", request_number, getpid(), tid, dur, -1);
    write(fd, fifo_data, sizeof(fifo_data)); 

    snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", getpid(), tid);

    fd_channels[request_number] = open(fifo_private, O_RDONLY, 00444);

    char string_received[100];

    read(fd, string_received, sizeof(string_received));

    printf("STRING RECEIVED: %s\n", string_received); // PRINTS DENTRO DAQUI PODE MERDAR

    pthread_exit(NULL);
}


int main(int argc, char *argv[]){

    struct command c;
    char fifoname[100];
    time_t start = time(NULL);


    c=parser(argc,argv);

    if(c.error){
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./U1 <-t nsecs> fifoname\n");
        exit(4);
    }

    snprintf(fifoname, sizeof(fifoname), "/tmp/%s", c.fifoname);

    do
    {
        fd = open(fifoname, O_WRONLY, 00222);
        if (fd == -1) // Se ainda não tiver sido criado pelo reader
            sleep(1);
    } while (fd == -1);

    time_t endwait;
    time_t seconds = c.nsecs;

    endwait = start + seconds;

    int request_number = 0;

    // Sends requests until closed 

    while (start < endwait) {
        void * received;
        pthread_t thread;

        pthread_create(&thread, NULL, thread_function, &request_number);

        request_number++;

        mSleep(100);

        start = time(NULL);
    }

    close(fd);
    
    return 0;
}