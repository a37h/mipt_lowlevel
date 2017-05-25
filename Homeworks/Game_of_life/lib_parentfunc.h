/*********************************************************************************************************/
/********************************************* PARENT PROCESS ********************************************/

#include <pthread.h>

void Fork_function__parent(int* ptr1, int* ptr2, int* ptr3, sem_t* sem1, sem_t* sem2,
                           int shmid1, int shmid2, int shmid3, pid_t *pid)
{
    produce_thread(ptr1, ptr2, ptr3, sem1, sem2, shmid1, shmid2, shmid3, pid);
}


void Child_processes_exit_checker(void *arg)
{
    while (pid = waitpid (-1, NULL, 0)){
        if (errno == ECHILD)
            break;
    }
    /* shared memory detach */
    shmdt (ptr1);
    shmdt (ptr2);
    shmdt (ptr3);
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);
    shmctl(shmid3, IPC_RMID, NULL);
    /* cleanup semaphores */
    sem_destroy (sem1);
    sem_destroy (sem2);
    printf ("\nParent: All children have exited.\n");
    exit (0);
}



void produce_thread(int* ptr1, int* ptr2, int* ptr3, sem_t* sem1, sem_t* sem2,
                    int shmid1, int shmid2, int shmid3, pid_t *pid)
{
    void* arguments_array[9];
    arguments_array[0] = (void *) ptr1;
    arguments_array[1] = (void *) ptr2;
    arguments_array[2] = (void *) ptr3;
    arguments_array[3] = (void *) sem1;
    arguments_array[4] = (void *) sem2;
    arguments_array[5] = (void *) &shmid1;
    arguments_array[6] = (void *) &shmid2;
    arguments_array[7] = (void *) &shmid3;
    arguments_array[8] = (void *) pid;

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, Child_processes_exit_checker, &arguments_array);
}



//
//int not_main(int argc, char **argv)
//{
//    // array for pulling args to func
//    int *argum[2];
//    argum[0] = &sum;
//    argum[1] = &limit;
//
//    // creating pthread (id (random), attr (random), constructor(attr), pthread creation)
//    pthread_t tid; // it stores pthread id (which is unique)
//    pthread_attr_t attr;
//    pthread_attr_init(&attr);
//    pthread_create(&tid, &attr, sum_runner, &argum);
//
//    int sum_temp = 0;
//    // make that process part
//    for (int i = 0; i < limit / 2; ++i) {
//        sum_temp+=i;
//    }
//    adder(&sum, sum_temp);
//
//    // wait untill pthread is done it's calculating part
//    pthread_join(tid, NULL);
//
//    // print result
//    printf("result: %d\n", sum);
//
//    // destroying mutex lock
//    pthread_mutex_destroy(&lock);
//
//    return 0;
//}

//pthread_mutex_t lock;
// locked adding function
//void* adder(int *sum, int value)
//{
//    pthread_mutex_lock(&lock);
//    *sum += value;
//    pthread_mutex_unlock(&lock);
//    return NULL;
//}
// second process
//void* sum_runner(void *arg)
//{
//    int **args = (int**) arg;
//    int sum_temp = 0;
//    for (int i = *args[1]/2; i <= *args[1]; ++i)
//    {
//        sum_temp += i;
//    }
//    adder(args[0], sum_temp);
//    pthread_exit(0);
//}