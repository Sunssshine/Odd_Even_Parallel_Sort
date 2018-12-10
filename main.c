#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <termios.h>
#include <unistd.h>

#define TRUE 1
#define FALSE 0
#define DEFAULT_THREADS_NUMBER 4
#define DEFAULT_NUMBER_OF_ELEMENTS 100000000

////THREADS DATA BLOCK////
int ** threadsArrays;
int * threadsArraysLengths;
//THREADS DATA BLOCK END//

void swap(int * first, int * second);

void myqsort(int begin, int end, int * array);

int * genRandArray(int maxValue, int length, int isNegative);

void printArray(int * array, int length);

int * mergeSortedArrays(const int * fArray, int fArrayLen, const int * sArray, int sArrayLen);

void * threadSort(void *params);

void * threadMerge(void *params);

void oddEvenParallelSort(int threadsCount, int * array, int length);

void parseArguments(int argc, char * argv[], int * threadsCount, int * numberOfElements);

int checkSorted(int * array, int length);

char getch();

int main(int argc, char * argv[])
{


    srand((unsigned int) time(NULL));
    int numberOfElements;
    int threadsCount;
    int maxGenValue = 10000;
    int isNegative = FALSE;

    printf("\n------------\n"
           "Initializing\n"
           "------------\n\n");

    parseArguments(argc, argv, &threadsCount, &numberOfElements);

    printf("\nGenerate random array contains %d elements...\n", numberOfElements);
    int * randomArray = genRandArray(maxGenValue, numberOfElements, isNegative);
    //printArray(randomArray, numberOfElements);

    int * arrayForSort = (int*)malloc(sizeof(int)*numberOfElements);

    long startTime;
    long finishTime;
    long resultTime;
    struct timeval  tv;


    /////////////////////////////ODD EVEN SORT BLOCK////////////////////////////////////////////
    printf("\nPress ENTER to start ODD-EVEN parallel sort: ");
    getch();
    memcpy(arrayForSort, randomArray, numberOfElements * sizeof(int));
    gettimeofday(&tv, NULL);
    startTime = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;

    oddEvenParallelSort(threadsCount, arrayForSort, numberOfElements);

    gettimeofday(&tv, NULL);
    finishTime = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
    resultTime = finishTime - startTime;

    //printArray(arrayForSort, numberOfElements);
    if(checkSorted(arrayForSort, numberOfElements))
        printf("\nIs sorted\n");
    else
        printf("\n!!!Is not sorted!!!\n");

    printf("\n|ODD-EVEN| Time for %d threads and %d array length: %ld ms\n", threadsCount, numberOfElements, resultTime);
    /////////////////////////////ODD EVEN SORT BLOCK END////////////////////////////////////////////

    /////////////////////////////QSORT BLOCK////////////////////////////////////////////
    printf("\nPress ENTER to start QSORT sort: ");
    getch();

    memcpy(arrayForSort, randomArray, numberOfElements * sizeof(int));
    gettimeofday(&tv, NULL);
    startTime = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;

    myqsort(0, numberOfElements, arrayForSort);

    gettimeofday(&tv, NULL);
    finishTime = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
    resultTime = finishTime - startTime;

    //printArray(arrayForSort, numberOfElements);
    if(checkSorted(arrayForSort, numberOfElements))
        printf("\nIs sorted\n");
    else
        printf("\n!!!Is not sorted!!!\n");

    printf("\n|QSORT| Time for %d array length: %ld ms", numberOfElements, resultTime);
    /////////////////////////////QSORT BLOCK END////////////////////////////////////////////
    free(arrayForSort);
    free(randomArray);

    //printArray(arrayForSort, numberOfElements);
    return 0;
}

void swap(int * first, int * second)
{
    int tmp = *first;
    *first = *second;
    *second = tmp;
}

void myqsort(int begin, int end, int * array)
{

    int leftIterator = begin;
    int rightIterator = end;
    if(!end && !begin)
        return;
    int divideElem = array[(end+begin)/2];

    while(leftIterator <= rightIterator)
    {
        while(array[leftIterator] < divideElem)
            leftIterator++;
        while(array[rightIterator] > divideElem)
            rightIterator--;

        if(leftIterator <= rightIterator)
            swap(&array[leftIterator++], &array[rightIterator--]);

    }
    if(begin < rightIterator)
        myqsort(begin, rightIterator, array);

    if(end > leftIterator)
        myqsort(leftIterator, end, array);
}

int * genRandArray(int maxValue, int length, int isNegative)
{

    int * array = (int*)malloc(sizeof(int) * length);
    int i = 0;
    for(i = 0; i<length; i++)
    {
        array[i] = rand() % maxValue;
        if(isNegative)
            array[i] -= maxValue/2;
    }
    return array;
}

void printArray(int * array, int length)
{
    int i = 0;
    printf("\n\n-----------------------------");
    for(i = 0; i<length; i++)
    {
        if(!(i%20))
        {
            printf("\n");
        }
        printf("%d ",array[i]);

    }
}

int * mergeSortedArrays(const int * fArray, int fArrayLen, const int * sArray, int sArrayLen)
{
    // each of arrays should be sorted previously
    int fArrayCounter = 0;
    int sArrayCounter = 0;
    int totalCounter = 0;
    int * mergedArray = (int*)malloc(sizeof(int)*(fArrayLen+sArrayLen));
    while(totalCounter < fArrayLen+sArrayLen)
    {

        if(((fArray[fArrayCounter] < sArray[sArrayCounter]) || (sArrayCounter >= sArrayLen))
            && (fArrayCounter < fArrayLen))
        {
            mergedArray[totalCounter] = fArray[fArrayCounter];
            fArrayCounter++;
        }
        else
        {
            mergedArray[totalCounter] = sArray[sArrayCounter];
            sArrayCounter++;
        }
        totalCounter++;
    }
    return mergedArray;
}

void *threadSort(void *params) {
    int id = ((int*)params)[0];
    myqsort(0,threadsArraysLengths[id]-1, threadsArrays[id]);

    free((int*)params);
    pthread_exit(0);
}

void *threadMerge(void *params) {
    int id = ((int*)params)[0];
    int * mergedArray = mergeSortedArrays(threadsArrays[id], threadsArraysLengths[id],
                                          threadsArrays[id+1], threadsArraysLengths[id+1]);

    memcpy(threadsArrays[id], mergedArray, sizeof(int)*threadsArraysLengths[id]);

    memcpy(threadsArrays[id+1], &mergedArray[threadsArraysLengths[id]], sizeof(int)*threadsArraysLengths[id+1]);

    free((int*)params);
    free(mergedArray);
    pthread_exit(0);
}

int checkSorted(int *array, int length)
{
    int i;
    for(i = 1; i<length; i++)
    {
        if (array[i] < array[i-1])
            return FALSE;
    }
    return TRUE;
}

void parseArguments(int argc, char **argv, int *threadsCount, int *numberOfElements) {
    switch (argc)
    {
        case 1:
            *threadsCount = DEFAULT_THREADS_NUMBER;
            *numberOfElements = DEFAULT_NUMBER_OF_ELEMENTS;
            printf("Whoa u didn`t say number of threads and even number of elements to me, well, ok...\n"
                   "* Number of threads is: %d\n"
                   "* Number of elements is: %d\n"
                   "- SHAME ON YOU -\n",
                   DEFAULT_THREADS_NUMBER, DEFAULT_NUMBER_OF_ELEMENTS);

            break;

        case 2:
            *threadsCount = atoi(argv[1]);
            *numberOfElements = DEFAULT_NUMBER_OF_ELEMENTS;
            printf("Whoa u didn`t say number of elements to me, well, ok, it's %d now. ._.\n", DEFAULT_NUMBER_OF_ELEMENTS);
            break;

        case 3:
            *threadsCount = atoi(argv[1]);
            *numberOfElements = atoi(argv[2]);

            printf("* Number of threads is: %d\n"
                   "* Number of elements is: %d\n",
                   *threadsCount, *numberOfElements);
            break;

        default:
            *threadsCount = atoi(argv[1]);
            *numberOfElements = atoi(argv[2]);

            printf("Whoa, I got too much arguments so...\nI will just ignore all except first two, alright? #_#\n"
                   "* Number of threads is: %d\n"
                   "* Number of elements limit is: %d\n",
                   *threadsCount, *numberOfElements);
            break;
    }

    if(*threadsCount > *numberOfElements)
        *threadsCount = *numberOfElements;
}

void oddEvenParallelSort(int threadsCount, int *array, int length)
{
    threadsArrays = (int**)malloc(sizeof(int*)*threadsCount);
    threadsArraysLengths = (int*)malloc(sizeof(int)*threadsCount);
    int i = 0;

    int numberForEach = length/threadsCount;
    int restForLast = length%threadsCount;
    for (i = 0; i < threadsCount; ++i)
    {
        if(i == threadsCount-1)
        {
            threadsArrays[i] = &array[numberForEach*i];
            threadsArraysLengths[i] = numberForEach+restForLast;
            break;
        }
        threadsArrays[i] = &array[numberForEach*i];
        threadsArraysLengths[i] = numberForEach;
    }

    pthread_t * threads = (pthread_t*)malloc(sizeof(pthread_t)*threadsCount);

    for (i = 0; i < threadsCount; ++i) {
        int * params = (int*)malloc(sizeof(int));
        *params = i;
        pthread_create(&threads[i], NULL, threadSort, params);
    }

    for (i = 0; i < threadsCount; ++i)
    {
        pthread_join(threads[i], NULL);
    }

    for(i = 0; i<threadsCount; i++)
    {
        int j;
        if(i % 2) {
            j = 1;
        }
        else {
            j = 0;
        }

        for(; j<threadsCount-1; j+=2)
        {
            int * params = (int*)malloc(sizeof(int));
            *params = j;
            pthread_create(&threads[j], NULL, threadMerge, params);
        }

        if(i % 2) {
            j = 1;
        }
        else {
            j = 0;
        }
        for(; j<threadsCount-1; j+=2)
        {
            pthread_join(threads[j], NULL);
        }
    }
    free(threads);
    free(threadsArrays);
    free(threadsArraysLengths);
}

char getch(){
    char buf=0;
    struct termios old={0};
    fflush(stdout);
    if(tcgetattr(0, &old)<0)
        perror("tcsetattr()");
    old.c_lflag&=~ICANON;
    old.c_lflag&=~ECHO;
    old.c_cc[VMIN]=1;
    old.c_cc[VTIME]=0;
    if(tcsetattr(0, TCSANOW, &old)<0)
        perror("tcsetattr ICANON");
    if(read(0,&buf,1)<0)
        perror("read()");
    old.c_lflag|=ICANON;
    old.c_lflag|=ECHO;
    if(tcsetattr(0, TCSADRAIN, &old)<0)
        perror ("tcsetattr ~ICANON");
    return buf;
}
