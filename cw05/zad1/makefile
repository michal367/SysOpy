CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) main.c
	$(CC) main.o -o main

test:
	./main commands

clean:
	rm -f main
	rm -f *.o
	rm -f *.a
	rm -f *.so