/*
Alumno:  Merenda, Franco N.
Carrera: Ing. en Computación

Sistemas Distribuidos y Paralelos
    TP N°6 - Ejercicio 1.d
*/

#include<stdio.h>
#include<mpi.h>
#include<stdlib.h>
#define BUFFER 50

int main(int argc, char **argv){
    int node;
    char data[BUFFER];
    
    //Init MPI_Library
    MPI_Init(&argc, &argv);
    
    //Set up process receiver
    int PROCESS_RECEIVER = atoi(argv[1]);
    
    //Get rank from node in their commnicator
    MPI_Comm_rank(MPI_COMM_WORLD, &node);
    
    if(node == PROCESS_RECEIVER){
        //Get Number of processes
        int size = 0;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        
        //Send data to itself 
        sprintf(data,"(Hola desde el proceso %d)\n", node);
        
        //Print the requested
        printf("Hola, soy el proceso %d ( hay %d procesos ) y recibo: %s", node, size, data);
        for(int i = 1; i < size; i++){
            MPI_Recv(data, BUFFER, MPI_CHAR, (PROCESS_RECEIVER + i)%size , MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s",data);       
        }
    }else{
        sprintf(data,"(Hola desde el proceso %d)\n", node);
        MPI_Send(data, BUFFER, MPI_CHAR, PROCESS_RECEIVER, 0, MPI_COMM_WORLD);
    }

    //Finalize MPI
    MPI_Finalize();
}