main: main.o 
	gcc main.o -pthread -Wall -o main

main.o: main.c
	gcc -c main.c



clean:
	rm *.o main
