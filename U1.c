#include "register.h"
#include "parser.h"
#include <pthread.h>

int main(int argc, char *argv[]){

    struct command c;
    char fifoname[100];
    int fd;

    c=parser(argc,argv);

    if(c.error){
        printf("Error: wrong arguments provided!\n");
        printf("Usage: ./U1 <-t nsecs> fifoname\n");
        exit(4);
    }

    snprintf(fifoname, sizeof(fifoname), "/tmp/%s", c.fifoname);


    // lança continuamente (i.e. com intervalos de alguns milissegundos) threads, cada um ficando associado a um pedido;

    // cada thread gera aleatoriamente a duração do acesso,
    // e trata de toda a comunicação com o servidor (os pedidos são enviados ao servidor através do canal público fifoname); 
    // as respostas do servidor são recebidas por um canal privado com nome, criado e posteriormente eliminado pelo cliente 
    // termina só após o atendimento ter sido completado;


    // [ i, pid, tid, dur, pl ]

    do
    {
        fd = open(fifoname, O_WRONLY, 00222);
        if (fd == -1) // Se ainda não tiver sido criado pelo reader
            sleep(1);
    } while (fd == -1);

    printf("Got out of do while\n");

    char fifo_data[100];

    snprintf(fifo_data, sizeof(fifo_data), "[ %d, %d, %lu, %d, %d ]", 2, getpid(), pthread_self(), 3, 12);

    write(fd, fifo_data, sizeof(fifo_data)); 

    close(fd);
    
    return 0;
}