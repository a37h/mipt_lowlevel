#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* sum_runner(void *arg)
{
    int **args = (int**) arg;
//
//    *args[0] = 5;
//    *args[1] = 3;

//    long long limit = args[0];
//    long long *sum = (long long*) args[1];
//
//    for (long long i = 0; i <= limit; i++)
//    {
//        *sum += i;
//    }

    for (int i = *args[1]/2; i <= *args[1]; ++i)
    {
        *args[0] += i;
    }

    pthread_exit(0);
}

int main(int argc, char **argv) {

    // check if input is correct
    if (argc < 2) {
        printf("Usage: %s <num>\n", argv[0]);
        exit(-1);
    }

    // variable for collecting sum (default is 0)
    int sum = 0;
    int sum_p = 0;
    // variable which tells us which number to count to (get from input)
    // getting input
    int limit = 0;
    limit = atoi(argv[1]);

    // array for pulling args to func
    int *argum[2];
    argum[0] = &sum_p;
    argum[1] = &limit;


    // creating pthread (id (random), attr (random), constructor(attr), pthread creation)
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, sum_runner, &argum);

    for (int i = 0; i < limit / 2; ++i) {
        sum += i;
    }

    // wait untill pthread is done it's calculating part
    pthread_join(tid, NULL);

    sum += sum_p;

    printf("result: %d\n", sum);

    return 0;
}