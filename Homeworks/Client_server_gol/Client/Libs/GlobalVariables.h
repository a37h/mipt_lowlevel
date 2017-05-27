#define STR_SIZE 1024
int* point = NULL;
int* is_change = NULL;
int* changed = NULL;
char str[STR_SIZE];
sem_t *sem;
sem_t *sems[PROC_AMOUNT];
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int socket_id;