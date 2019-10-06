void show_matrix(int *p1, int m) {
    printf("------------------------------------------");
    int i, j;
    for (i = 0; i < m; i++) { printf("\n");
        for (j = 0; j < m; j++) { printf("%i ",p1[i*m+j]); }}
    printf("\n------------------------------------------\n"); }

void mx_set(int *ptr, int size, int x, int y, int val) { ptr[x*size + y] = val; }

int mx_get(int *ptr, int size, int x, int y) { return ptr[x*size + y]; }

int mx_getneighbours (int **ptr, int size, int x, int y) { int counter = 0;
    counter += mx_get(ptr,size,x-1,y-1) + mx_get(ptr,size,x-1,y) + mx_get(ptr,size,x-1,y+1) + mx_get(ptr,size,x,y-1);
    counter += mx_get(ptr,size,x,y+1) + mx_get(ptr,size,x+1,y-1) + mx_get(ptr,size,x+1,y) + mx_get(ptr,size,x+1,y+1);
    return counter; }

void Clear_the_array(int **array, int size) { int i, j;
    for (i = 0; i < size; i++) { for (j = 0; j < size; j++) { array[i*size+j] = 0; }}}

void* Alloc_in_shm(int shmid, int size, char* address) { void *p;
    key_t shmkey;
    shmkey = ftok(address, 5);
    printf ("shmkey1 for p1 = %d\n", shmkey);
    shmid = shmget (shmkey, size, 0644 | IPC_CREAT);
    if (shmid < 0) { perror ("shmget\n");
        exit (1); }
    p = (void *) shmat(shmid, NULL, 0);
    printf ("p1= is allocated in shared memory.\n\n");
    return p; }

void fill_randomization(char** World, int size) { time_t t;
    srand((unsigned) time(&t));
    int i, world_space = size * size;
    for (i = 0; i < world_space; i++) { World[i] = 0; }
    for (i = 0; i < world_space * 0.3; i++) { World[rand() % world_space] = 1; }
    for (int i = 0; i < size; i++) {
        World[i] = 0;
        World[size*size-size+i] = 0;
        World[size*i] = 0;
        World[size*i+size-1] = 0; }}