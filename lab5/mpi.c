#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: ./lab5 (seed)\n");
        return 0;
    }
    int size, rank;
    const int count = 100000000;
    const int random_seed = atoi(argv[1]);
    double begin, end;
    int *arr, lmax, max;
    FILE *fp;

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    arr = (int *)malloc(count * sizeof(int));
    if (!rank)
    {
        srand(random_seed);
        for (int i = 0; i < count; ++i)
        {
            arr[i] = rand();
        }
    }

    MPI_Bcast(arr, count, MPI_INTEGER, 0, MPI_COMM_WORLD);

    const int wstart = rank * count / size;
    const int wend = (rank + 1) * count / size;

    MPI_Barrier(MPI_COMM_WORLD);

    if (!rank)
    {
        begin = MPI_Wtime();
    }
    lmax = arr[0];
    for (int i = wstart; i < wend; ++i)
    {
        if (arr[i] > lmax)
        {
            lmax = arr[i];
        }
    }
    MPI_Reduce(&lmax, &max, 1, MPI_INTEGER, MPI_MAX, 0, MPI_COMM_WORLD);

    free(arr);

    if (!rank)
    {
        end = MPI_Wtime();
    }

    MPI_Finalize();
    if (!rank)
    {
        printf("Maximum is %d\n", max);
        fp = fopen("log1.txt", "a");
        fprintf(fp, "%d %lf\n", size, end - begin);
    }
    return 0;
}