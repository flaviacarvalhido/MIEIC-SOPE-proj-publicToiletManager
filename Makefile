all: U1 Q1

U1: U1.o parser.o register.o
	gcc U1.o parser.o register.o -pthread -Wall -o U1

Q1: Q1.o parser.o register.o
	gcc Q1.o parser.o register.o -pthread -Wall -o Q1

Q1.o: Q1.c
	gcc -c Q1.c

U1.o: U1.c
	gcc -c U1.c

register.o: register.c register.h
	gcc -c register.c

parser.o: parser.c parser.h
	gcc -c parser.c


clean:
	rm *.o U1 Q1