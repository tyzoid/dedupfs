CC=gcc
CFLAGS=--std=c99 -g3

all: fuse_layer.o main.o
	gcc $(CFLAGS) -lfuse fuse_layer.o main.o -o dedupfs

clean:
	rm -vf *.o dedupfs
