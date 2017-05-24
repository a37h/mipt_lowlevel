#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <pthread.h>
#include <time.h>
// my includes here
#include "gol.h"
#include "My_semaphore.h"

pthread_mutex_t lock;

void* Alloc_in_shm(int shmid, int size, char* address)
{
    void *p;                    // shared variable
    key_t shmkey;
    shmkey = ftok(address, 5);
    printf ("shmkey1 for p1 = %d\n", shmkey);
    shmid = shmget (shmkey, size, 0644 | IPC_CREAT);
    if (shmid < 0) {
        perror ("shmget\n");
        exit (1);
    }
    p = (void *) shmat(shmid, NULL, 0);
    printf ("p1= is allocated in shared memory.\n\n");
    return p;
}

int main (int argc, char **argv){

    int m;
    printf ("How big plaing field do you want to get (m*m)?\nPlease, enter 'm': ");
    scanf ("%u", &m);

/********************************************************************************************************/

    int shmid1;                 // shared memory id
    int **ptr1 = (int **) Alloc_in_shm(shmid1, m*m*sizeof (int), "/home/quasar/shmstuff/shm1");

    int shmid2;
    int **ptr2 = (int **) Alloc_in_shm(shmid2, m*m*sizeof (int), "/home/quasar/shmstuff/shm2");

    int shmid3;
    struct Qsem* ptr3 = (struct Qsem*) Alloc_in_shm(shmid3, sizeof(*ptr3), "/home/quasar/shmstuff/shm3");

    ptr2[0] = 0;

    fill_randomization(ptr1,m);

//    int temp;
//    for (temp = 0; temp < m*m; temp++)
//    {
//        p1[temp] = temp;
//        p2[temp] = temp*3;
//    }
//    show_matrix(p1,m);
//    show_matrix(p2,m);
//    shmdt (p1);
//    shmdt (p2);
//    shmctl(shmid1, IPC_RMID, NULL);
//    shmctl(shmid2, IPC_RMID, NULL);
//    return 0;

/********************************************************************************************************/

    int i = 0;
    sem_t *sem1;
    sem_t *sem2;
    pid_t pid;
    unsigned int n;
    unsigned int value;

//    printf ("How many processes you want to work (n*n)?\n");
//    printf ("Fork count: ");
//    scanf ("%u", &n);
    // knock knock
    // who's there
    // no decision in amount of forks is THERE
    // i will add customisation someday i guess but im not sure

    n = 2;

    // well...

    int amount_of_forks = n*n;
    value = amount_of_forks;

    sem1 = sem_open ("pSem1", O_CREAT | O_EXCL, 0644, 0);
    sem2 = sem_open ("pSem2", O_CREAT | O_EXCL, 0644, 0);
    sem_unlink ("pSem1");
    sem_unlink ("pSem2");
    printf ("semaphores initialized.\n\n");

    ptr3->sem1 = sem1;
    ptr3->sem2 = sem2;
    ptr3->value = value;
    ptr3->amount_of_started = 0;
    ptr3->amount_of_ended = 0;

    srand(time(NULL));
    int r = 0;
    /* fork child processes */
    for (i = 0; i < amount_of_forks; i++) {
        pid = fork ();  // should only be called once
        if (pid < 0)              /* check for error      */
            printf ("Fork error.\n");
        else if (pid == 0)
            break;                  /* child processes */
    }

    int myleftborder, myrightborder, mytopborder, mybottomborder;
    if (i % 2 == 0)
    {
        myleftborder = 0;
        myrightborder = m/2;
    }
    else
    {
        myleftborder = m/2+1;
        myrightborder = m-1;
    }
    if (i < 2)
    {
        mytopborder = 0;
        mybottomborder = m/2;
    }
    else
    {
        mytopborder = m/2+1;
        mybottomborder = m-1;
    }
    //map looking something like that:
    // 0 1
    // 2 3
    // numbers = process pid

    ptr1[0] = 1;

/********************************************************************************************************/

/******************************************************/
/******************   PARENT PROCESS   ****************/
/******************************************************/
    if (pid != 0){
        /* wait for all children to exit */
        while (pid = waitpid (-1, NULL, 0)){
            if (errno == ECHILD)
                break;
        }

        printf ("\nParent: All children have exited.\n");

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
        exit (0);
    }

/******************************************************/
/******************   CHILD PROCESS   *****************/
/******************************************************/
    else{
        int a = 0;
        for ( a = 0; a < 10; a++)
        {
            Qsem_sem_start(ptr3);

            printf("Im a child(%d) and my LB=%i,RB=%i,TB=%i,BB=%i\n", i, myleftborder,myrightborder,mytopborder,mybottomborder);

//            pthread_mutex_lock(&lock);
//            for (int i = myleftborder; i <= myrightborder; ++i) {
//                for (int j = mybottomborder; j <= mytopborder; ++j) {
//                    int Amount_of_neighbours = Get_amount_of_neighbours(ptr1,m,i,j);
//                    if (Amount_of_neighbours < 2 || Amount_of_neighbours > 3) ptr2[i][j]=0;
//                    else if (Amount_of_neighbours == 2) ptr2[i][j]=ptr1[i][j];
//                    else ptr2[i][j]=1;
//                }
//            }
//            pthread_mutex_unlock(&lock);

            Swap(ptr1,ptr2);

            Qsem_sem_end(ptr3, ptr1, m);
        }
        exit(0);
    }
}