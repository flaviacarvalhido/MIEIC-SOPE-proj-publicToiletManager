main: main.o parser.o register.o
	gcc main.o parser.o register.o -pthread -Wall -o main

main.o: main.c
	gcc -c main.c

register.o: register.c register.h
	gcc -c register.c

parser.o: parser.c parser.h
	gcc -c parser.c




clean:
	rm *.o main
