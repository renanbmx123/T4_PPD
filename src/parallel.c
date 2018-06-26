#include "parallel.h" 

int main(int argc, char *argv[])
{
	double t1, t2;
	int my_rank;
	int proc_n;
	MPI_Status status;
	int i;  

	alg = QSORT;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &proc_n);
	t1 = MPI_Wtime();
	if (my_rank == 0)
	{ 
		int array[ARRAY_SIZE];
		for (i = 0; i < ARRAY_SIZE; i++)
			array[i] = ARRAY_SIZE - i;

		int *result = order(my_rank, array, ARRAY_SIZE, status);
		
	}
	else
	{
		int size; 
	
		MPI_Recv(&size, 1, MPI_INT, parent(my_rank), SIZE_TAG, MPI_COMM_WORLD, &status);

		int *array = calloc(size, sizeof(int));

		MPI_Recv(array, size, MPI_INT, parent(my_rank), SEND_DOWN_TAG, MPI_COMM_WORLD, &status);

		if(is_leaf(size, ARRAY_SIZE, proc_n))
		{
			if(alg == BSORT)
			{
				bs(size, array);
			}
			else if(alg == QSORT)
			{
		        qsort(&array, size, sizeof(int), compare);
			}
			MPI_Send(array, size, MPI_INT, parent(my_rank), SEND_UP_TAG, MPI_COMM_WORLD);
		}
		else
		{
			array = order(my_rank, array, size, status);
			MPI_Send(array, size, MPI_INT, parent(my_rank), SEND_UP_TAG, MPI_COMM_WORLD);
		}
	}

	if (my_rank == 0) {
		t2 = MPI_Wtime();
		printf( "Elapsed time is %f\n", t2 - t1 );
	}
	MPI_Finalize();
	return 0;
}

int* order(int my_rank, int array[], int size, MPI_Status status)
{
	int half_size = size / 2;
	// Send size
	MPI_Send(&half_size, 1,
			MPI_INT, left_child(my_rank), SIZE_TAG,
			MPI_COMM_WORLD);

	MPI_Send(&half_size, 1,
			MPI_INT, right_child(my_rank), SIZE_TAG,
			MPI_COMM_WORLD);

	// Send array
	MPI_Send(array, half_size,
			MPI_INT, left_child(my_rank), SEND_DOWN_TAG,
			MPI_COMM_WORLD);

	MPI_Send(array + half_size, half_size,
			MPI_INT, right_child(my_rank), SEND_DOWN_TAG,
			MPI_COMM_WORLD);

	// Receive response
	MPI_Recv(array, half_size,
			MPI_INT, left_child(my_rank), SEND_UP_TAG,
			MPI_COMM_WORLD, &status);

	MPI_Recv(array + half_size, half_size,
			MPI_INT, right_child(my_rank), SEND_UP_TAG,
			MPI_COMM_WORLD, &status);

	return interleaving(array, size);
}


