struct Qsem {
    int value; // processes total amount

    sem_t* sem1;
    int amount_of_started;

    sem_t* sem2;
    int amount_of_ended;

    int* matrix1;
    int* matrix2;
    int size;
    int** p1;
    int** p2;
//    pthread_mutex_t* lock;
};

void Qsem_sem_start(struct Qsem *qsem){
//    pthread_mutex_unlock(qsem->lock);
    qsem->amount_of_started++;
    printf("%i ", qsem->amount_of_started);
//    if (qsem->amount_of_started == qsem->value) { Qsem_clear1(qsem); }
    sem_wait(qsem->sem1);
}

void Qsem_refresh_sem1(struct Qsem *qsem){
//    pthread_mutex_lock(qsem->lock);
    if (qsem->amount_of_started == qsem->value) {
        int i = 0;
        for (i = 0; i < qsem->value; i++)
            sem_post(qsem->sem1);
        qsem->amount_of_started = 0;
    }
//    pthread_mutex_unlock(qsem->lock);
}

void Qsem_sem_end(struct Qsem *qsem)
{
//    pthread_mutex_unlock(qsem->lock);
    qsem->amount_of_ended++;
    sem_wait(qsem->sem2);
}

void Qsem_refresh_sem2(struct Qsem *qsem){
//    pthread_mutex_lock(qsem->lock);
    if (qsem->amount_of_ended == qsem->value) {
        printf("\n$$$$");
//      show_matrix(qsem->matrix1, qsem->size);
//      show_matrix(qsem->matrix2, qsem->size);
        int i = 0;
        for (i = 0; i < qsem->value; i++)
            sem_post(qsem->sem2);
        qsem->amount_of_ended = 0;

        int *tmp = *qsem->p1;
        *qsem->p1 = *qsem->p2;
        *qsem->p2 = tmp;
    }
//    pthread_mutex_unlock(qsem->lock);
}