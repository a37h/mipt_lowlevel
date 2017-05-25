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
#include "lib_semaphore.h"
#include "lib_matrixwork.h"

int main (int argc, char **argv) {
/*********************************************************************************************************/
/**************************** Getting field size and allocating needed memory ****************************/

    int size;
    printf("Field size (n*n): ");
    scanf("%u", &size);
    size += 2;

    int shmid1;
    int *ptr1 = (int *) Alloc_in_shm(shmid1, size*size*sizeof(int), "/home/quasar/shmstuff/shm1");
    Clear_the_array(ptr1, size);

    int shmid2;
    int *ptr2 = (int *) Alloc_in_shm(shmid2, size*size*sizeof(int), "/home/quasar/shmstuff/shm2");
    Clear_the_array(ptr2, size);

    int shmid3;
    struct Qsem *ptr3 = (struct Qsem *) Alloc_in_shm(shmid3, sizeof(*ptr3), "/home/quasar/shmstuff/shm3");
    fill_randomization(ptr1, size);

/*********************************************************************************************************/
/******************************** Creating semaphores and semaphore "box" ********************************/

    sem_t *sem1, *sem2;
    pid_t pid;
    unsigned int i, n, value, amount_of_forks;

    //printf ("How many processes you want to work (n*n)?\n");
    //printf ("Fork count: ");
    //scanf ("%u", &amount_of_forks);
    amount_of_forks = 4;

    sem1 = sem_open ("pSem1", O_CREAT | O_EXCL, 0644, 0);
    sem2 = sem_open ("pSem2", O_CREAT | O_EXCL, 0644, 0);
    sem_unlink ("pSem1");
    sem_unlink ("pSem2");
    printf ("semaphores initialized.\n\n");

    ptr3->sem1 = sem1;
    ptr3->sem2 = sem2;
    ptr3->value = amount_of_forks;
    ptr3->amount_of_started = 0;
    ptr3->amount_of_ended = 0;
    ptr3->counter = 0;

/*********************************************************************************************************/
/***************************************** Creating fork processes ***************************************/

    for (i = 0; i < amount_of_forks; i++) { pid = fork ();
        if (pid < 0) printf ("Fork error.\n");
        else if (pid == 0) break; }

/*********************************************************************************************************/
/********************************************* PARENT PROCESS ********************************************/
    if (pid != 0){
        /* wait for all children to exit */
        while (pid = waitpid (-1, NULL, 0)){
            if (errno == ECHILD)
                break;
        }
        /* shared memory detach */
        shmdt (ptr1);
        shmdt (ptr2);
        shmctl(shmid1, IPC_RMID, NULL);
        shmctl(shmid2, IPC_RMID, NULL);

        /* cleanup semaphores */
        sem_destroy (sem1);
        sem_destroy (sem2);

        printf ("\nParent: All children have exited.\n");
        exit (0);
    }

/*********************************************************************************************************/
/********************************************** CHILD PROCESS ********************************************/
    else{
        int myleftborder, myrightborder, mytopborder, mybottomborder;
        if (i % 2 == 0) { myleftborder = 1; myrightborder = size/2+1; }              // map looks like this
        else { myleftborder = size/2+2; myrightborder = size; }                                    // 0 1
        if (i < 2) { mytopborder = 1; mybottomborder = size/2+1; }                                 // 2 3
        else { mytopborder = size/2+2; mybottomborder = size; }

        int a = 0;
        for ( a = 0; a < 5; a++)
        {
//          Qsem_sem_start(ptr3);
//          printf("Im a child (%d)\n", i);

            printf("%i  ",a);
            sleep(1);

//          Qsem_sem_end(ptr3, ptr1, ptr2, size, &ptr1, &ptr2);
        }
        printf("Proccess %i died\n", i);
        exit(0);
    }

    printf("Error: somehow you ended up there.");
    return 0;
}