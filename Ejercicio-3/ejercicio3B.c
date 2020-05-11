#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#define ROOTPROCESSOR 0

//Utils
int *allocate1D2DMatrix(int rows, int columns);

//Program Utils
int getCurrentRows(int rowsNumber, int rank, int size);
void calcSendCountAndDisplacements(int* sendCounts, int*displacements,int size, int rowsNumber, int multiplier);

//Matrix Operations
void printMatrix(int *matrixToPrint, int rows, int columns);
void fillMatrix(int *matrixToFill, int rows, int columns);
void multiplyMatrixVector(int *matrixA, int *matrixB, int *dataProcessed, int rows, int columns);



int main(int argc, char** argv){
    
    /*
        rank = processor rank in Communicator
        size = Size of Communicator
        rowsNumber = rowsNumber of Matrix  == ColumnNumber
        countToReceive = Counts of data that will receive this processor
        currentRows = Rows that will handle this processor

        dataProcessed = Buffer for multiplication of matrixPortion with VectorMultiply
        vectorToMultiply - self-explained
        matrixPortion = MatrixPortion that the processor receives.
    */
    int rank, size, rowsNumber, countToReceive = 0, currentRows;
    int *dataProcessed, *vectorToMultiply, *matrixPortion;

    //Init MPI_Library
    MPI_Init(&argc, &argv);

    //Get number of laps from args
    rowsNumber = atoi(argv[1]);

    //Get rank from rank in their commnicator
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //Get Number of processes
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    //Set the data for this processor
    currentRows  = getCurrentRows(rowsNumber, rank, size);
    countToReceive = currentRows * rowsNumber;

    /*Allocate Memory*/
    vectorToMultiply = allocate1D2DMatrix(rowsNumber, 1);
    dataProcessed = allocate1D2DMatrix(currentRows, 1);
    matrixPortion = allocate1D2DMatrix(currentRows, rowsNumber); 
   
    if (rank == ROOTPROCESSOR){
        //1° Allocate Matrix and results
        int *matrix = allocate1D2DMatrix(rowsNumber, rowsNumber);
        int *results = allocate1D2DMatrix(rowsNumber, 1);

        /*2°Vectors with information for Scatterv and gatherv
            sendCounts - as long as "size", in each position, the count that will send to each processor
            displacements - as log as "size", in each position, the displacement from where to take de data
        */
        int *sendCounts = allocate1D2DMatrix(1, size);
        int *displacements = allocate1D2DMatrix(1, size); 

        //3° Fill Matrix and Vector
        fillMatrix(matrix, rowsNumber, rowsNumber); 
        fillMatrix(vectorToMultiply, rowsNumber, 1);

         
        
        //1° Share data with all the process in communicator: Calculate de Sendcount and displacement for share 
        //                                                    the data among the processors from the "matrix"
        
        calcSendCountAndDisplacements(sendCounts, displacements, size, rowsNumber ,rowsNumber);
        MPI_Scatterv(matrix, sendCounts, displacements, MPI_INT, matrixPortion, countToReceive, MPI_INT, ROOTPROCESSOR, MPI_COMM_WORLD);
        free(matrix); //Free matrix as soon as posible

        //2° Shares the content of vector with other processors
        MPI_Bcast(vectorToMultiply, rowsNumber, MPI_INT, 0, MPI_COMM_WORLD);

        //3° Multiply
        multiplyMatrixVector(matrixPortion, vectorToMultiply, dataProcessed, currentRows, rowsNumber);
        
        /*4° Gather data from all the processors in the communicator:
                Calculate de Sendcount and displacement for share the data among the processors from the "matrix"
        */
        calcSendCountAndDisplacements(sendCounts, displacements, size, rowsNumber, 1);
        MPI_Gatherv(dataProcessed, currentRows, MPI_INT, results, sendCounts, displacements, MPI_INT, ROOTPROCESSOR, MPI_COMM_WORLD);

        //5° -- FINAL STEP : Print RESULTS
        printMatrix(results, rowsNumber, 1);


        //6° Free memory after deliver data to all the processors
        free(sendCounts);
        free(displacements);
        free(results);
    }
    else{
        //1° Receive in matrixPortion
        MPI_Scatterv(NULL, NULL, NULL, MPI_INT, matrixPortion, countToReceive, MPI_INT, ROOTPROCESSOR, MPI_COMM_WORLD);
        
        //2°Receive in vectorToMultiply
        MPI_Bcast(vectorToMultiply, rowsNumber, MPI_INT, 0, MPI_COMM_WORLD);
       
        //3° Multiply
        multiplyMatrixVector(matrixPortion, vectorToMultiply, dataProcessed, currentRows,rowsNumber);
        
        //4°Send data processed to Master
        MPI_Gatherv(dataProcessed, currentRows, MPI_INT, NULL, NULL, NULL, MPI_INT, ROOTPROCESSOR, MPI_COMM_WORLD);
    }
    
    //4° Release Memory
    free(matrixPortion);
    free(vectorToMultiply);
    free(dataProcessed);

    //Finalize Process
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}

//Function to calculate the number of rows that will handle the "rank" processor
int getCurrentRows(int rowsNumber, int rank, int size){

    /*
        isExtraPortion = Info about if its a bigger or normal size portion of a matrix.
        minPortionRows = minSize for matrix portion, when rows are not divisible with number of processors
    */
    int isExtraPortion = 0, minPortionRows = 0;

    //Get Info about number of rows vs processors 
    minPortionRows = rowsNumber / size; //To know how big will be what we will send.

    // Cause the program knows that the master will send in order, and the biggers will be in the first deallocatedRowsSize processors,
    // So we can deduce beforehand if it will be bigger or not.
    isExtraPortion = (((rowsNumber % size) - (rank)) <= 0); //(rowLength % size) == number of rows unallocated

    //Update actual PortionRows
    return isExtraPortion ? minPortionRows  : (minPortionRows + 1);
}

//Function to calculate the sendCount and Displacement array for MPI_Gatherv and MPI_Scatterv function
void calcSendCountAndDisplacements(int* sendCounts, int*displacements,int size, int rowsNumber, int multiplier){
    int sum = 0; //Auxiliary to set the displacement for each processor
    
    for(int processorRank = 0; processorRank < size; processorRank++){
        sendCounts[ processorRank ] = getCurrentRows(rowsNumber, processorRank, size) * multiplier; 
        displacements[processorRank] = sum;
        //Index of total array
        sum += sendCounts[processorRank];
    }
}

int *allocate1D2DMatrix(int rows, int columns){
    int *matrix = (int *)malloc(rows * columns * sizeof(int));
    return matrix;
}

void fillMatrix(int *matrixToFill, int rows, int columns){
    int dataToCharge = 1;

    for (int rowIndex = 0; rowIndex < rows; rowIndex++)
    {
        for (int columnIndex = 0; columnIndex < columns; columnIndex++)
        {
            matrixToFill[(rowIndex * columns) + columnIndex] = dataToCharge++;
        }
    }
}

void printMatrix(int *matrixToPrint, int rows, int columns){
    printf("[\n");
    for (int rowIndex = 0; rowIndex < rows; rowIndex++)
    {
        for (int columnIndex = 0; columnIndex < columns; columnIndex++)
        {
            printf("%d ", matrixToPrint[rowIndex * columns + columnIndex]);
        }
        printf("\n");
    }
    printf("]");
}

void multiplyMatrixVector(int *matrixA, int *vector, int *dataProcessed, int rows, int columns){

    for (int rowIndex = 0; rowIndex < rows; rowIndex++)
    {
        dataProcessed[rowIndex] = 0;
        for (int colIndex = 0; colIndex < columns; colIndex++)
        {
            dataProcessed[rowIndex] += matrixA[rowIndex * columns + colIndex] * vector[colIndex];
        }
    }
}