struct Qsem {
    int value; // keeps the amount of processes
    sem_t *sem1;
    sem_t *sem2;
    int counter;
    int amount_of_started;
    int amount_of_ended;
} qse;

void Qsem_sem_start(struct Qsem *qsem){
    qsem->amount_of_started++;
    if (qsem->amount_of_started == qsem->value) { Qsem_clear1(qsem); }
    sem_wait(qsem->sem1);
}

void Qsem_clear1(struct Qsem *qsem){
    int i = 0;
    for (i = 0; i < qsem->value; i++)
        sem_post(qsem->sem1);
    qsem->amount_of_started = 0;

}

void Qsem_sem_end(struct Qsem *qsem, int **matrix1,int **matrix2, int size, int **p1, int **p2){
    qsem->amount_of_ended++;
    if (qsem->amount_of_ended == qsem->value) {
        qsem->counter++;
        printf("\n   $$$%i$$$   ",qsem->counter);
        show_matrix(matrix1,size);
        show_matrix(matrix2,size);
        int i = 0;
        for (i = 0; i < qsem->value; i++)
            sem_post(qsem->sem2);
        qsem->amount_of_ended = 0;


        int *tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
    }
    sem_wait(qsem->sem2);
}

// This part should be reworked
// Semaphores should be controlled by main process