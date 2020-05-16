#include "register.h"
#include "aux.h"

int fd;
int fd_channels[10000];
pthread_t last_one;
int endProgram = 0;

pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER; 

void sigalarm_handler(int signo){
    endProgram = 1;
}


void * thread_function(void * arg){
    

    char fifo_data[100];
    char fifo_private[3000];

    int request_number = *((int *) arg);

    pthread_t tid = pthread_self();
    int dur = rand() % (5 + 1 - 1) + 1;   // Randomizer

    pid_t pid; // Descartado
    int pos;
    
    snprintf(fifo_data, sizeof(fifo_data), "[ %d, %d, %lu, %d, %d ]", request_number, getpid(), tid, dur, -1);

    write(fd, fifo_data, sizeof(fifo_data)); 

    writeRegister(request_number, getpid(), pthread_self(), dur, -1, IWANT);
    fflush(stdout);

    snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", getpid(), tid);

    mkfifo(fifo_private, 0660);

    fd_channels[request_number] = open(fifo_private, O_RDONLY, 00444); // Canal privado

    char string_received[100];

    usleep(100*1000);

    while(read(fd_channels[request_number], string_received, sizeof(string_received)) == -1){
        writeRegister(request_number, getpid(), pthread_self(), dur, -1, FAILD);
        fflush(stdout);
        pthread_exit(NULL);
    }

    extractData(string_received, "[ %d, %d, %lu, %d, %d ]", &request_number, &pid, &tid, &dur, &pos);

    // Processa resposta, vamos assumir que consegue entrar, se receber 2LATE, escreve CLOSD

    if(pos == -1){
        writeRegister(request_number, getpid(), pthread_self(), dur, pos, CLOSD);
        fflush(stdout);
    }
    else{
        pthread_mutex_lock(&mut);
        last_one=pthread_self();
        pthread_mutex_unlock(&mut);
        writeRegister(request_number, getpid(), pthread_self(), dur, pos, IAMIN);
        fflush(stdout);
        usleep(dur * 100000);
    }

    close(fd_channels[request_number]);
    unlink(fifo_private);

    pthread_exit(NULL);
}


int main(int argc, char *argv[]){

    struct command c;
    char fifoname[100];


    //incializar alarme
    struct sigaction act_alarm;
    act_alarm.sa_handler = sigalarm_handler;
    sigemptyset(&act_alarm.sa_mask);
    act_alarm.sa_flags = 0;

    if (sigaction(SIGALRM,&act_alarm,NULL) < 0)  {        
        fprintf(stderr,"Unable to install SIGALARM handler\n");        
        exit(1);  
    }  

    c=parser(argc,argv);

    if(c.error){
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./U2 <-t nsecs> fifoname\n");
        exit(4);
    }
    
    snprintf(fifoname, sizeof(fifoname), "/tmp/%s", c.fifoname);

    do
    {
        fd = open(fifoname, O_WRONLY, 00222); // Canal público
        if (fd == -1) // Se ainda não tiver sido criado pelo reader
            sleep(1);
    } while (fd == -1);

   

    int request_number = 0;

    //inicializa tempo de programa
    alarm(c.nsecs);

    // Sends requests until closed 
    while (!endProgram) {
        pthread_t thread;

        pthread_create(&thread, NULL, thread_function, &request_number);

        request_number++;

        usleep(100*1000);
 
    }

    usleep(5*1000000);

    pthread_join(last_one, NULL);

    
    close(fd);
    unlink(fifoname);
    
    return 0;
}
