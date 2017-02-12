CC=gcc
CFLAGS=-std=c99 -g
DEPS = cachesim.h cache.h vc.h
OBJ = cachesim.o cache.o vc.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cachesim: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

clean:
	rm -f *.o cachesim
