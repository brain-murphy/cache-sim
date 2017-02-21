# Simple Cache Simulator
Simulates a 64-bit, byte-addressable, write-allocate, write-back cache with sub-blocking and a 
victim cache. Accepts SPEC benchmark traces as input to simulate. On a read or write, sub blocks
are fetched from the sub block of the access to the end of the block.

## Building
Run `make` to build the project. `make clean` removes built files.

## Running

### Args and Defaults

| Flag | Use | Default |
| ---- | --- | ------- |
| -C   | Cache size = 2^C | 15
| -B   | Bytes per cache block = 2^B | 5
| -S   | (set associativity) Blocks per set = 2^S | 3
| -K   | Bytes per sub block = 2^K | 3
| -V   | Size of victim cache in blocks = V | 4
| -i   | SPEC format input | stdin

### Examples
```
make clean
make
./cachesim -B 5 -V 4 -i traces/astar.trace
```

can also accept input from stdin:
```
./cachesim -B 5 -V 4 < traces/perlbench.trace
```

## Known bugs

* Issues reading from files and stdin on OSx. About 1 in 20,000 inputs will not be correctly read on these systems.
