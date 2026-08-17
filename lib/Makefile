CC = gcc

all:main

func.o: func.c func.h
	$(CC) -fPIC -c func.c

libfunc.so: libfunc.o
	$(CC) -shared libfunc.o -o libfunc.so -liio

main: main.c libfunc.h libfunc.so
	$(CC) main.c -o main -L. -lfunc

run: main
	LD_LIBRARY_PATH=. ./main

clean:
	rm -f libfunc.o libfunc.so main