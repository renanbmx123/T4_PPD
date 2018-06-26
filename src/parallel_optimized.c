#include "parallel_optimized.h"

int main(int argc, char *argv[])
{
	double t1, t2;
	
	int my_rank;
	int proc_n;
	MPI_Status status;
	int i;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
	t1 = MPI_Wtime();
	int piece_size = ARRAY_SIZE / proc_n;

	if (my_rank == 0)
	{
		int array[ARRAY_SIZE];
		for (i = 0; i < ARRAY_SIZE; i++)
			array[i] = ARRAY_SIZE - i;

		int *result = order(my_rank, array, ARRAY_SIZE, status, piece_size);
	}
	else
	{
		int size;

		MPI_Recv(&size, 1,
				 MPI_INT, parent(my_rank), SIZE_TAG,
				 MPI_COMM_WORLD, &status);

		int *array = calloc(size, sizeof(int));

		MPI_Recv(array, size,
				 MPI_INT, parent(my_rank), SEND_DOWN_TAG,
				 MPI_COMM_WORLD, &status);

		if(is_leaf3(size, ARRAY_SIZE, proc_n))
		{
			if(alg == BSORT)
			{
				bs(size, array);
			}
			else if(alg == QSORT)
			{
				qsort(array, size,sizeof(int), compare);
			}
			

			MPI_Send(array, size,
					 MPI_INT, parent(my_rank), SEND_UP_TAG,
					 MPI_COMM_WORLD);
		}
		else
		{
			array = order(my_rank, array, size, status, piece_size);
			MPI_Send(array, size,
					 MPI_INT, parent(my_rank), SEND_UP_TAG,
					 MPI_COMM_WORLD);
		}
	}
	if (my_rank == 0) {
		t2 = MPI_Wtime();
		printf( "Elapsed time is %f\n", t2 - t1 );
	}
	MPI_Finalize();

	return 0;
}

int* order(int my_rank, int array[], int size, MPI_Status status, int piece_size)
{

	int third_size = (size - piece_size) / 2;
	int left_size = size - 2 * third_size;
	// Send size
	MPI_Send(&third_size, 1,
			 MPI_INT, left_child(my_rank), SIZE_TAG,
			 MPI_COMM_WORLD);

	MPI_Send(&third_size, 1,
			 MPI_INT, right_child(my_rank), SIZE_TAG,
			 MPI_COMM_WORLD);

	// Send array
	MPI_Send(array, third_size,
			 MPI_INT, left_child(my_rank), SEND_DOWN_TAG,
			 MPI_COMM_WORLD);

	MPI_Send(array + third_size, third_size,
			 MPI_INT, right_child(my_rank), SEND_DOWN_TAG,
			 MPI_COMM_WORLD);

	//order rest locally
	if(alg == BSORT)
	{
		bs(left_size, array + 2 * third_size);
	}
	else if(alg == QSORT)
	{
		qsort((array+2* third_size), left_size,sizeof(int), compare);
	}
	

	// Receive response
	MPI_Recv(array, third_size,
			 MPI_INT, left_child(my_rank), SEND_UP_TAG,
			 MPI_COMM_WORLD, &status);

	MPI_Recv(array + third_size, third_size,
			 MPI_INT, right_child(my_rank), SEND_UP_TAG,
			 MPI_COMM_WORLD, &status);

	return interleaving3(array, size, piece_size);
}