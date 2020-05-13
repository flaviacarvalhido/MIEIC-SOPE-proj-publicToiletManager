all: U2 Q2

U2: U2.o register.o aux.o
	gcc -Wall -pthread U2.o register.o aux.o -o U2

Q2: Q2.o register.o aux.o
	gcc -Wall -pthread Q2.o register.o aux.o -o Q2

Q2.o: Q2.c
	gcc -Wall -pthread -c Q2.c

U2.o: U2.c
	gcc -Wall -pthread -c U2.c

register.o: register.c register.h
	gcc -Wall -pthread -c register.c

aux.o: aux.c aux.h
	gcc -Wall -pthread -c aux.c

clean:
	rm *.o U2 Q2