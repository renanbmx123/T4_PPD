#include <stdio.h>
#include <stdlib.h>
#include "bubblesort.h"
#include <time.h>

//#define DEBUG 1
#define ARRAY_SIZE 1000000

int main(int argc, const char* argv[])
{
	time_t start, stop;
  	char alg = *argv[1];
	start = clock();
	int vetor[ARRAY_SIZE];
	int i;

	for (i=0 ; i<ARRAY_SIZE; i++)
		vetor[i] = ARRAY_SIZE-i;

#ifdef DEBUG
	printf("\nVetor: ");
	for (i=0 ; i<ARRAY_SIZE; i++)
		printf("[%03d] ", vetor[i]);
#endif

	if(alg == 'q')
        qsort(vetor, ARRAY_SIZE, sizeof(int), compare);
    else if(alg == 'b')
        bubble_sort(ARRAY_SIZE, vetor);   

#ifdef DEBUG
	printf("\nVetor: ");
	for (i=0 ; i<ARRAY_SIZE; i++)
		printf("[%03d] ", vetor[i]);
#endif

	stop = clock();
  	float diff = ((float)(stop - start) / 1000000.F) * 1000;
   	printf("Time: %.0fms\n\n", diff);

	return 0;
}
