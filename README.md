# Práctica2

- Arely Hilda Luis Tiburcio

## Descripción

Práctica 2 de la clase de computo de alto desempeño

Hacer un anillo de n procesadores que haga la suma comulativa de un valor inicial x0 para m ciclos sobre el anillo.

## Ejecución

Clonar el repositorio:

```text
git clone https://github.com/areelu/practica2.git
```
Compilar el archivo test2.c:

```text
mpicc test2.c -o test2
```
Correr el archivo compilado:

```text
mpirun -np 10 ./test2
```

Este programa requiere tener instalado mpi.
