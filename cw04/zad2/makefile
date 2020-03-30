CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) main.c
	$(CC) main.o -o main

	$(CC) $(CFLAGS) main_exec.c
	$(CC) main_exec.o -o main_exec


test:
	# FORK
	# IGNORE
	./main fork ignore
	@echo
	# HANDLER
	./main fork handler
	@echo
	# MASK
	./main fork mask
	@echo
	# PENDING
	./main fork pending

	@echo
	@echo
	@echo
	# EXEC
	# IGNORE
	./main exec ignore
	@echo
	# MASK
	./main exec mask
	@echo
	# PENDING
	./main exec pending


clean:
	rm -f main
	rm -f main_exec
	rm -f *.o
	rm -f *.a
	rm -f *.so