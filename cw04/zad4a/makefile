CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) sender.c
	$(CC) sender.o -o sender
	
	$(CC) $(CFLAGS) catcher.c
	$(CC) catcher.o -o catcher


test_kill:
	make test mode=kill
test_sigqueue:
	make test mode=sigqueue
test_sigrt:
	make test mode=sigrt

test:
	./catcher & pid=$$!; \
	#echo $$pid; \
	echo; \
	sleep 1; \
	./sender $$pid 100 $(mode);


clean:
	rm -f sender
	rm -f catcher
	rm -f *.o
	rm -f *.a
	rm -f *.so
