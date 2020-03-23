CC=gcc
CFLAGS=-c -Wall

standard:
	$(CC) $(CFLAGS) program.c
	$(CC) $(CFLAGS) functions.c
	$(CC) program.o functions.o -o program


clean:
	rm -f program
	rm -f *.o
	rm -f *.a
	rm -f *.so



test_file=test.txt

test_single:
	@echo "$(length)-byte length, $(number) lines" >> $(test_file)

	./program generate plik $(number) $(length) 
	./program copy plik plik_sys $(number) $(length) sys >> $(test_file)
	./program copy plik plik_lib $(number) $(length) lib >> $(test_file)

	./program sort plik_sys $(number) $(length) sys >> $(test_file)
	./program sort plik_lib $(number) $(length) lib >> $(test_file)

	@echo >> $(test_file)

test:
	echo > $(test_file)

	# 1
	make test_single number=10000 length=1
	make test_single number=20000 length=1

	# 4
	make test_single number=20000 length=4
	make test_single number=40000 length=4

	# 512
	make test_single number=20000 length=512
	make test_single number=40000 length=512

	# 1024
	make test_single number=20000 length=1024
	make test_single number=40000 length=1024

	# 4096
	make test_single number=10000 length=4096
	make test_single number=20000 length=4096

	# 8192
	make test_single number=7000 length=8192
	make test_single number=14000 length=8192



