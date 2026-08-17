cc = gcc

all: main

libfunc.o: libfunc.c libfunc.h
	$(cc) -fPIC -c libfunc.c

libfunc.so: libfunc.o
	$(cc) -shared libfunc.o -o libfunc.so -liio

main: main.c libfunc.h libfunc.so
	$(cc) main.c -o main -L. -lfunc

run: main
	LD_LIBRARY_PATH=. ./main

clean: 
	rm -f libfunc.o libfunc.so main