#include "register.h"
#include "aux.h"
#include <time.h>


#define MAX_ATTEMPTS  5


int fd_channels[10000];
int is_free[10000];
int queue[10000];
struct Request queue2[10000];
pthread_t last_one;

int can_create;

struct command c;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER; 


void *processClient(void *arg)
{
    last_one = pthread_self();
   
    char fifo_private[1000];

    int attempts = 0;

    struct Request r = *(struct Request *)arg;

    printf("Request Received Info:\n");
    printf("Request Number: %d:\n", r.request_number);
    printf("Request PID: %d:\n", r.pid);
    printf("Request TID: %ld:\n", r.tid);
    printf("Request Duration: %d:\n", r.duration);
    printf("Request Placement: %d:\n", r.placement);

    
    snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", r.pid, r.tid);

    do
    {   
        fd_channels[r.request_number] = open(fifo_private, O_WRONLY, 00222);
        if (fd_channels[r.request_number] == -1){ // Se ainda não tiver sido criado pelo reader
            printf("IT AINT OPEN\n");
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
    for (int i = 0; i < c.nplaces; i++)
    {
        if(is_free[i] == 1){
            r.placement = i+1;
            is_free[i] = 0;
            break;
        }
    }

    if(r.placement == -1){
        queue[r.request_number] = 1;
    }

    // Didn't get place: waiting queue
    while(r.placement == -1){
        pthread_mutex_lock(&mut); // Só pode entrar uma thread de cada vez
        for (int i = 0; i < c.nplaces; i++)
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

    //free the placement
    is_free[r.placement - 1] = 1;


    //close & remove channel, exit
    close(fd_channels[r.request_number]);

    remove(fifo_private);

    pthread_mutex_lock(&mut2); // Só pode entrar uma thread de cada vez
    can_create--;
    printf("Can create depois de decrementar: %d\n", can_create);
    pthread_mutex_unlock(&mut2); // Só pode entrar uma thread de cada vez

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{

    int fd;
    char fifoname[100];
    pthread_t thread;
    struct Request r;
    struct Request nullRequest;
    struct Request temp;
    
    nullRequest.request_number = -404; // Request not found

    char data_received[100];
    can_create = 0;

    //filling up the global arrays with default values
    for (int i = 0; i < 10000; i++)
    {
        is_free[i] = 1;
    }
    for (int i = 0; i < 10000; i++)
    {
        queue[i] = -1;
    }
    for (int i = 0; i < 10000; i++)
    {
        queue2[i] = nullRequest;
    }

    //parse command line
    c = parser(argc, argv);

    if (c.error)
    {
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
        exit(4);
    }


    //open public channel
    snprintf(fifoname, sizeof(fifoname), "/tmp/%s", c.fifoname);

    mkfifo(fifoname, 0660);

    fd = open(fifoname, O_RDONLY, 00444);


    //start time for while loop
    time_t start = time(NULL);

    time_t endwait;
    time_t seconds = c.nsecs;

    endwait = start + seconds;

    // Receives requests until closed
    while (start < endwait)
    {
        mSleep(300);

        if (read(fd, data_received, sizeof(data_received))) { // Ler canal publico

            extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &r.request_number, &r.pid, &r.tid, &r.duration, &r.placement);

            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, RECVD);

            //threads - uma para cada cliente
            // 2ndStage - número de threads limitadas
            queue2[r.request_number] = r;
            
        }

        if(can_create < c.nthreads){
            printf("Can create: %d\n", can_create);
            for(int i = 1; i < 10000; i++)
            {
                if (queue2[i].request_number != nullRequest.request_number)
                {
                    //this fixes the problem :))))))))))))))
                    temp.request_number = queue2[i].request_number;
                    temp.pid = queue2[i].pid;
                    temp.tid = queue2[i].tid;
                    temp.duration = queue2[i].duration;
                    temp.placement = queue2[i].placement;
                    
                    printf("Request Number Passado: %d\n", temp.request_number);
                    pthread_create(&thread, NULL, processClient, (void *)&temp);
                    printf("Created thread\n");
                    queue2[i] = nullRequest;
                    can_create++;
                    printf("Can create: %d\n", can_create);
                    break;
                }   
            }
        }
       
        start = time(NULL);
    }

    mSleep(300);

    //While para tratar de pedidos que estão na Queue e pedidos que cheguem após a casa de banho fechar (mais simples que na primeira entrega)
    while (true)
    {
        if(read(fd, data_received, sizeof(data_received))){
            mSleep(300);
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


            //write RECVD & TOO_LATE
            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, RECVD);
            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TOO_LATE);
        }
        else {
            if(pthread_join(last_one, NULL) == 0){
                break;
            }
        }
    }

    close(fd);
    remove(fifoname);

    return 0;
}