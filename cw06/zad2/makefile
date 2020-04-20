CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) server.c
	$(CC) server.o -o server -lrt

	$(CC) $(CFLAGS) client.c
	$(CC) client.o -o client -lrt


# Test is system specific
# test_kde - KDE
# test_gnome - Gnome
# test_xterm - X system 

test:
	# use one of these:
	# test_kde
	# test_gnome
	# test_xterm

test_kde:
	# STARTING SERVER
	konsole --hold -e "./server" &
	sleep 0.5
	# STARTING 3 CLIENTS
	konsole --hold -e "./client" &
	konsole --hold -e "./client" &
	konsole --hold -e "./client" &

test_gnome:
	# STARTING SERVER
	gnome-terminal -- bash -c "./server; exec bash" &
	sleep 0.5
	# STARTING 3 CLIENTS
	gnome-terminal -- bash -c "./client; exec bash" &
	gnome-terminal -- bash -c "./client; exec bash" &
	gnome-terminal -- bash -c "./client; exec bash" &

test_xterm:
	# STARTING SERVER
	xterm -hold -e "./server" &
	sleep 0.5
	# STARTING 3 CLIENTS
	xterm -hold -e "./client" &
	xterm -hold -e "./client" &
	xterm -hold -e "./client" &



clean:
	rm -f server
	rm -f client
	rm -f *.o
	rm -f *.a
	rm -f *.so