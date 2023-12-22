#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

int** makeArr(int rows, int columns)
{
    int **result = (int**) malloc(rows * sizeof(int*));
    for (int i = 0; i < rows; ++i)
    {
        result[i] = (int*) malloc(columns * sizeof(int));
    }
    return result;
}

void fillRand(int** arrays, int rows, int columns)
{
    const int random_seed = 920215;
    for (int i = 0; i < rows; ++i)
    {
        srand(random_seed);
        for (int j = 0; j < columns; ++j)
        {
            arrays[i][j] = rand();
        }
    }
}

void printAvTime(int* threadNums, double* avtime, int threadNumsLen)
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

void printAvCount(int* threadNums, int** avCount, int threadNumsLen) {
    char *fmt = "%-10d";
    int dellen = 10 * (threadNumsLen + 1);
    printDel(dellen);
    printf("#%-9d", "");
    for (int measId = 0; measId < 10; ++measId)
    {
        printf(fmt, threadNums[measId]);
    }
    printf("...%d, ...%d, ...%d\n", 16, 32, 64);
    printDel(dellen);
    for (int measId = 0; measId < threadNumsLen; ++measId)
    {
        printf(fmt, threadNums[measId]);
        for (int arrId = 0; arrId < threadNumsLen; ++arrId)
        {
            printf(fmt, avCount[measId][arrId]);
        }
        printf("\n");
    }
    printDel(dellen);
}

int main(int argc, char* argv[]) {

    // Determine the OpenMP support
    printf("OpenMP: %d;\n======\n", _OPENMP);

    // Init data
    const int arLen = 10000000;
    const int arNum = 10;
    int threadNumsLen = 13;
    int threadsNum[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 16, 32, 64};

    // Init measurement arrays
    int **avCount = makeArr(threadNumsLen, 64);
    double *avTime = (double*) malloc(threadNumsLen * sizeof(double));

    // Init arrays
    int **arrays = makeArr(arNum, arLen);
    fillRand(arrays, arNum, arLen);


    // Start measuring
    for (int measId = 0; measId < threadNumsLen; ++measId) {
        int threadCount = threadsNum[measId];
        double totalTime = 0;
        for (int arId = 0; arId < arNum; ++arId) {
            int max = -1;
            double startTime = omp_get_wtime();
            #pragma omp parallel num_threads(threadCount) shared(arId, totalTime, arrays, arLen) reduction(max: max) default(none)
            {
                #pragma omp for
                for (int posArr = 0; posArr < arLen; ++posArr) {
                    if (arrays[arId][posArr] > max) {
                        max = arrays[arId][posArr];
                    }
                }
            }
            totalTime += omp_get_wtime() - startTime;
        }
        avTime[measId] = totalTime / arNum;
        for (int arId = 0; arId < arNum; ++arId) {
            int max = -1;
            #pragma omp parallel num_threads(threadCount) shared(measId, arId, avCount, arrays, arLen) reduction(max: max) default(none)
            {
                #pragma omp for
                for (int posArr = 0; posArr < arLen; ++posArr) {
                    avCount[measId][omp_get_thread_num()] += 1;
                    if (arrays[arId][posArr] > max)
                    {
                        max = arrays[arId][posArr];
                    }
                }
            }
        }
        for (int threadId = 0; threadId < threadsNum[measId]; ++threadId)
        {
            avCount[measId][threadId] /= arNum;
        }
    }
    printAvTime(threadsNum, avTime, threadNumsLen);
    printf("\n");
    printAvCount(threadsNum, avCount, threadNumsLen);
    return 0;
}