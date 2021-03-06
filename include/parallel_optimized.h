#pragma once

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"
#include "bubblesort.h"

//#define ARRAY_SIZE 100000
  #define ARRAY_SIZE 1000000
                   
//#define DEBUG 1
#define SIZE_TAG 0
#define SEND_UP_TAG 1
#define SEND_DOWN_TAG 2

#define BSORT 1
#define QSORT 2

int alg = 0;
int main(int argc, char *argv[]);
int *order(int my_rank, int array[], int size, MPI_Status status, int piece_size);
