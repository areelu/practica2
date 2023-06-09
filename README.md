## Practice 2


## Description

High Performance Computing Class Practice 2

Create a filter that performs a color transformation that works in parallel with MPICH.

## Execution

Clone the repository:


```text
git clone https://github.com/areelu/practica2.git
```
Compile the rojo.c file:

```text
mpicc \-o filter rojo.c -lm
```
Run the compiled file:

```text
mpiexec -n 4 ./filter "flor.png"
```
If all goes well, it will give us the following results:

<img src='photo3.png'>

In the folder where the files are executed, an image with the filter applied will be created. In this case 'flor_rojos.png'.

<img src='photo7.png'>

## Results

<img src='photo6.png'>
<img src='photo5.png'>


# Requirements:
mpi 
spng

