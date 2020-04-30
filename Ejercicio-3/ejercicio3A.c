/*
Alumno:  Merenda, Franco N.
Carrera: Ing. en Computación

Sistemas Distribuidos y Paralelos
    TP N°6 - Ejercicio 2 -- Funciona siempre y cuando tamaño matriz <= cantidad de procesadores disponibles.
*/

#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>

//Set process Master
#define MASTER 0

//Tags
#define TAGVECTOR 0
#define TAGMATRIX 1
#define TAGRESPONSE  2

//Allocations
int** allocate2DMatrix(int rows, int columns);
int* allocateVector(int dimension);

//Utils
void printMatrix(int** matrixToPrint, int rows, int columns);
void printVector(int* vector, int dimension);
void fillMatrix(int** matrixToFill, int rows, int columns);
void fillVector(int* vector, int dimension);

int main(int argc, char **argv){
    int node = 0, communicatorSize, matrixSize;
    int *vectorToMultiply, data, **matrixPortion, *finalResult ;
    
    //Init MPI_Library
    MPI_Init(&argc, &argv);

    //Get number of laps from args
    matrixSize = atoi(argv[1]);

    //Get rank from node in their commnicator
    MPI_Comm_rank(MPI_COMM_WORLD, &node);
    
    //Get Number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &communicatorSize);

    /*MASTER PROCESSOR*/
    if(node == MASTER){
        /*Generate Matrix and vector*/
        
        //Allocate memory
        int **matrix = allocate2DMatrix(matrixSize, matrixSize);
        int *vector = allocateVector(matrixSize);
         
        //Fill vectors and matrix
        fillMatrix(matrix, matrixSize, matrixSize);
        fillVector(vector,matrixSize);

        //Assign vector and data to every worker process
        for(int processorNumber = 0; processorNumber < matrixSize ; processorNumber++){

            //Send Data
            MPI_Send(vector, matrixSize, MPI_INT, processorNumber, TAGVECTOR, MPI_COMM_WORLD);
            MPI_Send(matrix[processorNumber], matrixSize, MPI_INT, processorNumber, TAGMATRIX, MPI_COMM_WORLD);
        }   

    }

    /*WORKER and MASTER PROCESSOR*/
    if(node < matrixSize){ //Condition in case that were it would be more processes than rows.
        //Vectors
        vectorToMultiply = allocateVector(matrixSize);
        vectorFromMatrix = allocateVector(matrixSize);

        //Receive vector
        MPI_Recv(vectorToMultiply, matrixSize, MPI_INT, MASTER, TAGVECTOR, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        //Receive row to process
        MPI_Recv(vectorFromMatrix, matrixSize, MPI_INT, MASTER, TAGMATRIX, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        
        //Process
        for(int i = 0; i < matrixSize; i++){
            dataProcessed[0] += vectorFromMatrix[i] * vectorToMultiply[i];
        }
        //Send back result to master
        MPI_Send(dataProcessed, 1, MPI_INT, MASTER, TAGRESPONSE, MPI_COMM_WORLD);
    }

       
    
    /*MASTER PROCESSOR*/
    if(node == MASTER){
        int *finalResult = allocateVector(matrixSize);
        
        //Gather all processed data
        for(int i = 0; i < matrixSize; i++){
            //Gather all the results
            MPI_Recv(dataProcessed, 1, MPI_INT, i, TAGRESPONSE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //Assign data where it belongs
            finalResult[i] = dataProcessed[0];
        }
        
        //Print final output
        printVector(finalResult,matrixSize);
    }

    //Finalize MPI
    MPI_Finalize();
}

int** allocate2DMatrix(int rows, int columns){
    int **matrix = (int **)malloc(rows * sizeof(int *)); 
    for (int i = 0; i < rows; i++) 
        matrix[i] = (int *)malloc(columns * sizeof(int));
    
    return matrix;
}

int* allocateVector(int dimension){
    return (int*)malloc(dimension * sizeof(int));
}

void fillMatrix(int** matrixToFill, int rows, int columns){
    int dataToCharge = 1;
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            matrixToFill[i][j] = dataToCharge++;
        }
    }
}

void fillVector(int* vector, int dimension){
    int dataToCharge = 1;
    for(int i = 0; i < dimension; i++){
        vector[i] = dataToCharge++;
    }
}

void printMatrix(int** matrixToPrint, int rows, int columns){
    printf("[\n");
    for(int i = 0; i < rows; i++){
        for(int j = 0; j < columns; j++){
            printf("%d ",matrixToPrint[i][j]);
        }
        printf("\n");
    }
    printf("]");
}

void printVector(int* vector, int dimension){
    printf("[\n");
    for(int i = 0; i < dimension; i++){
        printf("%d\n",vector[i]);
    }
    printf("]");
}