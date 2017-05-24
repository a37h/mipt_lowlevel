#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <pthread.h>

#include <time.h>

#include "gol.h"

/******************************************************/
/**************   Semaphore incapsulated  *************/
/******************************************************/

pthread_mutex_t lock;

struct Qsem {
    int value; // keeps the amount of processes
    sem_t *sem1;
    sem_t *sem2;
    int amount_of_started;
    int amount_of_ended;
} qse;

// Кладем процессы только вошедшие в работу в сон
void Qsem_sem_start(struct Qsem *qsem){
    pthread_mutex_lock(&lock);
    qsem->amount_of_started++;
    if (qsem->amount_of_started == qsem->value) { Qsem_clear1(qsem); }
    sem_wait(qsem->sem1);
    pthread_mutex_unlock(&lock);
}

// Когда все процессы пришли уснуть (не уснул только тот, что вызывал эту функцию последний)
// Добавляем в семафор значения, тем самым будим процессы и даем им работать
void Qsem_clear1(struct Qsem *qsem){
    int i = 0;
    for (i = 0; i < qsem->value; i++)
        sem_post(qsem->sem1);
    qsem->amount_of_started = 0;

}

// Кладем процессы только вошедшие в работу в сон
void Qsem_sem_end(struct Qsem *qsem, int **matrix, int size){
    pthread_mutex_lock(&lock);
    qsem->amount_of_ended++;
    if (qsem->amount_of_ended == qsem->value) { Qsem_clear2(qsem,matrix,size); }
    sem_wait(qsem->sem2);
    pthread_mutex_unlock(&lock);
}

// Когда все процессы пришли уснуть (не уснул только тот, что вызывал эту функцию последний)
// Добавляем в семафор значения, тем самым будим процессы и даем им работать
void Qsem_clear2(struct Qsem *qsem, int **matrix, int size){
    int i = 0;
    show_matrix(matrix,size);
    for (i = 0; i < qsem->value; i++)
        sem_post(qsem->sem2);
    qsem->amount_of_ended = 0;
    printf("\n\n Each process has ended it's journey --------------------------------- \n\n");

}

void* alloc1(int shmid1, int neededsize, char* string)
{
    void *p1;                    // shared variable
    key_t shmkey1;
    shmkey1 = ftok(string,5);
    printf ("shmkey1 for p1 = %d\n", shmkey1);
    shmid1 = shmget (shmkey1, neededsize, 0644 | IPC_CREAT);
    if (shmid1 < 0) {
        perror ("shmget\n");
        exit (1);
    }
    p1 = (void *) shmat(shmid1, NULL, 0);
    printf ("p1= is allocated in shared memory.\n\n");
    return p1;
}

int main (int argc, char **argv){

    int m;
    printf ("How big plaing field do you want to get (m*m)?\nPlease, enter 'm': ");
    scanf ("%u", &m);

/**************************** shared memory related stuff *****************************/

    int shmid1;                 // shared memory id
    int **p1 = (int **) alloc1(shmid1, m*m*sizeof (int), "/home/quasar/shmstuff/shm1");

    int shmid2;
    int **p2 = (int **) alloc1(shmid2, m*m*sizeof (int), "/home/quasar/shmstuff/shm2");

    int shmid3;
    struct Qsem* my_sem = (struct Qsem*) alloc1(shmid3, sizeof(*my_sem), "/home/quasar/shmstuff/shm3");

    int temp;
    for (temp = 0; temp < m*m; temp++)
    {
        p1[temp] = temp;
        p2[temp] = temp*3;
    }

    show_matrix(p1,m);
    show_matrix(p2,m);

    shmdt (p1);
    shmdt (p2);
    shmctl(shmid1, IPC_RMID, NULL);
    shmctl(shmid2, IPC_RMID, NULL);

    return 0;


/**************************** semaphore and forks related stuff *****************************/
//
//    int i = 0;
//    sem_t *sem1;                   /*      synch semaphore         *//*shared */
//    sem_t *sem2;
//    pid_t pid;                    /*      fork pid                */
//    unsigned int n;               /*      fork count              */
//    unsigned int value;           /*      semaphore value         */
//
////    printf ("How many processes you want to work (n*n)?\n");
////    printf ("Fork count: ");
////    scanf ("%u", &n);
//    // knock knock
//    // who's there
//    // no decision in amount of forks is THERE
//    // i will add customisation someday i guess but im not sure
//
//    n = 2;
//
//    // well...
//
//    int amount_of_forks = n*n;
//    value = amount_of_forks;
//
//    /* initialize semaphores for shared processes */
//    sem1 = sem_open ("pSem1", O_CREAT | O_EXCL, 0644, 0);
//    sem2 = sem_open ("pSem2", O_CREAT | O_EXCL, 0644, 0);
//    /* name of semaphore is "pSem", semaphore is reached using this name */
//    sem_unlink ("pSem1");
//    sem_unlink ("pSem2");
//    /* unlink prevents the semaphore existing forever */
//    /* if a crash occurs during the execution         */
//    printf ("semaphores initialized.\n\n");
//
//    ptr3->sem1 = sem1;
//    ptr3->sem2 = sem2;
//    ptr3->value = value;
//    ptr3->amount_of_started = 0;
//    ptr3->amount_of_ended = 0;
//
//    srand(time(NULL));
//    int r = 0;
//    /* fork child processes */
//    for (i = 0; i < amount_of_forks; i++) {
//        pid = fork ();  // should only be called once
//        r = rand();      // returns a pseudo-random integer between 0 and RAND_MAX
//        if (pid < 0)              /* check for error      */
//            printf ("Fork error.\n");
//        else if (pid == 0)
//            break;                  /* child processes */
//    }
//
//    int myleftborder, myrightborder, mytopborder, mybottomborder;
//    if (i % 2 == 0)
//    {
//        myleftborder = 0;
//        myrightborder = m/2;
//    }
//    else
//    {
//        myleftborder = m/2+1;
//        myrightborder = m-1;
//    }
//    if (i < 2)
//    {
//        mytopborder = 0;
//        mybottomborder = m/2;
//    }
//    else
//    {
//        mytopborder = m/2+1;
//        mybottomborder = m-1;
//    }
//    //map looking something like that:
//    // 0 1
//    // 2 3
//    // numbers = process pid
//
///******************************************************/
///******************   PARENT PROCESS   ****************/
///******************************************************/
//    if (pid != 0){
//        /* wait for all children to exit */
//        while (pid = waitpid (-1, NULL, 0)){
//            if (errno == ECHILD)
//                break;
//        }
//
//        printf ("\nParent: All children have exited.\n");
//
//        printf ("\nShowing ptr1 and ptr2:\n");
//
//        show_matrix(ptr1,m);
//
//        /* shared memory detach */
//        shmdt (p);
//        shmctl (shmid, IPC_RMID, 0);
//
//        /* cleanup semaphores */
//        sem_destroy (sem1);
//        sem_destroy (sem2);
//        exit (0);
//    }
//
///******************************************************/
///******************   CHILD PROCESS   *****************/
///******************************************************/
//    else{
//        int a = 0;
//        for ( a = 0; a < 10; a++)
//        {
//            Qsem_sem_start(ptr3);
////            sleep(1);
//            for(int i = 0; i < m; i++)
//                ptr1[i] = i;
//            printf("Child(%d) has done what he had to be having done. His number: %i\n", i,ptr1[i]);
//
////          Work_over_an_area(ptr1,ptr2,m,myleftborder,myrightborder,mytopborder,mybottomborder);
//
//
//            char **tmp = *ptr1;
//            *ptr1 = *ptr2;
//            *ptr2 = tmp;
//
//            Qsem_sem_end(ptr3, ptr1, m);
//        }
//        exit(0);
//    }
}