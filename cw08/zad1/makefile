CC=gcc
CFLAGS=-c -Wall

file=files/f14.ascii.pgm

standard:
	$(CC) $(CFLAGS) main.c
	$(CC) main.o -o main -lpthread

test_sign:
	./main 4 sign $(file) histogram_sign.txt

test_block:
	./main 4 block $(file) histogram_block.txt

test_interleaved:
	./main 4 interleaved $(file) histogram_interleaved.txt


test:
	# SIGN
	# 1 THREAD
	./main 1 sign $(file) histogram_sign.txt
	# 2 THREAD
	./main 2 sign $(file) histogram_sign.txt
	# 4 THREAD
	./main 4 sign $(file) histogram_sign.txt
	# 8 THREAD
	./main 8 sign $(file) histogram_sign.txt

	@echo
	@echo
	@echo
	# BLOCK
	# 1 THREAD
	./main 1 block $(file) histogram_block.txt
	# 2 THREAD
	./main 2 block $(file) histogram_block.txt
	# 4 THREAD
	./main 4 block $(file) histogram_block.txt
	# 8 THREAD
	./main 8 block $(file) histogram_block.txt

	@echo
	@echo
	@echo
	# INTERLEAVED
	# 1 THREAD
	./main 1 interleaved $(file) histogram_interleaved.txt
	# 2 THREAD
	./main 2 interleaved $(file) histogram_interleaved.txt
	# 4 THREAD
	./main 4 interleaved $(file) histogram_interleaved.txt
	# 8 THREAD
	./main 8 interleaved $(file) histogram_interleaved.txt


clean:
	rm -f main
	rm -f histogram_sign.txt
	rm -f histogram_block.txt
	rm -f histogram_interleaved.txt
	rm -f *.o
	rm -f *.a
	rm -f *.so