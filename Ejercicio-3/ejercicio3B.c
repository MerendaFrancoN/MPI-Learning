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
#define TAGRESPONSE 2

//Resource Manager
void resourceManager(int* matrix, int columns, int* vector, int vectorSize, int processors, int portionRows, int deallocatedRowsSize);
int* resultsManager(int totalRows, int columns, int minPortionRows, int processors, int deallocatedRowsSize);

//Allocations
int* allocate1D2DMatrix(int rows, int columns);

//Utils
void printMatrix(int* matrixToPrint, int rows, int columns);
void fillMatrix(int* matrixToFill, int rows, int columns);


int main(int argc, char **argv){
    int node = 0, size, matrixSize;
    int *dataProcessed, *vectorToMultiply, *matrixPortion;
    
    //Variables about number of rows vs processors
    int isExtraPortion = 0;     //Variable to know if its a bigger portion or normal sized.
    int portionRows = 0;            //To know how big will be what we will send.
    int minPortionRows = 0;
    int count = 0;              //Count of items received

    //Init MPI_Library
    MPI_Init(&argc, &argv);

    //Get number of laps from args
    matrixSize = atoi(argv[1]);

    //Get rank from node in their commnicator
    MPI_Comm_rank(MPI_COMM_WORLD, &node);
    
    //Get Number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    /* INFO ABOUT MATRIX PORTIONS*/

    //Get Info about number of rows vs processors (Resource manager will need this)
    minPortionRows = matrixSize / size; //To know how big will be what we will send.

    // Info about if its a bigger or normal size portion of a matrix.
    // Cause the program knows that the master will send in order, and the biggers will be in the first deallocatedRowsSize processors,
    // So we can deduce beforehand if it will be bigger or not.
    isExtraPortion = ( ((matrixSize % size) - (node) ) <= 0); //(matrixSize % size) == number of rows unallocated
    
    //Update actual PortionRows
    portionRows =  isExtraPortion ? minPortionRows : minPortionRows + 1;  
    
    //Number of items to receive
    count = portionRows * matrixSize;

    
    
    
    /*MASTER PROCESSOR*/
    if(node == MASTER){
        /*Generate Matrix and vector*/
        
        //Allocate memory
        int *matrix = allocate1D2DMatrix(matrixSize, matrixSize);
        int *vector = allocate1D2DMatrix(matrixSize, matrixSize);
         
        //Fill vectors and matrix
        fillMatrix(matrix, matrixSize, matrixSize);
        fillMatrix(vector, 1 ,matrixSize);
        
        //Assign resources to process
        resourceManager(matrix, matrixSize, vector, matrixSize, size, minPortionRows, matrixSize % size );

        //Free matrix
        free(matrix);
        free(vector);
    }
    
    
    //-------------------------------------//
    
    /*WORKER PROCESSOR*/
    
    //Allocate Vectors
    vectorToMultiply = allocate1D2DMatrix(1, matrixSize);
    matrixPortion = allocate1D2DMatrix(portionRows, matrixSize);
    dataProcessed = allocate1D2DMatrix(portionRows, 1); // mxn nxz ==> m x z

    //Receive row to process
    MPI_Recv(matrixPortion, count, MPI_INT, MASTER, TAGMATRIX, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    //Receive vector
    MPI_Recv(vectorToMultiply, matrixSize, MPI_INT, MASTER, TAGVECTOR, MPI_COMM_WORLD, MPI_STATUS_IGNORE);   

    //Process Data
    for(int i = 0; i < portionRows; i++){
        dataProcessed[i] = 0;
        for(int j = 0; j < matrixSize; j++){
            dataProcessed[i] += vectorToMultiply[j] * matrixPortion[i*matrixSize + j];
        }
        
    }

    //Send back result to master
    MPI_Send(dataProcessed, portionRows, MPI_INT, MASTER, TAGRESPONSE, MPI_COMM_WORLD);
    
    //Free memory
    free(vectorToMultiply);
    free(matrixPortion);
    free(dataProcessed);

    /*FINAL PART OF WORKER*/

    //-------------------------------------//

    /*MASTER PROCESSOR -- GATHER ALL THE DATA*/
    if(node == MASTER){
        int *finalResult = resultsManager(matrixSize, matrixSize, matrixSize / size, size, matrixSize % size );
        
        //Print Results
        printMatrix(finalResult, matrixSize, 1);
        
        //Free finalResult
        free(finalResult);
    }

    MPI_Finalize();
}

void resourceManager(int* matrix, int columns, int* vector,
 int vectorSize, int processors, int minPortionRows, int deallocatedRowsSize){
    
    //Index of rows of matrix
    int rowMatrixIndex = 0; 
     
    //Auxiliary matrix
    int* portionMatrix = allocate1D2DMatrix(minPortionRows , columns); //Auxiliary Matrix
    int* portionMatrixWithExtraData = allocate1D2DMatrix(minPortionRows + 1 , columns); //Auxiliary Matrix
    
    //MPI Info
    int count = minPortionRows * columns; //Counts of data to send to process

    //Assign vector and data to every worker process
    for(int processorNumber = 0; processorNumber < processors ; processorNumber++){

        //Prepare data
        if(deallocatedRowsSize > 0){ //Case of slices to add in some processors
            
            //Get portion from matrix
            for(int rowIndex = 0; rowIndex < minPortionRows + 1; rowIndex++){
                for(int columnIndex = 0; columnIndex < columns; columnIndex++){
                    portionMatrixWithExtraData[ rowIndex*columns + columnIndex] = matrix[(rowMatrixIndex + rowIndex)*columns + columnIndex];
                }
            }
            
            //Update row Index
            rowMatrixIndex += minPortionRows + 1; 
          
            //Send Data 
            MPI_Send(portionMatrixWithExtraData, (count + columns), MPI_INT, processorNumber, TAGMATRIX, MPI_COMM_WORLD);
            
            
            //One less
            deallocatedRowsSize--;

        } 
        else{ //Case normal
            
            //Get portion from matrix
            for(int rowIndex = 0; rowIndex < minPortionRows; rowIndex++){
                for(int columnIndex = 0; columnIndex < columns; columnIndex++){
                    portionMatrix [rowIndex*columns + columnIndex] = matrix[(rowIndex + rowMatrixIndex) * columns + columnIndex];
                }
            }
            
            //Update row Index
            rowMatrixIndex += minPortionRows; 
        
            //Send Data 
            MPI_Send(portionMatrix, count, MPI_INT, processorNumber, TAGMATRIX, MPI_COMM_WORLD);
        }

        //Send Vector
        MPI_Send(vector, vectorSize, MPI_INT, processorNumber, TAGVECTOR, MPI_COMM_WORLD);
    } 
}

int* resultsManager(int totalRows, int columns, int minPortionRows, int processors, int deallocatedRowsSize){
    //Index of rows of matrix
    int rowMatrixIndex = 0; 
     
    //Auxiliary matrix
    int* portionMatrix = allocate1D2DMatrix(minPortionRows , columns); //Auxiliary Matrix
    int* portionMatrixWithExtraData = allocate1D2DMatrix(minPortionRows + 1 , columns); //Auxiliary Matrix

    int* results = allocate1D2DMatrix(totalRows, 1);

    //Assign vector and data to every worker process
    for(int processorNumber = 0; processorNumber < processors ; processorNumber++){

        //Prepare data
        if(deallocatedRowsSize > 0){ //Case of slices to add in some processors
            
            //Receive the data
            MPI_Recv(portionMatrixWithExtraData, minPortionRows + 1, MPI_INT, processorNumber, TAGRESPONSE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            
            for(int rowIndex = 0; rowIndex < minPortionRows + 1 ; rowIndex++){

                //Get the data to results
                results[rowIndex + rowMatrixIndex] = portionMatrixWithExtraData[rowIndex];
            }

            //Update row Index
            rowMatrixIndex += minPortionRows + 1; 
            
            //One less
            deallocatedRowsSize--;
        } 
        else{ //Case normal
            
            //Receive the data
            MPI_Recv(portionMatrix, minPortionRows, MPI_INT, processorNumber, TAGRESPONSE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            for(int rowIndex = 0; rowIndex < minPortionRows ; rowIndex++){
                
                //Get the data to results
                results[rowIndex + rowMatrixIndex] = portionMatrix[rowIndex];
            }

            //Update row Index
         rowMatrixIndex += minPortionRows; 
        }
    }
    //Free memory
    free(portionMatrixWithExtraData);
    free(portionMatrix);

    //Return results
    return results; 
}

int* allocate1D2DMatrix(int rows, int columns){
    int *matrix = (int *)malloc(rows * columns * sizeof(int)); 
    return matrix;
}

void fillMatrix(int* matrixToFill, int rows, int columns){
    int dataToCharge = 1;

    for(int rowIndex = 0; rowIndex < rows; rowIndex++){
        for(int columnIndex = 0; columnIndex < columns; columnIndex++){
            matrixToFill[ (rowIndex*columns) + columnIndex ] = dataToCharge++;
        }
    }
}

void printMatrix(int* matrixToPrint, int rows, int columns){
    printf("[\n");
    for(int rowIndex = 0; rowIndex < rows; rowIndex++){
        for(int columnIndex = 0; columnIndex < columns; columnIndex++){
            printf("%d ",matrixToPrint[rowIndex * columns + columnIndex]);
        }
        printf("\n");
    }
    printf("]");
}
