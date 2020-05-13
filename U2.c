#include "register.h"
#include "aux.h"

int fd;
int fd_channels[10000];
pthread_t wow;

void * thread_function(void * arg){
    wow=pthread_self();

    char fifo_data[100];
    char fifo_private[3000];

    int request_number = *((int *) arg);

    pthread_t tid = pthread_self();
    int dur = rand() % (5 + 1 - 1) + 1;   //randomizer

    pid_t pid; // Descartado
    int pos;
    
    snprintf(fifo_data, sizeof(fifo_data), "[ %d, %d, %lu, %d, %d ]", request_number, getpid(), tid, dur, -1);

    write(fd, fifo_data, sizeof(fifo_data)); 

    writeRegister(request_number, getpid(), pthread_self(), dur, -1, IWANT);

    snprintf(fifo_private, sizeof(fifo_private), "/tmp/%d.%lu", getpid(), tid);

    mkfifo(fifo_private, 0660);

    fd_channels[request_number] = open(fifo_private, O_RDONLY, 00444); // Canal privado

    char string_received[100];

    mSleep(100);

    while(read(fd_channels[request_number], string_received, sizeof(string_received)) == -1){
        writeRegister(request_number, getpid(), pthread_self(), dur, -1, FAILD);
        pthread_exit(NULL);
    }

    extractData(string_received, "[ %d, %d, %lu, %d, %d ]", &request_number, &pid, &tid, &dur, &pos);

    // Processa resposta, vamos assumir que consegue entrar, se receber 2LATE, escreve CLOSD

    if(pos == -1)
        writeRegister(request_number, getpid(), pthread_self(), dur, pos, CLOSD);
    else
        writeRegister(request_number, getpid(), pthread_self(), dur, pos, IAMIN);


    close(fd_channels[request_number]);
    unlink(fifo_private);

    pthread_exit(NULL);
}


int main(int argc, char *argv[]){

    struct command c;
    char fifoname[100];


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

    time_t start = time(NULL);

    time_t endwait;
    time_t seconds = c.nsecs;

    endwait = start + seconds;

    int request_number = 0;

    // Sends requests until closed 

    while (start < endwait) {
        pthread_t thread;

        pthread_create(&thread, NULL, thread_function, &request_number);

        request_number++;

        mSleep(100); // TODO: Mudar para 100

        start = time(NULL);
    }

    //sleep(1);

    pthread_join(wow, NULL);

    printf("Client closed\n");

    

    close(fd);
    unlink(fifoname);
    
    return 0;
}
