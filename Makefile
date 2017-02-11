CC=gcc
CFLAGS=-std=c99
DEPS = cachesim.h
OBJ = cachesim.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cachesim: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)
