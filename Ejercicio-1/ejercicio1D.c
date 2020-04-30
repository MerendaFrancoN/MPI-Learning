/*
Alumno:  Merenda, Franco N.
Carrera: Ing. en Computación

Sistemas Distribuidos y Paralelos
    TP N°6 - Ejercicio 1.d
*/

#include<stdio.h>
#include<mpi.h>

#define BUFFER 50
#define PROCESS_RECEIVER 3 //Set the process number that will receive the data, values (0 - (np - 1 )), np = number of processes.

int main(int argc, char **argv){
    int node;
    char data[BUFFER];
    //Init MPI_Library
    MPI_Init(&argc, &argv);

    //Get rank from node in their commnicator
    MPI_Comm_rank(MPI_COMM_WORLD, &node);
    
    if(node == PROCESS_RECEIVER){
        //Get Number of processes
        int size = 0;
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        
        //Send data to itself 
        sprintf(data,"(Hola desde el proceso %d)\n", node);
        
        // MPI_Send(buffer,count,type,dest,tag,comm)  -- tag has to match in receiver and sender, however,
        // One could set any tag (positive integer to identify a message), and in receiver set MPI_ANY_TAG to be able 
        // to receive any kind.
        MPI_Send(data, BUFFER, MPI_CHAR, PROCESS_RECEIVER, 0, MPI_COMM_WORLD);
        
        //Print the requested
        printf("Hola, soy el proceso %d ( hay %d procesos ) y recibo: ", node, size);
        for(int i = 0; i < size; i++){            
            MPI_Recv(data, BUFFER, MPI_CHAR, i, MPI_ANY_TAG, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%s",data);
        }
    }else{
        sprintf(data,"(Hola desde el proceso %d)\n", node);
        MPI_Send(data, BUFFER, MPI_CHAR, PROCESS_RECEIVER, 0, MPI_COMM_WORLD);
    }

    //Finalize MPI
    MPI_Finalize();
}