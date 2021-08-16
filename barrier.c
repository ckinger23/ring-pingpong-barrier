/*
 * Lab5: Barrier
 * Carter King
 * CS380: Parallel
 * Dr. Larkins
 * Due Friday, 4/26/19
 */

#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <openssl/des.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <mpi.h>
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"


void ring_barrier(int worldrank, int worldsize){
  int message = 0;
  int rightProc, i, buf;
  rightProc = (worldrank + 1) % worldsize;
  for (i = 0; i < 2; i++){
    if(i == 0){
      if(worldrank != 0){
        MPI_Recv(&message, 1, MPI_INT, worldrank - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Send(&buf, 1, MPI_INT, rightProc, 1, MPI_COMM_WORLD);
      }
      else{
        message = 1;
        MPI_Send(&message, 1, MPI_INT, rightProc, 1, MPI_COMM_WORLD);
        MPI_Recv(&buf, 1, MPI_INT, worldsize - 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
      }
    }
    else{
      return;
    }
  }
}

void tree_barrier(int worldrank, int worldsize){
  int message = 0;
  int secondmessage = 0;
  int leftbuf;
  int rightbuf;
  int recvbuf;
  MPI_Status stat;
  //no left child
  if(((2 * worldrank) + 1) > (worldsize - 1)){
    //even
    if(worldrank % 2 == 0){
      MPI_Send(&message, 1, MPI_INT, (worldrank / 2) - 1, 23, MPI_COMM_WORLD);
    }
    else{
      MPI_Send(&message, 1, MPI_INT, (worldrank / 2), 23, MPI_COMM_WORLD);
    }
  }
  //only left child
  else if(((2 * worldrank) + 1) == (worldsize - 1)){
    MPI_Recv(&leftbuf, 1, MPI_INT, ((2 * worldrank) + 1), 23, MPI_COMM_WORLD, &stat);
    //even
    if(worldrank % 2 == 0){
      MPI_Send(&message, 1, MPI_INT, (worldrank / 2) - 1, 23, MPI_COMM_WORLD);
    }
    else{
      MPI_Send(&message, 1, MPI_INT, (worldrank / 2), 23, MPI_COMM_WORLD);
    }


  }
  //both children
  else{
    MPI_Recv(&leftbuf, 1, MPI_INT, ((2 * worldrank) + 1), 23, MPI_COMM_WORLD, &stat);
    MPI_Recv(&rightbuf, 1, MPI_INT, ((2 * worldrank) + 2), 23, MPI_COMM_WORLD, &stat);
    //even
    if(worldrank % 2 == 0){
      MPI_Send(&message, 1, MPI_INT, (worldrank / 2) - 1, 23, MPI_COMM_WORLD);
    }
    else{
      MPI_Send(&message, 1, MPI_INT, (worldrank / 2), 23, MPI_COMM_WORLD);
    }
  }

  //Return down the tree
  //no left child
  if(((2 * worldrank) + 1) > (worldsize - 1)){
    if(worldrank % 2 == 0){
      MPI_Recv(&recvbuf, 1, MPI_INT, (worldrank / 2) - 1, 17, MPI_COMM_WORLD, &stat);
    }
    else{
      MPI_Recv(&recvbuf, 1, MPI_INT, (worldrank / 2), 17, MPI_COMM_WORLD, &stat);
    }
    return;
  }
  //only left child
  else if(((2 * worldrank) + 1) == (worldsize - 1)){
    if(worldrank % 2 == 0){
      MPI_Recv(&recvbuf, 1, MPI_INT, (worldrank / 2) - 1, 17, MPI_COMM_WORLD, &stat);
    }
    else{
      MPI_Recv(&recvbuf, 1, MPI_INT, (worldrank / 2), 17, MPI_COMM_WORLD, &stat);
    }
    MPI_Send(&message, 1, MPI_INT, ((2 * worldrank) + 1), 17, MPI_COMM_WORLD);
    return;
  }
  //both children
  else{
    if(worldrank % 2 == 0){
      MPI_Recv(&recvbuf, 1, MPI_INT, (worldrank / 2) - 1, 17, MPI_COMM_WORLD, &stat);
    }
    else{
      MPI_Recv(&recvbuf, 1, MPI_INT, (worldrank / 2), 17, MPI_COMM_WORLD, &stat);
    }
    MPI_Send(&message, 1, MPI_INT, ((2 * worldrank) + 1), 17, MPI_COMM_WORLD);
    MPI_Send(&message, 1, MPI_INT, ((2 * worldrank) + 2), 17, MPI_COMM_WORLD);
    return;
  }


}

int main(int argc, char **argv) {
  int worldsize, worldrank, i, j;
  double start, end;
  MPI_Init(NULL, NULL);
  //get number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &worldsize);
  //Get rank of processor
  MPI_Comm_rank(MPI_COMM_WORLD, &worldrank); 
  start = MPI_Wtime();
  for(i = 0; i < 1000; i++){
    ring_barrier(worldrank, worldsize);
  }
  end = MPI_Wtime();
  if(worldrank == 0){
    printf("ring_barrier() with %d processors took %f ms.\n", worldsize, (end - start) * 1000);
  }
  start = MPI_Wtime();
  for(j = 0; j < 1000; j++){
    tree_barrier(worldrank, worldsize);
  }
  end = MPI_Wtime();
  if(worldrank == 0){
    printf("tree_barrier() with %d processors took %f ms.\n", worldsize, (end - start) * 1000);
  }
  MPI_Finalize();
  return 0;       
}    
