# Projeto 2 SOPE

No âmbito da unidade curricular, apresentamos o segundo projeto, um gestor de Quartos de Banho do tipo cliente-servidor, que funciona à base de pedidos do cliente para a casa de banho, e do processamento destes por esta mesmo.

## Usage Etapa 1

```bash
Q1 <-t nsecs> fifoname
U1 <-t nsecs> fifoname
```

## Detalhes da implementação

No momento, as features que permitem a utilização dos parâmetros opcionais [-l nplaces] e [-n nthreads] estão desativadas (Etapa 2 do projeto).

Um cliente pede sempre a utilização do quarto de banho entre 1 e 5 segundos, sendo que cada pedido é espaçado de 0.3 segundos.

Quando um cliente faz um pedido ao servidor, o quarto de banho que lhe fica atribuido é o primeiro que estiver livre num dado momento.

Todas as mensagens de registo foram implementadas e a comunicação entre os FIFOs conforme o padrão pedido.

## Collaborators

Flávia Carvalhido - up201806857@fe.up.pt\
João Gonçalves - up201806796@fe.up.pt\
Nuno Resende - up201806825@fe.up.pt


Turma 7 Grupo 6 - MIEIC 2019/20
