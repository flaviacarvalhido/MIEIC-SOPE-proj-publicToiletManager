all: U1 Q1

U1: U1.o register.o aux.o
	gcc -Wall -pthread U1.o register.o aux.o -o U1

Q1: Q1.o register.o aux.o
	gcc -Wall -pthread Q1.o register.o aux.o -o Q1

Q1.o: Q1.c
	gcc -Wall -pthread -c Q1.c

U1.o: U1.c
	gcc -Wall -pthread -c U1.c

register.o: register.c register.h
	gcc -Wall -pthread -c register.c

aux.o: aux.c aux.h
	gcc -Wall -pthread -c aux.c

clean:
	rm *.o U1 Q1