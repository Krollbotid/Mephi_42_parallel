#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <string.h>
#include <limits.h>

void shellSort(int *A, int count)
{
    int x, k;
    for (int d = count / 2; d > 0; d /= 2)
    {
        for (int i = 0; i < d; ++i)
        {
            for (int j = i + d; j < count; j += d)
            {
                x = A[j];
                int k = j - d;
                while (k >= i && A[k] > x)
                {
                    A[k + d] = A[k];
                    k -= d;
                }
                A[k + d] = x;
            }
        }
    }
    return;
}

void afterSort(int *input, int *res, int block, int count, int size)
{
    int *ps = (int*) malloc(size * sizeof(int));
    int x, nx;
    for (int i = 0; i < size; ++i)
    {
        ps[i] = i * block;
    }
    for (int i = 0; i < count; ++i)
    {
        x = INT_MAX;
        nx = -1;
        for (int j = 0; j < size; ++j)
        {
            if (ps[j] < (j + 1) * block && input[ps[j]] < x)
            {
                x = input[ps[j]];
                nx = j;
            }
        }
        res[i] = x;
        ps[nx]++;
    }
    free(ps);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    if (argc != 1)
    {
        printf("Wrong usage");
        MPI_Finalize();
        return 0;
    }
    srand(0);
    int size, rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int rcount = 10e5;
    int count = rcount + (size - rcount % size);
    int block = count / size;
    int *input, *b, *res;
    if (!rank)
    {
        input = (int*) malloc(count * sizeof(int));
        res = (int*) malloc(count * sizeof(int));
    }
    b = (int*) malloc(block * sizeof(int));

    if (!rank)
    {
        for (int i = 0; i < count; ++i)
        {
            input[i] = rand();
        }
    }
    MPI_Scatter(input, block, MPI_INT, b, block, MPI_INT, 0, MPI_COMM_WORLD);

    double begin, end;
    begin = MPI_Wtime();

    shellSort(b, block);

    MPI_Gather(b, block, MPI_INT, input, block, MPI_INT, 0, MPI_COMM_WORLD);

    MPI_Barrier(MPI_COMM_WORLD);

    if (!rank)
    {
        afterSort(input, res, block, count, size);
    }
    end = MPI_Wtime();

    if (!rank)
    {
        free(input);
        free(res);
    }
    free(b);
    if (!rank)
    {
        FILE *fp;
        fp = fopen("log.txt", "a");
        fprintf(fp, "%d %f\n", size, end - begin);
        fclose(fp);
    }
    MPI_Finalize();
    return 0;
}