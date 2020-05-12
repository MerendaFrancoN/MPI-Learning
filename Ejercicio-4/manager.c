#include <stdio.h>
#include <stdlib.h>
#include <mpi/mpi.h>

#define ROOT_PROCESSOR 0
//Info Reference: https://etutorials.org/Linux+systems/cluster+computing+with+linux/Part+II+Parallel+Programming/Chapter+9+Advanced+Topics+in+MPI+Programming/

/* Explanation of MPI_Comm_spawn - https://stackoverflow.com/questions/21497605/creating-child-process-with-mpi-comm-spawn

MPI_Comm_spawn is a collective call and it does not spawn n additional
processes per rank but rather it spawns a child MPI job with n processes, therefore
adding n to the total number of processes. When called with n = 2, it spawn a child 
job with 2 processes and that's exactly what you observe in the output.


To spawn n processes from one parent, you have to use MPI_COMM_SELF.
*/

//Error Function
void error(char* string);

int main(int argc, char *argv[]){

    /*
        World_size == Intracommunicator amount of processors
        flag = Control for error in call to MPI_Comm_get_attr
        universe_sizep = Get the size of universe processes
    */
    int world_size = 0;
    int flag, universe_sizep, universe_size;
    char workerProgram[10] = "./worker";
    /*Intercommunicator -- Binds two groups of processes, the local group and remote groups.
      Ranks in intercommunicator always refer to the processes in the remote group.
      The local group contains the parents and the remote the children.*/ 
    MPI_Comm workercomm;


    //Init MPI
    MPI_Init(&argc, &argv);
    
    //Get size of Intracommunicator MPI_COMM_WORLD
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    if(world_size != 1) 
        error("Top heavy with management\n");

    //Get Info about processors availables
    MPI_Comm_get_attr(MPI_COMM_WORLD, MPI_UNIVERSE_SIZE, &universe_sizep, &flag);

    //Control for error to call MPI_Comm_get_attr attribute
    if(!flag){
        printf("This MPI does not support UNIVERSE_SIZE. How many processes total?");
        scanf("%d", &universe_size);
    }else{
        //Define the actual universe size
        universe_size = universe_sizep;    
    }

    //Check if there is processors availabe to spawn new workers
    if(universe_size == 1)
        error("No room to start workers");

    //Print I'm a parent
    printf("I'm a parent!!\n");

    //Spawn Workers - A laburah !
    MPI_Comm_spawn(workerProgram, MPI_ARGV_NULL, universe_size - 1, MPI_INFO_NULL, ROOT_PROCESSOR, MPI_COMM_SELF, &workercomm, MPI_ERRCODES_IGNORE);
    
    //Finalize Process Root
    MPI_Finalize();

    return 0;
}

void error(char* string){
    printf("%s", string);
    exit(0);
}

