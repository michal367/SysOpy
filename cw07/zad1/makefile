CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) main.c
	$(CC) main.o -o main
	$(CC) $(CFLAGS) worker.c
	$(CC) worker.o -o worker

test:
	./main 5 5 5

clean:
	rm -f main
	rm -f worker
	rm -f logs
	rm -f *.o
	rm -f *.a
	rm -f *.so