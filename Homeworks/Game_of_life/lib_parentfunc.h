/*********************************************************************************************************/
/********************************************* PARENT PROCESS ********************************************/

#include <pthread.h>

struct Args_box_for_Cpec{
    int* ptr1_c;
    int* ptr2_c;
    int* ptr3_c;
    sem_t* sem1_c;
    sem_t* sem2_c;
    int shmid1_c;
    int shmid2_c;
    int shmid3_c;
};

void Child_processes_exit_checker(void *arg)
{
    struct Args_box_for_Cpec * wrapped_args = (struct Args_box_for_Cpec *) arg;

    int *ptr1 = wrapped_args->ptr1_c;
    int *ptr2 = wrapped_args->ptr2_c;
    int *ptr3 = wrapped_args->ptr3_c;
    sem_t *sem1 = wrapped_args->sem1_c;
    sem_t *sem2 = wrapped_args->sem2_c;
    int shmid1 = wrapped_args->shmid1_c;
    int shmid2 = wrapped_args->shmid2_c;
    int shmid3 = wrapped_args->shmid3_c;
    pid_t pid;

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



void Fork_function__parent(int* ptr1, int* ptr2, int* ptr3, sem_t* sem1, sem_t* sem2,
                           int shmid1, int shmid2, int shmid3)
{
/*********************************************************************************************************/
/************ creating a Args_box_for_Cpec and pthread for Child_processes_exit_checker ******************/
    struct Args_box_for_Cpec test;
    test.ptr1_c = ptr1;
    test.ptr2_c = ptr2;
    test.ptr3_c = ptr3;
    test.sem1_c = sem1;
    test.sem2_c = sem2;
    test.shmid1_c = shmid1;
    test.shmid2_c = shmid2;
    test.shmid3_c = shmid3;
    void *argument_variable = (void *) &test;
    pthread_t tid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_create(&tid, &attr, Child_processes_exit_checker, &argument_variable);
/*********************************************************************************************************/
/************ creating a Args_box_for_Cpec and pthread for Child_processes_exit_checker ******************/

    pthread_join(tid, NULL);
    printf("Error: somehow you ended up there. [lib_parentfunc.h: void Fork_function__parent(...)]");
}