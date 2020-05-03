#include "register.h"
#include "parser.h"
#include "aux.h"

int fd_channels[10000];
int is_free[10000];

#define MAX_ATTEMPTS  5

void *processClient(void *arg)
{
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

    for (int i = 0; i < 10000; i++)
    {
        if(is_free[i] == 1){
            r.placement = i+1;
            is_free[i] = 0;
            break;
        }
    }
    
    char response_string[100];

    snprintf(response_string, sizeof(response_string), "[ %d, %d, %lu, %d, %d ]", r.request_number, getpid(), pthread_self(), r.duration, r.placement);

    if(write(fd_channels[r.request_number], response_string, sizeof(response_string)) == -1) {
        writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, GAVUP);
        pthread_exit(NULL);
    }

    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, ENTER); //se entrar

    // Tá no processo de cagar

    sleep(r.duration);

    // Time's up!
    writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TIMUP);

    is_free[r.placement - 1] = 1;

    close(fd_channels[r.request_number]);
    remove(fifo_private);

    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    for (int i = 0; i < 10000; i++)
    {
        is_free[i] = 1;
    }

    struct command c;
    int fd;
    char fifoname[100];
    pthread_t thread;
    time_t start = time(NULL);
    struct Request r;


    c = parser(argc, argv);

    if (c.error)
    {
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./Q1 <-t nsecs> fifoname\n");
        exit(4);
    }

    snprintf(fifoname, sizeof(fifoname), "/tmp/%s", c.fifoname);

    mkfifo(fifoname, 0660);

    fd = open(fifoname, O_RDONLY /*| O_NONBLOCK*/, 00444);

    time_t endwait;
    time_t seconds = c.nsecs;

    endwait = start + seconds;

    // Receives requests until closed

    char data_received[100];

    while (start < endwait)
    {
        mSleep(100);

        if (read(fd, data_received, sizeof(data_received))) { // Ler canal publico
            //read request info
            extractData(data_received, "[ %d, %d, %lu, %d, %d ]", &r.request_number, &r.pid, &r.tid, &r.duration, &r.placement);

            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, RECVD);

            //threads - uma para cada cliente

            pthread_create(&thread, NULL, processClient, (void *)&r);
        }

        //atualizar array free bathrooms para libertar casa de banho

        start = time(NULL);
    }

    mSleep(300);

    time_t start_2 = time(NULL);

    time_t endwait_2;
    time_t seconds_2 = 5 + 1;

    endwait_2 = start_2 + seconds_2;

    while (start_2 < endwait_2) // Tratar dos pedidos depois do encerramento
    {
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

            writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TOO_LATE);
        }
        start_2 = time(NULL);
    }

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

        writeRegister(r.request_number, getpid(), pthread_self(), r.duration, r.placement, TOO_LATE);
    }

    close(fd);
    remove(fifoname);

    return 0;
}
