#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: ./lab5 (threads) (seed)\n");
        return 0;
    }

    srand(atoi(argv[2]));
    const int count = 1000000000;
    const int threads = atoi(argv[1]);

    int *arr = (int *)malloc(count * sizeof(int));
    for (int i = 0; i < count; ++i)
    {
        arr[i] = rand();
    }

    int max = arr[0];
    double begin = omp_get_wtime();
    #pragma omp parallel num_threads(threads) shared(arr, count) reduction(max : max) default(none)
    {
        #pragma omp for
        for (int i = 0; i < count; ++i)
        {
            max = (arr[i] > max) ? arr[i] : max;
        }
    }
    double end = omp_get_wtime();

    printf("max: %d\n", max);
    FILE *fp = fopen("log.txt", "a");
    fprintf(fp, "%d %lf\n", threads, (end - begin));
    return 0;
}
