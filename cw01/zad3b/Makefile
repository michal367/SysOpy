CC=gcc
CFLAGS=-c -Wall

normal: zad

zad: zad.o lib.o
	$(CC) zad.o lib.o -o zad
	
zad.o: zad.c
	$(CC) $(CFLAGS) zad.c

lib.o: lib.c
	$(CC) $(CFLAGS) lib.c
	
	
static:
	$(CC) $(CFLAGS) lib.c
	ar rcs lib.a lib.o
	$(CC) $(CFLAGS) zad.c
	$(CC) zad.o lib.a -o zad
	

shared:
	$(CC) $(CFLAGS) -fPIC lib.c
	$(CC) -shared -fPIC -o liblib.so lib.o
	$(CC) -L"`pwd`" -Wl,-rpath="`pwd`" -Wall -o zad zad.c -llib

dynamic:
	$(CC) $(CFLAGS) -fPIC lib.c
	$(CC) -shared -fPIC -o liblib.so lib.o
	$(CC) -fPIC -L"`pwd`" -Wl,-rpath="`pwd`" -Wall -o zad zad_dynamic.c -ldl
	
clean:
	rm -f zad
	rm -f *.o
	rm -f *.a
	rm -f *.so


zad3b_static:
	make clean
	$(CC) $(CFLAGS) lib.c
	ar rcs lib.a lib.o
	$(CC) $(CFLAGS) zad.c
	$(CC) zad.o lib.a -o zad -O${level}
	make test > results3b.txt

zad3b_shared:
	make clean
	$(CC) $(CFLAGS) -fPIC lib.c
	$(CC) -shared -fPIC -o liblib.so lib.o
	$(CC) -L"`pwd`" -Wl,-rpath="`pwd`" -Wall -o zad zad.c -llib -O${level}
	make test > results3b.txt

zad3b_dynamic:
	make clean
	$(CC) $(CFLAGS) -fPIC lib.c
	$(CC) -shared -fPIC -o liblib.so lib.o
	$(CC) -fPIC -L"`pwd`" -Wl,-rpath="`pwd`" -Wall -o zad zad_dynamic.c -ldl -O${level}
	make test > results3b.txt
	

zad3b_0:
	make zad3b level=0
zad3b_1:
	make zad3b level=1
zad3b_2:
	make zad3b level=2
zad3b_3:
	make zad3b level=3
zad3b_s:
	make zad3b level=s

zad3b:
	make clean
	$(CC) $(CFLAGS) lib.c
	ar rcs lib.a lib.o
	$(CC) $(CFLAGS) zad.c
	$(CC) zad.o lib.a -o zad -O${level}
	@echo STATIC: > results3b.txt
	make test >> results3b.txt

	make clean
	$(CC) $(CFLAGS) -fPIC lib.c
	$(CC) -shared -fPIC -o liblib.so lib.o
	$(CC) -L"`pwd`" -Wl,-rpath="`pwd`" -Wall -o zad zad.c -llib -O${level}
	@echo -e "\n\n\n" >> results3b.txt
	@echo SHARED: >> results3b.txt
	make test >> results3b.txt

	make clean
	$(CC) $(CFLAGS) -fPIC lib.c
	$(CC) -shared -fPIC -o liblib.so lib.o
	$(CC) -fPIC -L"`pwd`" -Wl,-rpath="`pwd`" -Wall -o zad zad_dynamic.c -ldl -O${level}
	@echo -e "\n\n\n" >> results3b.txt
	@echo DYNAMIC: >> results3b.txt
	make test >> results3b.txt



test:
	@echo JEDNA PARA

	@echo
	@echo PODOBNE
	./zad create_table 1 compare_pairs pliki/podobne/a.txt:pliki/podobne/b.txt remove_operation 0 0 remove_block 0
	
	@echo
	@echo PRAWIE PODOBNE
	./zad create_table 1 compare_pairs pliki/prawie_podobne/a.txt:pliki/prawie_podobne/b.txt remove_operation 0 0 remove_block 0
	
	@echo
	@echo NIEPODOBNE
	./zad create_table 1 compare_pairs pliki/niepodobne/a.txt:pliki/niepodobne/b.txt remove_operation 0 0 remove_block 0


	@echo
	@echo
	@echo TRZY PARY

	@echo
	@echo PODOBNE
	./zad create_table 3 compare_pairs pliki/podobne/a.txt:pliki/podobne/b.txt pliki/podobne/c.txt:pliki/podobne/d.txt pliki/podobne/e.txt:pliki/podobne/f.txt remove_operation 0 0 remove_operation 0 1 remove_block 0 remove_block 0
	
	@echo
	@echo PRAWIE PODOBNE
	./zad create_table 3 compare_pairs pliki/prawie_podobne/a.txt:pliki/prawie_podobne/b.txt pliki/prawie_podobne/c.txt:pliki/prawie_podobne/d.txt pliki/prawie_podobne/e.txt:pliki/prawie_podobne/f.txt  remove_operation 0 0 remove_operation 0 1 remove_block 0 remove_block 0
	
	@echo
	@echo NIEPODOBNE
	./zad create_table 3 compare_pairs pliki/niepodobne/a.txt:pliki/niepodobne/b.txt pliki/niepodobne/c.txt:pliki/niepodobne/d.txt pliki/niepodobne/e.txt:pliki/niepodobne/f.txt remove_operation 0 0 remove_operation 0 1 remove_block 0 remove_block 0


	@echo
	@echo
	@echo PIĘĆ PAR

	@echo
	@echo PODOBNE
	./zad create_table 5 compare_pairs pliki/podobne/a.txt:pliki/podobne/b.txt pliki/podobne/c.txt:pliki/podobne/d.txt pliki/podobne/e.txt:pliki/podobne/f.txt pliki/podobne/g.txt:pliki/podobne/h.txt pliki/podobne/i.txt:pliki/podobne/j.txt remove_operation 0 0 remove_operation 0 1 remove_block 1 remove_block 0 remove_block 0
	
	@echo
	@echo PRAWIE PODOBNE
	./zad create_table 5 compare_pairs pliki/prawie_podobne/a.txt:pliki/prawie_podobne/b.txt pliki/prawie_podobne/c.txt:pliki/prawie_podobne/d.txt pliki/prawie_podobne/e.txt:pliki/prawie_podobne/f.txt pliki/prawie_podobne/g.txt:pliki/prawie_podobne/h.txt pliki/prawie_podobne/i.txt:pliki/prawie_podobne/j.txt remove_operation 0 0 remove_operation 0 1 remove_block 1 remove_block 0 remove_block 0
	
	@echo
	@echo NIEPODOBNE
	./zad create_table 5 compare_pairs pliki/niepodobne/a.txt:pliki/niepodobne/b.txt pliki/niepodobne/c.txt:pliki/niepodobne/d.txt pliki/niepodobne/e.txt:pliki/niepodobne/f.txt pliki/niepodobne/g.txt:pliki/niepodobne/h.txt pliki/niepodobne/i.txt:pliki/niepodobne/j.txt remove_operation 0 0 remove_operation 0 1 remove_block 1 remove_block 0 remove_block 0