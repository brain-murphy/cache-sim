CC=gcc
CFLAGS=-std=c99 -g
DEPS = cachesim.h cache.h
OBJ = cachesim.o cache.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cachesim: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o cachesim
