/*
Alumno:  Merenda, Franco N.
Carrera: Ing. en Computación

Sistemas Distribuidos y Paralelos
    TP N°6 - Ejercicio 2
*/

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

#define BUFFER 2
#define PROCESS_ROOT 0 //Set the process number that will receive the data, values (0 - (np - 1 )), np = number of processes.

int main(int argc, char **argv){
    int node, size, previousNode, nextNode, numberLap;
    char data[BUFFER];
    //Init MPI_Library
    MPI_Init(&argc, &argv);

    //Get number of laps from args
    numberLap = atoi(argv[1]);

    //Get rank from node in their commnicator
    MPI_Comm_rank(MPI_COMM_WORLD, &node);
    
    //Get Number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Set previous and next Node from ring
    previousNode = (node + size - 1 ) % size;
    nextNode = (node + 1) % size;
    
    if(node == PROCESS_ROOT){
        printf("Starting ring from node %d\n", node);
        sprintf(data, "A");
        //Send data to next node in ring
        MPI_Send(data, BUFFER, MPI_CHAR, nextNode, 0, MPI_COMM_WORLD);
    }   

    for(int i = 0; i < numberLap; i++){
        //Receive from previous
        MPI_Recv(data, BUFFER, MPI_CHAR, previousNode, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        if(node == PROCESS_ROOT){
            printf("Data reached origin with = %s, lap number = %d\n",data, i);
        }

        //Pass the data to next node
        MPI_Send(data, BUFFER, MPI_CHAR, nextNode, 0, MPI_COMM_WORLD);
    } 

    //Finalize MPI
    MPI_Finalize();
}
