CC=gcc
CFLAGS=-c -Wall


normal: lib.c
	$(CC) $(CFLAGS) lib.c
	
	
static:
	$(CC) $(CFLAGS) lib.c
	ar rcs lib.a lib.o
	

shared:
	$(CC) $(CFLAGS) -fPIC lib.c
	$(CC) -shared -fPIC -o liblib.so lib.o


	
clean:
	rm -f zad
	rm -f *.o
	rm -f *.a
	rm -f *.so