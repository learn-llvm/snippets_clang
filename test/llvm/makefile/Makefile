all: min.out

min.out: main.o min.o
	$(CC) main.o min.o -o min.out

main.o:
	$(CC) main.c -c -o $@ 

min.o:
	$(CC) min.c -c -o $@

clean:
	rm -f *.o *.out

rebuild: clean min.out
