CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) server.c
	$(CC) server.o -o server -lpthread 
	$(CC) $(CFLAGS) client.c
	$(CC) client.o -o client -lpthread 
	

# Test is system specific
# test_kde - KDE
# test_gnome - Gnome
# test_xterm - X system 

test:
	# use one of these:
	# test_kde
	# test_gnome
	# test_xterm
	# in case of some program errors on client disconnection dont use make test

test_kde:
	# STARTING SERVER
	konsole --hold -e "./server 3000 unix_socket" &
	sleep 1
	# STARTING CLIENTS
	konsole --hold -e "./client user1 UNIX unix_socket" &
	konsole --hold -e "./client user2 UNIX unix_socket" &
	konsole --hold -e "./client user3 INET 127.0.0.1:3000" &
	sleep 5
	konsole --hold -e "./client user4 INET 127.0.0.1:3000" &

test_gnome:
	# STARTING SERVER
	gnome-terminal -- bash -c "./server 3000 unix_socket; exec bash" &
	sleep 1
	# STARTING 3 CLIENTS
	gnome-terminal -- bash -c "./client user1 UNIX unix_socket; exec bash" &
	gnome-terminal -- bash -c "./client user2 UNIX unix_socket; exec bash" &
	gnome-terminal -- bash -c "./client user3 INET 127.0.0.1:3000; exec bash" &
	sleep 5
	gnome-terminal -- bash -c "./client user4 INET 127.0.0.1:3000; exec bash" &

test_xterm:
	# STARTING SERVER
	xterm -hold -e "./server 3000 unix_socket" &
	sleep 1
	# STARTING 3 CLIENTS
	xterm -hold -e "./client user1 UNIX unix_socket" &
	xterm -hold -e "./client user2 UNIX unix_socket" &
	xterm -hold -e "./client user3 INET 127.0.0.1:3000" &
	sleep 5
	xterm -hold -e "./client user4 INET 127.0.0.1:3000" &


clean:
	rm -f server
	rm -f client
	rm -f unix_socket
	rm -f *.o
	rm -f *.a
	rm -f *.so