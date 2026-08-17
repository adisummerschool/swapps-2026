CC = gcc

all: main

libffunc.o: libfunc.c libfunc.h
	$(CC) -fPIC -c libfunc.c

libfunc.so: libfunc.o
	$(CC) -shared libfunc.o -o libfunc.so -liio

main: main.c libfunc.h libfunc.so
	$(CC) main.c -o main -L. -lfunc

run: main
	LD_LIBRARY_PATH=. ./main

clean:
	rm -rf libfunco libfunc.so main