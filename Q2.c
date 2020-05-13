#include "register.h"
#include "aux.h"
#include <time.h>


#define MAX_ATTEMPTS  5


int fd_channels[10000];
int is_free[10000];
int queue[10000];
pthread_t last_one;

struct command c;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER; 

void *processClient(void *arg)
{
    last_one=pthread_self();

    char fifo_private[1000];

    int attempts = 0;

    struct Request r = *(struct Request *)arg;

    snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", r.pid, r.tid);

    do
    {
        fd_channels[r.request_number] = open(fifo_private, O_WRONLY, 00222);
        if (fd_channels[r.request_number] == -1){ // Se ainda não tiver sido criado pelo reader
            attempts++;
            if (attempts >= MAX_ATTEMPTS)
            {
                writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, GAVUP);
                pthread_exit(NULL);
            }
            sleep(1);
        }
    } while (fd_channels[r.request_number] == -1);

    // Verificar se pode entrar... (calcula r.placement)
    // First try
    for (int i = 0; i <= c.nplaces; i++)
    {
        if(is_free[i] == 1){
            r.placement = i+1;
            is_free[i] = 0;
            break;
        }
    }

    if(r.placement == -1)
        queue[r.request_number] = 1;

    // Didn't get place: waiting line
    while(r.placement == -1){
        pthread_mutex_lock(&mut); // Só pode entrar uma thread de cada vez
        for (int i = 0; i <= c.nplaces; i++)
        {
            if(is_free[i] == 1 && queue[r.request_number-1] != 1){
                r.placement = i+1;
                is_free[i] = 0;
                queue[r.request_number]=-1;
                break;
            }
        }
        pthread_mutex_unlock(&mut);
    }
    
    char response_string[100];

    snprintf(response_string, sizeof(response_string), "[ %d, %d, %lu, %d, %d ]", r.request_number, getpid(), pthread_self(), r.duration, r.placement);

    if(write(fd_channels[r.request_number], response_string, sizeof(response_string)) == -1) {
        writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, GAVUP);
        pthread_exit(NULL);
    }

    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, ENTER); //se entrar

    // Tá no processo de ir à casinha

    sleep(r.duration);

    // Time's up!
    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TIMUP);

    is_free[r.placement - 1] = 1;

    close(fd_channels[r.request_number]);

    

    remove(fifo_private);

    pthread_exit(NULL);
}

int notTotallyFree(){
    for (int i = 0; i < 10000; i++)
    {
        if(is_free[i] != 1){
            return 1;
        }
    }

    return 0;
    
}


int queueHasPeople(){
    for (int i = 0; i < 10000; i++)
    {
        if(queue[i] != -1){
            return 1;
        }
    
    }

    return 0;
    
}

int main(int argc, char *argv[])
{

    for (int i = 0; i < 10000; i++)
    {
        is_free[i] = 1;
    }

    for (int i = 0; i < 10000; i++)
    {
        queue[i] = -1;
    }
    

    
    int fd;
    char fifoname[100];
    pthread_t thread;
    struct Request r;


    c = parser(argc, argv);

    if (c.error)
    {
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
        exit(4);
    }

    if(c.nplaces == 0){
        c.nplaces = 10000;
    }
    
    if(c.nthreads == 0){
        c.nthreads = 10000;
    }

    printf("Size of is_free: %ld\n", sizeof(is_free)/sizeof(int));
    printf("Size of fd_channels: %ld\n", sizeof(fd_channels)/sizeof(int));

    snprintf(fifoname, sizeof(fifoname), "/tmp/%s", c.fifoname);

    mkfifo(fifoname, 0660);

    fd = open(fifoname, O_RDONLY /*| O_NONBLOCK*/, 00444);

    time_t start = time(NULL);

    time_t endwait;
    time_t seconds = c.nsecs;

    endwait = start + seconds;

    // Receives requests until closed

    char data_received[100];
    char last_request[100];



    while (start < endwait)
    {
        mSleep(100);

        if (read(fd, data_received, sizeof(data_received))) { // Ler canal publico
            //read request info
            extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &r.request_number, &r.pid, &r.tid, &r.duration, &r.placement);

            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, RECVD);

            //threads - uma para cada cliente
            // 2ndStage - número de threads limitadas





            pthread_create(&thread, NULL, processClient, (void *)&r);
        }

        //atualizar array free bathrooms para libertar casa de banho

        start = time(NULL);
    }


    strcpy(last_request, data_received);


    
    mSleep(300);

    /*

    time_t start_2 = time(NULL);

    time_t endwait_2;
    time_t seconds_2 = 5 + 1;

    endwait_2 = start_2 + seconds_2;

    while (start_2 < endwait_2) // Tratar dos pedidos depois do encerramento
    {
        printf("Entering here\n");
        if (read(fd, data_received, sizeof(data_received)))
        { // Ler canal publico
            char fifo_private[1000];
            char response_string[100];

            //read request info
            extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &r.request_number, &r.pid, &r.tid, &r.duration, &r.placement);

            //open private channel
            snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", r.pid, r.tid);

            do
            {
                fd_channels[r.request_number] = open(fifo_private, O_WRONLY, 00222);
                if (fd_channels[r.request_number] == -1) // Se ainda não tiver sido criado pelo reader
                    sleep(1);
            } while (fd_channels[r.request_number] == -1);

            //write in private channel 2LATE (= pos->-1)
            snprintf(response_string, sizeof(response_string), "[ %d, %d, %lu, %d, %d ]", r.request_number, getpid(), pthread_self(), r.duration, -1);
            write(fd_channels[r.request_number], response_string, sizeof(response_string));

            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, RECVD);
            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TOO_LATE);
        }
        start_2 = time(NULL);
    }

    */

    while (read(fd, data_received, sizeof(data_received)))
    {
        char fifo_private[1000];
        char response_string[100];

        //read request info
        extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &r.request_number, &r.pid, &r.tid, &r.duration, &r.placement);

        //open private channel
        snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", r.pid, r.tid);

        do
        {
            fd_channels[r.request_number] = open(fifo_private, O_WRONLY, 00222);
            if (fd_channels[r.request_number] == -1) // Se ainda não tiver sido criado pelo reader
                sleep(1);
        } while (fd_channels[r.request_number] == -1);

        //write in private channel 2LATE (= pos->-1)
        snprintf(response_string, sizeof(response_string), "[ %d, %d, %lu, %d, %d ]", r.request_number, getpid(), pthread_self(), r.duration, -1);
        write(fd_channels[r.request_number], response_string, sizeof(response_string));

        writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, RECVD);
        writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TOO_LATE);
    }
    
    
    pthread_join(last_one, NULL);

    close(fd);
    remove(fifoname);

    return 0;
}