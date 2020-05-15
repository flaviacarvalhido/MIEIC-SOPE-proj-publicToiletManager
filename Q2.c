#include "register.h"
#include "aux.h"
#include "queue.h"
#include <time.h>
#include <semaphore.h>
#include <signal.h>


#define MAX_ATTEMPTS  5


int fd_channels[10000];
int is_free[10000];
struct Queue * queue;

sem_t nthread;
sem_t nplacements;

//int can_create = 0;
int endProgram = 0;



struct command c;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER; 

void *tooLate(void *arg)
{


    char fifo_private[1000];
    char response_string[100];
    struct Request r = *(struct Request *)arg;

    snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", r.pid, r.tid);

    do
    {
        fd_channels[r.request_number] = open(fifo_private, O_WRONLY, 00222);
        if (fd_channels[r.request_number] == -1) // Se ainda não tiver sido criado pelo reader
            usleep(100000);
    } while (fd_channels[r.request_number] == -1);

    //write in private channel 2LATE (= pos->-1)
    snprintf(response_string, sizeof(response_string), "[ %d, %d, %lu, %d, %d ]", r.request_number, getpid(), pthread_self(), r.duration, -1);
    write(fd_channels[r.request_number], response_string, sizeof(response_string));

    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, RECVD);
    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TOO_LATE);

    pthread_exit(NULL);
}



void sigalarm_handler(int signo){
    endProgram = 1;
}


void *processClient(void *arg)
{
    pthread_detach(pthread_self());

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
              
                sem_post(&nthread);
                pthread_exit(NULL);
            }
            usleep(100000);
        }
    } while (fd_channels[r.request_number] == -1);

    
    sem_wait(&nplacements);
    pthread_mutex_lock(&mut);
    r.placement = dequeue(queue);
    pthread_mutex_unlock(&mut);
    
    char response_string[100];

    snprintf(response_string, sizeof(response_string), "[ %d, %d, %lu, %d, %d ]", r.request_number, getpid(), pthread_self(), r.duration, r.placement);

    if(write(fd_channels[r.request_number], response_string, sizeof(response_string)) == -1) {
        writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, GAVUP);
        pthread_exit(NULL);
    }

    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, ENTER); //se entrar

    // Tá no processo de ir à casinha

    usleep(r.duration*100000);

    // Time's up!
    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TIMUP);

    sem_post(&nthread);
    sem_post(&nplacements);

    pthread_mutex_lock(&mut);
    enqueue(queue, r.placement);
    pthread_mutex_unlock(&mut);


    close(fd_channels[r.request_number]);

    

    remove(fifo_private);
    pthread_exit(NULL);
}



int main(int argc, char *argv[])
{
    
    struct sigaction act_alarm;
    act_alarm.sa_handler = sigalarm_handler;
    sigemptyset(&act_alarm.sa_mask);
    act_alarm.sa_flags = 0;

    if (sigaction(SIGALRM,&act_alarm,NULL) < 0)  {        
        fprintf(stderr,"Unable to install SIGALARM handler\n");        
        exit(1);  
    }  


    int fd;
    char fifoname[1000];
    pthread_t thread;
    char data_received[1000];
    struct Request r;

    
    c = parser(argc, argv);

    if (c.error)
    {
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname\n");
        exit(4);
    }

    sem_init(&nthread,0,c.nthreads);
    sem_init(&nplacements,0,c.nplaces);

    queue = createQueue(c.nplaces);
    fillQueue(queue);


    snprintf(fifoname, sizeof(fifoname), "/tmp/%s", c.fifoname);

    mkfifo(fifoname, 0660);

    fd = open(fifoname, O_RDONLY, 00444);



    alarm(c.nsecs);

    // Receives requests until closed
    while (!endProgram)
    {
        
       
        if (read(fd, data_received, sizeof(data_received)) > 0 ) { // Ler canal publico
            //read request info

            extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &r.request_number, &r.pid, &r.tid, &r.duration, &r.placement);

            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, RECVD);

            //threads - uma para cada cliente
            //2ndStage - número de threads limitadas

            
            //sem_wait
            sem_wait(&nthread);
            pthread_create(&thread, NULL, processClient, (void *)&r);


        }
        
    }


    usleep(1000000);

    printf("Fechou mano\n");

    int i = 0;

    while ((i = read(fd, data_received, sizeof(data_received))) > 0 || (queue->size != c.nplaces))
    {
        if(i > 0){
            i=0;
            //read request info
            extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &r.request_number, &r.pid, &r.tid, &r.duration, &r.placement);
            
            sem_wait(&nthread);
            pthread_create(&thread, NULL, tooLate, (void *)&r);
            pthread_join(thread, NULL);
        }
        i=0;
    }

    printf("Left while\n");

    /*while(queue->size != c.nplaces){
        usleep(1000);
    }*/

    usleep(1000000);
   

    
    //pthread_join(last_one, NULL);

    pthread_mutex_destroy(&mut);

    close(fd);
    remove(fifoname);

    return 0;
}