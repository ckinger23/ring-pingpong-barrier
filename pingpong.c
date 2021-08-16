/*
 * Lab5: Ping Pong
 * Carter King
 * CS380: Parallel
 * Due Friday 4/26/19
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



void blocking(int quantity, int worldrank){
  double buf[quantity];
  double ubuf[quantity];
  int i;
  double start, end;
  MPI_Status stat;
  for(i = 0; i < quantity; i++){
    buf[i] = 62793;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(worldrank == 0){
    start = MPI_Wtime();
    MPI_Send(buf, quantity, MPI_DOUBLE, 1, 123, MPI_COMM_WORLD);
    MPI_Recv(ubuf, quantity, MPI_DOUBLE, 1, 456, MPI_COMM_WORLD, &stat);
    end = MPI_Wtime();
    printf("Time for Blocking, %d items was %f ms.\n", quantity, (end - start) * 1000);
  }
  else if(worldrank == 1){
    MPI_Recv(ubuf, quantity, MPI_DOUBLE, 0, 123, MPI_COMM_WORLD, &stat);
    memcpy(buf, ubuf, sizeof(ubuf));
    MPI_Send(buf, quantity, MPI_DOUBLE, 0, 456, MPI_COMM_WORLD);

  }
  return;
}

void nonblocking(int quantity, int worldrank){
  double buf[quantity];
  double ubuf[quantity];
  MPI_Request req0, req1;
  MPI_Status stat;
  int i;
  double start, end;
  for(i < 0; i < quantity; i++){
    buf[i] = 62793;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(worldrank == 0){
    start = MPI_Wtime();
    MPI_Isend(buf, quantity, MPI_DOUBLE, 1, 0, MPI_COMM_WORLD, &req0);
    MPI_Wait(&req0, &stat);
    MPI_Irecv(ubuf, quantity, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD, &req1);
    MPI_Wait(&req1, &stat);
    end = MPI_Wtime();
    printf("Time for Non-Blocking, %d items was %f ms.\n", quantity, (end - start) * 1000);
  }
  else if(worldrank == 1){
    MPI_Irecv(ubuf, quantity, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, &req0);
    MPI_Wait(&req0, &stat);
    memcpy(buf, ubuf, sizeof(ubuf));
    MPI_Isend(buf, quantity, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD, &req1);
    MPI_Wait(&req1, &stat);
  }
  return;
}

void blocksend(int quantity, int worldrank){
  double buf[quantity];
  double ubuf[quantity];
  MPI_Request req;
  MPI_Status stat;
  int i;
  double start, end;
  for(i = 0; i < quantity; i ++){
    buf[i] = 62793;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(worldrank == 0){
    start = MPI_Wtime();
    MPI_Send(buf, quantity, MPI_DOUBLE, 1, 123, MPI_COMM_WORLD);
    MPI_Irecv(ubuf, quantity, MPI_DOUBLE, 1, 456, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);
    end = MPI_Wtime();
    printf("Time for B Send, NB Recv, %d items was %f ms.\n", quantity, (end - start) * 1000);
  }
  else if(worldrank == 1){
    MPI_Irecv(ubuf, quantity, MPI_DOUBLE, 0, 123, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);
    memcpy(buf, ubuf, sizeof(ubuf));
    MPI_Send(buf, quantity, MPI_DOUBLE, 0, 456, MPI_COMM_WORLD);
  }
  return;
}

void blockrecv(int quantity, int worldrank){
  double buf[quantity];
  double ubuf[quantity];
  MPI_Request req;
  MPI_Status stat;
  int i;
  double start, end;
  for(i = 0; i < quantity; i++){
    buf[i] = 62793;
  }
  MPI_Barrier(MPI_COMM_WORLD);
  if(worldrank == 0){
    start = MPI_Wtime();
    MPI_Isend(buf, quantity, MPI_DOUBLE, 1, 123, MPI_COMM_WORLD, &req);
    MPI_Wait(&req, &stat);
    MPI_Recv(ubuf, quantity, MPI_DOUBLE, 1, 456, MPI_COMM_WORLD, &stat);
    end = MPI_Wtime();
    printf("Time for B recv and NB send, %d items was %f ms.\n", quantity, (end - start) * 1000);
  }
  else if(worldrank == 1){
    printf("0 send\n");
    MPI_Recv(ubuf, quantity, MPI_DOUBLE, 0, 123, MPI_COMM_WORLD, &stat);
    memcpy(buf, ubuf, sizeof(ubuf));
    printf("0 send\n");
    MPI_Isend(buf, quantity, MPI_DOUBLE, 0, 456, MPI_COMM_WORLD, &req);
    printf("send\n");
    MPI_Wait(&req, &stat);
  }
  return;
}

int main(int argc, char **argv) {
  int quantity, worldsize, i, name_len, worldrank;
  MPI_Init(NULL, NULL);  
  //get number of processes
  MPI_Comm_size(MPI_COMM_WORLD, &worldsize);
  //Get rank of processor
  MPI_Comm_rank(MPI_COMM_WORLD, &worldrank);

  for(i = 0; i < 6; i++){
    if(i == 0){
      quantity = 1;
      blocking(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      //nonblocking(quantity, worldrank);
      //MPI_Barrier(MPI_COMM_WORLD);
      blocksend(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      blockrecv(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
    }
    if(i == 1){
      quantity = 8;
      blocking(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      //nonblocking(quantity, worldrank);
      //MPI_Barrier(MPI_COMM_WORLD);
      blocksend(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      blockrecv(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
    }
    if(i == 2){
      quantity = 64;
      blocking(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      //nonblocking(quantity, worldrank);
      //MPI_Barrier(MPI_COMM_WORLD);
      blocksend(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      blockrecv(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
    }
    if(i == 3){
      quantity = 512;
      blocking(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      //nonblocking(quantity, worldrank);
      //MPI_Barrier(MPI_COMM_WORLD);
      blocksend(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      blockrecv(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
    }
    if(i == 4){
      quantity = 4096;
      blocking(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);  
      //nonblocking(quantity, worldrank);
      //MPI_Barrier(MPI_COMM_WORLD);
      blocksend(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      blockrecv(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
    }
    else{
      quantity = 32768;
      blocking(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      //nonblocking(quantity, worldrank);
      //MPI_Barrier(MPI_COMM_WORLD);
      blocksend(quantity, worldrank);
      MPI_Barrier(MPI_COMM_WORLD);
      blockrecv(quantity, worldrank);

    }
  }

  MPI_Finalize();
  return 0;
}

