all: U1 Q1

U1: U1.o parser.o register.o aux.o
	gcc -Wall -pthread U1.o parser.o register.o aux.o -o U1

Q1: Q1.o parser.o register.o aux.o
	gcc -Wall -pthread Q1.o parser.o register.o aux.o -o Q1

Q1.o: Q1.c
	gcc -c Q1.c

U1.o: U1.c
	gcc -c U1.c

register.o: register.c register.h
	gcc -c register.c

parser.o: parser.c parser.h
	gcc -c parser.c

aux.o: aux.c aux.h
	gcc -c aux.c

clean:
	rm *.o U1 Q1