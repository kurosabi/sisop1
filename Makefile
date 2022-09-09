CC=gcc

%.o: %.c
	$(CC) -c -o $@ $<

proyectosis1: proyectosis1.o
	gcc -o proyectosis1 proyectosis1.o
Ma

tar:
	tar czvf c_language_basics.tgz *.c 
	
clean:
	rm -f *.o proyectosis1