#include<stdio.h>
#include<stdlib.h>
#include <mpi/mpi.h>

void error(char* string);

int main(int argc, char *argv[]){
    
    //Parent Communicator
    MPI_Comm parent;
    
    //Size of remote group - Parent communicator group
    int size; 


    //Init Worker
    MPI_Init(&argc, &argv);

    //Get communicator from parent
    MPI_Comm_get_parent(&parent);

    if(parent == MPI_COMM_NULL)
        error("No Parent!\n");

    //Get size of remote group
    MPI_Comm_remote_size(parent, &size);

    printf("I'm a child - sizeParent = %d\n", size);
    //Control for size
    if(size != 1)
        error("Something's wrong with the parent");
    
    //Finalize program
    MPI_Finalize();

    return 0;
}

void error(char* string){
    printf("%s", string);
    exit(0);
}