CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) matrix.c
	$(CC) $(CFLAGS) functions.c

	$(CC) $(CFLAGS) macierz.c
	$(CC) $(CFLAGS) generator.c
	$(CC) $(CFLAGS) tester.c
	
	$(CC) macierz.o matrix.o functions.o -o macierz
	$(CC) generator.o matrix.o -o generator
	$(CC) tester.o matrix.o -o tester

clean:
	rm -f macierz
	rm -f generator
	rm -f tester
	rm -f *.o
	rm -f *.a
	rm -f *.so
