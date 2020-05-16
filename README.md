# Projeto 2 SOPE

No âmbito da unidade curricular, apresentamos o segundo projeto, um gestor de Quartos de Banho do tipo cliente-servidor, que funciona à base de pedidos do cliente para a casa de banho, e do processamento destes por esta mesmo.

## Usage Etapa 2

```bash
Q2 <-t nsecs> [-l nplaces] [-n nthreads] fifoname
U2 <-t nsecs> fifoname
```

## Detalhes da implementação

No momento, as features que permitem a utilização dos parâmetros opcionais [-l nplaces] e [-n nthreads] estão ativadas.

Um cliente pede sempre a utilização do quarto de banho entre 1 e 5 segundos, sendo que cada pedido é espaçado de 0.1 segundos.

Quando um cliente faz um pedido ao servidor, o quarto de banho que lhe fica atribuido é o primeiro que estiver livre num dado momento. Se o quarto de banho encerrar, os clientes esperam numa queue (de tamanho nplaces) até que possa ser atendido. Esta trata-se da nossa interpretação do enunciado do problema.

As mensagens são de acordo com o seguinte:
    - Um cliente pede a entrada (IWANT)
    - Um cliente entra (IAMIN) caso possa
    - Um cliente não entra (CLOSD), se não puder
    - Um cliente não consegue abrir o FIFO (GAVUP)
    - O quarto de banho recebe o pedido (RECVD)
    - O quarto de banho reconhece a entrada do cliente (ENTER)
    - O quarto de banho reconhece a saída do cliente (TIMUP)
    - O quarto de banho não deixa entrar mais ninguém devido ao encerramento (2LATE)
    - O quarto de banho encontra um problema a escrever para o FIFO público (FAILD)

## Collaborators

Flávia Carvalhido - up201806857@fe.up.pt\
João Gonçalves - up201806796@fe.up.pt\
Nuno Resende - up201806825@fe.up.pt

Turma 7 Grupo 6 - MIEIC 2019/20
