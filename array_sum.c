#include <veda.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

#define NOW() ({ struct timespec ts; clock_gettime(CLOCK_MONOTONIC, &ts); ts; })
#define TIME(start, end) ((double)(end.tv_sec - start.tv_sec) * 1000.0 + (double)(end.tv_nsec - start.tv_nsec) / 1000000.0)

#define VEDA(err) check(err, __FILE__, __LINE__)

void check(VEDAresult err, const char* file, const int line) {
	if(err != VEDA_SUCCESS) {
		const char *name, *str;
		vedaGetErrorName	(err, &name);
		vedaGetErrorString	(err, &str);
		printf("%s: %s @ %s:%i\n", name, str, file, line);
		exit(1);
	}
}

int main(int argc,char *argv[]){
	struct timespec start, end;
	start = NOW();

	int rank, size;
    int n = 10000;  
    int array[n];
    int local_sum = 0, total_sum = 0;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        for (int i = 0; i < n; i++) {
            array[i] = i + 1; 
        }
    }

    int local_n = n / size;  

    int local_array[local_n];

    MPI_Scatter(array, local_n, MPI_INT, local_array, local_n, MPI_INT, 0, MPI_COMM_WORLD);

    for (int i = 0; i < local_n; i++) {
        local_sum += local_array[i];
    }

    MPI_Reduce(&local_sum, &total_sum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        printf("Total sum of array elements: %d\n", total_sum);
    }

    MPI_Finalize();

	end = NOW();
	double elapsed = TIME(start, end);
    //printf("Total Elapsed time: %f ms\n", elapsed);

	return 0;
}
