#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int **makeArr(int rows, int columns)
{
    int **result = (int**) malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; ++i)
    {
        result[i] = (int*) malloc(columns * sizeof(int));
    }
    return result;
}

void fillRand(int **arrays, int rows, int columns)
{
    for (int i = 0; i < rows; ++i)
    {
        srand(time(NULL));
        for (int j = 0; j < columns; ++j)
        {
            arrays[i][j] = rand();
        }
    }
}

void printAvTime(int *threadNums, double *avtime, int threadNumsLen)
{
    printf("in milliseconds\n");
    for (int threadId = 0; threadId < threadNumsLen; ++threadId)
    {
        printf("%d: %.10lf\n", threadNums[threadId], avtime[threadId] * 1000.0);
    }
}

void printDel(int minsCount)
{
    for (int minsId = 0; minsId < minsCount; ++minsId)
    {
        printf("-");
    }
    printf("\n");
}

int main(int argc, char **argv)
{
    // Init data
    const int arLen = 1000000;
    const int arNum = 10;
    int threadNumsLen = 13;
    int threadsNum[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 16, 32, 64};

    // Init measurement arrays
    double *avTime = (double*) malloc(threadNumsLen * sizeof(double));

    // Init arrays
    int **arrays = makeArr(arNum, arLen);
    fillRand(arrays, arNum, arLen);

    // Start measuring
    for (int measId = 0; measId < threadNumsLen; ++measId)
    {
        int threadCount = threadsNum[measId];
        double totalTime = 0;
        for (int arId = 0; arId < arNum; ++arId)
        {
            double startTime = omp_get_wtime();
            for (int d = arLen / 2; d > 0; d /= 2)
            {
                int i, j, x, k;
                #pragma omp parallel for num_threads(threadCount) shared(arrays, arId, arLen, d) private(i, j, x, k) default(none)
                for (i = 0; i < d; ++i)
                {
                    for (j = i + d; j < arLen; j += d)
                    {
                        x = arrays[arId][j];
                        k = j - d;
                        while (k >= i && arrays[arId][k] > x)
                        {
                            arrays[arId][k + d] = arrays[arId][k];
                            k -= d;
                        }
                        arrays[arId][k + d] = x;
                    }
                }
            }
            totalTime += omp_get_wtime() - startTime;
        }
        avTime[measId] = totalTime / arNum;
    }
    printAvTime(threadsNum, avTime, threadNumsLen);
    printf("\n");
    return 0;
}
