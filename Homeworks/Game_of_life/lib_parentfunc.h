/*********************************************************************************************************/
/********************************************* PARENT PROCESS ********************************************/

#include <pthread.h>

struct Args_box{
    int* ptr1_c;
    int* ptr2_c;
    int* ptr3_c;
    sem_t* sem1_c;
    sem_t* sem2_c;
    int shmid1_c;
    int shmid2_c;
    int shmid3_c;
    pid_t *pid_c;
};

void Fork_function__parent(int* ptr1, int* ptr2, int* ptr3, sem_t* sem1, sem_t* sem2,
                           int shmid1, int shmid2, int shmid3, pid_t *pid)
{
    struct Args_box test;
    test.ptr1_c = ptr1;
    test.ptr2_c = ptr2;
    test.ptr3_c = ptr3;
    test.sem1_c = sem1;
    test.sem2_c = sem2;
    test.shmid1_c = shmid1;
    test.shmid2_c = shmid2;
    test.shmid3_c = shmid3;
    test.pid_c = pid;

    produce_thread(&test);
}

void Child_processes_exit_checker(void *arg)
{
    struct Args_box * wrapped_args = (struct Args_box *) arg;

    int *ptr1 = wrapped_args->ptr1_c;
    int *ptr2 = wrapped_args->ptr2_c;
    int *ptr3 = wrapped_args->ptr3_c;
    sem_t *sem1 = wrapped_args->sem1_c;
    sem_t *sem2 = wrapped_args->sem2_c;
    int shmid1 = wrapped_args->shmid1_c;
    int shmid2 = wrapped_args->shmid2_c;
    int shmid3 = wrapped_args->shmid3_c;
    pid_t pid = *(wrapped_args->pid_c);

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

void produce_thread(struct Args_box* wrapped_args)
{
    void *argument_variable = (void *) wrapped_args;

    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, Child_processes_exit_checker, &argument_variable);
}