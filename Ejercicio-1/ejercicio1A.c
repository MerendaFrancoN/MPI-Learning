/*
Alumno:  Merenda, Franco N.
Carrera: Ing. en Computación

Sistemas Distribuidos y Paralelos
    TP N°6 - Ejercicio 1.a
*/

#include<stdio.h>
#include<mpi.h>

int main(int argc, char **argv){
    int node;
    
    //Init MPI_Library
    MPI_Init(&argc, &argv);

    //Set communicator which will belong process ( MPI_COMM_WORLD)
    MPI_Comm_rank(MPI_COMM_WORLD, &node);

    printf("Hello World from Node %d\n", node);

    //Finalize MPI
    MPI_Finalize();
}