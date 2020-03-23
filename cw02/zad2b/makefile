CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) find.c
	$(CC) $(CFLAGS) functions.c
	$(CC) find.o functions.o -o find


clean:
	rm -f find
	rm -f *.o
	rm -f *.a
	rm -f *.so
