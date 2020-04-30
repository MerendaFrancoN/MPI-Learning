/*
Alumno:  Merenda, Franco N.
Carrera: Ing. en Computación

Sistemas Distribuidos y Paralelos
    TP N°6 - Ejercicio 2 -- Funciona siempre y cuando tamaño matriz <= cantidad de procesadores disponibles.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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

    int *vectorFromMatrix, *vectorToMultiply;
    int dataProcessed[1];
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
        vectorToMultiply = allocateVector(matrixSize);
        vectorFromMatrix = allocateVector(matrixSize);

        //Fill vectors and matrix
        fillMatrix(matrix, matrixSize, matrixSize);
        fillVector(vectorToMultiply,matrixSize);
        
        
        //Assign vector and data to every worker process
        memcpy(vectorFromMatrix, matrix[0], matrixSize*sizeof(int)); //Processor 0

        for(int processorNumber = 1; processorNumber < matrixSize ; processorNumber++){

            //Send Data
            MPI_Send(vectorToMultiply, matrixSize, MPI_INT, processorNumber, TAGVECTOR, MPI_COMM_WORLD);
            MPI_Send(matrix[processorNumber], matrixSize, MPI_INT, processorNumber, TAGMATRIX, MPI_COMM_WORLD);
        }
   

    }else{
        
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
            dataProcessed[0] = 0;
                
            for(int rowIndex = 0; rowIndex < matrixSize; rowIndex++){
                dataProcessed[0] += vectorFromMatrix[rowIndex] * vectorToMultiply[rowIndex];
            }
            //Send back result to master
            MPI_Send(&dataProcessed, 1, MPI_INT, MASTER, TAGRESPONSE, MPI_COMM_WORLD);

            //Free memory
            free(vectorFromMatrix);
            free(vectorToMultiply);
        }
    }

    /*MASTER PROCESSOR*/
    if(node == MASTER){
        
        //Allocate Results
        int *finalResult = allocateVector(matrixSize);

        //Process MASTER
        finalResult[0] = 0;
        for(int rowIndex = 0; rowIndex < matrixSize; rowIndex++){
            finalResult[0] += vectorFromMatrix[rowIndex] * vectorToMultiply[rowIndex];
        }

        //Gather all rest of the processed data
        for(int i = 1; i < matrixSize; i++){
            //Gather all the results
            MPI_Recv(&dataProcessed, 1, MPI_INT, i, TAGRESPONSE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            //Assign data where it belongs
            finalResult[i] = dataProcessed[0];
        }
        
        //Print final output
        printVector(finalResult,matrixSize);

        //Free
        free(finalResult);
        free(vectorFromMatrix);
        free(vectorToMultiply);
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