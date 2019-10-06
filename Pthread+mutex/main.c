#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock;

// locked adding function
void* adder(int *sum, int value)
{
    pthread_mutex_lock(&lock);
    *sum += value;
    pthread_mutex_unlock(&lock);
    return NULL;
} 

// second process
void* sum_runner(void *arg)
{
    int **args = (int**) arg;
    int sum_temp = 0;
    for (int i = *args[1]/2; i <= *args[1]; ++i)
    {
        sum_temp += i;
    }
    adder(args[0], sum_temp);
    pthread_exit(0);
}

int main(int argc, char **argv)
{
    // check if input is correct
    if (argc < 2) {
        printf("Usage: %s <num>\n", argv[0]);
        exit(-1);
    }

    // variable for collecting sum (default is 0)
    int sum = 0;
    // variable which tells us which number to count to (get from input)
    // getting input
    int limit = 0;
    limit = atoi(argv[1]);

    // array for pulling args to func
    int *argum[2];
    argum[0] = &sum;
    argum[1] = &limit;

    // creating pthread (id (random), attr (random), constructor(attr), pthread creation)
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, sum_runner, &argum);

    int sum_temp = 0;
    // make that process part
    for (int i = 0; i < limit / 2; ++i) {
        sum_temp+=i;
    }
    adder(&sum, sum_temp);

    // wait untill pthread is done it's calculating part
    pthread_join(tid, NULL);

    // print result
    printf("result: %d\n", sum);

    // destroying mutex lock
    pthread_mutex_destroy(&lock);

    return 0;
}



// Пофиксить ADder (мьютекс должен использоваться реже, только для 1 суммы)
