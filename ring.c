/*
 * Lab5: Ring
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


void comm(int quantity, int worldrank, int worldsize){
  int j, i, rightProc;
  double buf[quantity];
  double ubuf[quantity];
  double start, end;
  MPI_Status stat;
  MPI_Request req;
  for(i = 0; i < quantity; i++){
    buf[i] = 62793;
  }
  rightProc = (worldrank + 1) % worldsize;
  start = MPI_Wtime();
  for(j = 0; j < worldsize; j++){
    if(worldrank == j){
      MPI_Isend(buf, quantity, MPI_DOUBLE, rightProc, 0, MPI_COMM_WORLD, &req);
      if(worldrank != 0){
        MPI_Irecv(ubuf, quantity, MPI_DOUBLE, worldrank - 1, 0, MPI_COMM_WORLD, &req);
        memcpy(buf, ubuf, sizeof(ubuf));
      }
      else{
        MPI_Irecv(ubuf, quantity, MPI_DOUBLE, worldsize - 1, 0, MPI_COMM_WORLD, &req);
        memcpy(buf, ubuf, sizeof(ubuf));
      }
      MPI_Wait(&req, MPI_STATUS_IGNORE);
    }
  }
  end = MPI_Wtime();
  if(worldrank == 0){
    printf("The time for %d doubles was %f ms.\n", quantity, (end - start) * 1000);
  }
}
int main(int argc, char **argv) {
  int i, j, rightProc, worldsize, worldrank, quantity;


  MPI_Init(NULL, NULL);

  //get number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &worldsize);
  //Get rank of processor
  MPI_Comm_rank(MPI_COMM_WORLD, &worldrank); 

  for(i = 0; i < 6; i++){
    if(i == 0){
      quantity = 1;
      comm(quantity, worldrank, worldsize);
    }
    else if(i == 1){
      quantity = 8;
      comm(quantity, worldrank, worldsize);
    }
    else if(i == 2){
      quantity = 64;
      comm(quantity, worldrank, worldsize);
    }
    else if(i == 3){
      quantity = 512;
      comm(quantity, worldrank, worldsize);
    }
    else if(i == 4){
      quantity = 4096;
      comm(quantity, worldrank, worldsize);
    }
    else{
      quantity = 32768;
      comm(quantity, worldrank, worldsize);
    }
  }
  MPI_Finalize();
  return 0;       
}    
