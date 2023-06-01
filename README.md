## Practice 2


## Description

High Performance Computing Class Practice 2

Create a filter that performs a color transformation that works in parallel with MPICH.

## Execution

Clone the repository:

```text
git clone https://github.com/areelu/practica2.git
```
Compile the test2.c file:

```text
mpicc -o test2 test2.c -lspng
```
Compile the test2.c file:

```text
mpirun -np 4 ./test2
```

This program requires mpi to be installed.
