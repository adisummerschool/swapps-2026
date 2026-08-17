CC = gcc

all: main

libad5592r.o: libad5592r.c libad5592r.h
	$(CC) -fPIC -c libad5592r.c

libad5592r.so: libad5592r.o
	$(CC) -shared libad5592r.o -o libad5592r.so -liio

main: main.c libad5592r.h libad5592r.so
	$(CC) main.c -o main -L. -lad5592r

run: main
	LD_LIBRARY_PATH=. ./main $(ARGS)

clean:
	rm -f libad5592r.o libad5592r.so main