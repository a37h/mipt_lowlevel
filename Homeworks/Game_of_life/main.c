#include <stdio.h>          /* printf()                 */
#include <stdlib.h>         /* exit(), malloc(), free() */
#include <sys/types.h>      /* key_t, sem_t, pid_t      */
#include <sys/shm.h>        /* shmat(), IPC_RMID        */
#include <errno.h>          /* errno, ECHILD            */
#include <semaphore.h>      /* sem_open(), sem_destroy(), sem_wait().. */
#include <fcntl.h>          /* O_CREAT, O_EXEC          */
#include <pthread.h>
#include <time.h>

/*************************************************/
/******** Next are custom (mine) includes ********/
#include "lib_semaphore.h"  // Box for semaphore
#include "lib_matrixwork.h" // All of the matrix funcs
#include "lib_childfunc.h"  // Child process program
#include "lib_parentfunc.h" // Parent process program

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
/******** If (pid!=0) is true, then we go to the parent process func, otherwise child process func *******/
    if (pid != 0) { Fork_function__parent(ptr1,ptr2,ptr3,sem1,sem2,shmid1,shmid2,shmid3, &pid); }
    else { Fork_function__child(ptr1, ptr2, ptr3, size, i); }

    printf("Error: somehow you ended up there.");
    return 0;
}