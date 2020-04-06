CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) producer.c
	$(CC) producer.o -o producer

	$(CC) $(CFLAGS) consumer.c
	$(CC) consumer.o -o consumer

	$(CC) $(CFLAGS) test.c
	$(CC) test.o -o test

clean:
	rm -f producer
	rm -f consumer
	rm -f test
	rm -f fifo
	rm -f *.o
	rm -f *.a
	rm -f *.so