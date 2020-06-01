#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int barrier(MPI_Comm world_comm){
    
    int size, node;
    MPI_Status status;

    //Get the size of the communicator
    MPI_Comm_size(world_comm, &size);
    //Get who is calling
    MPI_Comm_rank(world_comm, &node);

    if (node > 0){
        //Send the notification 
        MPI_Send(NULL, 0, MPI_INT, 0, 0, world_comm);
        //Wait for master confirmation
        MPI_Recv(NULL, 0, MPI_INT, 0, 0, world_comm, &status);
    }
    else{
        //Root part
        //Waits for all
        for (int i = 1; i < size; i++){
            MPI_Recv(NULL, 0, MPI_INT, i, 0, world_comm, &status);
        }
        
        //Notify them that all are at the same point
        for (int i = 1; i < size; i++){
            MPI_Send(NULL, 0, MPI_INT, i, 0, world_comm);
        }
    }
    return 1;
}

int cast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm world_comm) { 
     
    int size, node;
    //Get the size of the communicator
    MPI_Comm_size(world_comm, &size);
    //Get who is calling
    MPI_Comm_rank(world_comm, &node);

    if(node == root){
        for(int i = 0; i < size; i++){
            
            if(i != root){
                MPI_Send(buffer, count, datatype, i, MPI_ANY_TAG, world_comm);
            }
        }
    } else{
        MPI_Recv(buffer, count, datatype, root, MPI_ANY_TAG, world_comm, MPI_STATUS_IGNORE);
    }
}

int scatter(void *sendBuffer, int sendCount, MPI_Datatype sendtype, void* recvbuffer, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm world_comm) {

    int size, node, type_size, offset = 0;

    //Get the size of the communicator
    MPI_Comm_size(world_comm, &size);
    
    //Get who is calling
    MPI_Comm_rank(world_comm, &node);

    //Get datatype size
    MPI_Type_size(sendtype, &type_size);
    
    if(node == root){
        
        for(int i = 0; i < size; i++){
            
            void *data = (sendBuffer + (offset * type_size));
            if(i != root)
                MPI_Send(data, sendCount, sendtype, i, MPI_ANY_TAG, world_comm);
            else
                memcpy(recvbuffer, data, (sendCount * type_size));
                
            offset += sendCount; //Offset of sending data
        }
    }else
        MPI_Recv(recvbuffer, recvcount, recvtype, root, MPI_ANY_TAG, world_comm, MPI_STATUS_IGNORE);

    return 1;
    
}


int gather(void *sendbuffer, int sendCount, MPI_Datatype sendtype, void *recvbuffer, int recvcount, MPI_Datatype recvtype, int root, MPI_Comm world_comm) {
    int size, node, type_size;
    MPI_Status status;

    //Get the size of the communicator
    MPI_Comm_size(world_comm, &size);
    
    //Get who is calling
    MPI_Comm_rank(world_comm, &node);

    //Get datatype size
    MPI_Type_size(sendtype, &recvtype);
    
    if (node == root){

        unsigned long data_size = recvcount * recvtype; //Get the size
        void *data = malloc(data_size); //Buffer to receive from process

        for (int i = 0; i < size; i++){
            if (i != root) {
                //Receive from all process
                MPI_Recv(data, recvcount, recvtype, MPI_ANY_SOURCE, MPI_ANY_TAG, world_comm, &status);                 
            }
            else {
                memcpy(data, sendbuffer, data_size);
            }
            memcpy(recvbuffer + (data_size * i), data, data_size);        
        }
    }
    else {
        MPI_Send(sendbuffer, sendCount, sendtype, root, MPI_ANY_TAG, world_comm);
    }   
}

int reduce(void *sendbuffer, void *recvbuffer, int count, MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm){
    
}

int allToAll(void *sendbuffer, int sendCount, MPI_Datatype sendtype, void* recvbuffer, int recvcount, MPI_Datatype recvtype, MPI_Comm comm){

}