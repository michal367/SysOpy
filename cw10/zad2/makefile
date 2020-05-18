CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) server.c
	$(CC) server.o -o server -lpthread 
	$(CC) $(CFLAGS) client.c
	$(CC) client.o -o client -lpthread 
	

# there is no tests like in zad1, because then clients won't work

test_server:
	./server 3000 unix_socket

test_client1_unix:
	./client user1u UNIX unix_socket
test_client2_unix:
	./client user2u UNIX unix_socket

test_client1_inet:
	./client user1i INET 127.0.0.1:3000
test_client2_inet:
	./client user2i INET 127.0.0.1:3000


clean:
	rm -f server
	rm -f client
	rm -f unix_socket
	rm -f *.o
	rm -f *.a
	rm -f *.so