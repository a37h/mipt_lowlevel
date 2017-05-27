int* point = NULL;
int* changed = NULL;
sem_t *sem;
sem_t *sems[PROC_AMOUNT];
int num_connected = 0;
int amount_of_evolutions_to_show = 10;