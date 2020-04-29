#include "register.h"
#include "parser.h"
#include "aux.h"

int fd_channels[10000];

void * processClient(void * arg){
    char fifo_private[1000];

    struct Request r = *(struct Request *) arg;



    snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", r.pid, r.tid);

    do {
        fd_channels[r.request_number]= open(fifo_private, O_WRONLY, 00222);
        if (fd_channels[r.request_number] == -1) // Se ainda não tiver sido criado pelo reader
            sleep(1);
    } while (fd_channels[r.request_number] == -1);

    // Verificar se pode entrar... (calcula r.placement)
    r.placement=69; //temporary value for testing

    char response_string[100];

    snprintf(response_string, sizeof(response_string), "[ %d, %d, %lu, %d, %d ]", r.request_number, getpid(), pthread_self(), r.duration, r.placement);

    write(fd_channels[r.request_number], response_string, sizeof(response_string));

    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, ENTER);  //se entrar

    // Tá no processo de cagar
    
    sleep(r.duration);

    // Time's up!
    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TIMUP);

    close(fd_channels[r.request_number]);
    remove(fifo_private);

    pthread_exit(NULL);
}



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

    char data_received[100];

    while (start < endwait)
    {
        int request_number, duration, pos;
        pid_t pid;
        pthread_t thread;
        pthread_t tid;
        
        struct Request r;
        
        mSleep(100);

        if(read(fd, data_received, sizeof(data_received))){ // Ler canal publico
            
            //read request info
            extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &r.request_number, &r.pid, &r.tid, &r.duration, &r.placement);

            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, RECVD);

            //threads - uma para cada cliente

            pthread_create(&thread, NULL, processClient, (void *)&r);

        } 

        //atualizar array free bathrooms para libertar casa de banho
        
        start = time(NULL);
    }

    printf("CLOSED TOILET\n");

    time_t start_2 = time(NULL);

    time_t endwait_2;
    time_t seconds_2 = 5;

    endwait_2 = start_2 + seconds_2;

    while (start_2 < endwait_2) // Tratar dos pedidos depois do encerramento
    {
        if(read(fd, data_received, sizeof(data_received))){ // Ler canal publico
            
            struct Request r;
            char fifo_private[1000];
            char response_string[100];
            
            //read request info
            extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &r.request_number, &r.pid, &r.tid, &r.duration, &r.placement);

            //open private channel
            snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", r.pid, r.tid);

            do {
                fd_channels[r.request_number]= open(fifo_private, O_WRONLY, 00222);
                if (fd_channels[r.request_number] == -1) // Se ainda não tiver sido criado pelo reader
                    sleep(1);
            } while (fd_channels[r.request_number] == -1);

            //write in private channel 2LATE (= pos->-1)
            snprintf(response_string, sizeof(response_string), "[ %d, %d, %lu, %d, %d ]", r.request_number, getpid(), pthread_self(), r.duration, -1);
            write(fd_channels[r.request_number], response_string, sizeof(response_string));

            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TOO_LATE);

        } 
        start_2 = time(NULL);
    }
    
    close(fd);
    remove(fifoname);
    
    return 0;
}





