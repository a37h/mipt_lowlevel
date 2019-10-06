void CreatingForks(int area_x0, int area_y0, int area_x1, int area_y1) {
/*****************************************************************************************************/
/*********************************** Getting addinitional inputs *************************************/
    int conn_amount;
    int conn_number;
    read(socket_id, &conn_amount, sizeof(int));
    read(socket_id, &conn_number, sizeof(int));
/*****************************************************************************************************/
/******************************* Creating shared memory for processes ********************************/
    char str[1024];
    sprintf(str, "%d", conn_number+1000);
    int shm = shm_open(str, O_CREAT|O_RDWR, 0777);
    ftruncate(shm, sizeof(int)*WIDTH*HEIGHT*2 + sizeof(int));
/*****************************************************************************************************/
/************************************* MMaping shared memory *****************************************/
    point = (int*)mmap(0, sizeof(bool)*WIDTH*HEIGHT, PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    is_change = (int*)mmap(0, sizeof(int)*WIDTH*HEIGHT, PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    is_change += sizeof(int)*WIDTH*HEIGHT;
    changed = (int*)mmap(0, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    changed += sizeof(int)*WIDTH*HEIGHT*2;
/*****************************************************************************************************/
/**************************************** Semaphores part ********************************************/
    char names[PROC_AMOUNT + 1];
    for (int i = 0; i <= PROC_AMOUNT; i++) { names[i] = i + 100 + conn_amount + PROC_AMOUNT*(conn_number+1); }
    for (int i = 0; i <= PROC_AMOUNT; i++) { sem_unlink(&names[i]); }
    sem = sem_open(&names[0], O_CREAT, 0777, 0);
/*****************************************************************************************************/
/**************************************** Semaphores part ********************************************/
    int area_len = area_y1*WIDTH+area_x1-area_y0*WIDTH-area_x0;
    int arr_len = area_len / PROC_AMOUNT;
    std::cout << "┃ Current client array len: " << area_len << std::endl;
    std::cout << "┃ Clint processes array len: " << arr_len << std::endl;
    std::cout << "┣━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
    std::cout << "┣ You can stop current client at any time by pressing Ctrl+C ot Ctrl+Z" << std::endl;
    std::cout << "┣ The playing field is going to be saved, but if there is at least 1" << std::endl;
    std::cout << "┣ another active client then game will just freeze." << std::endl;
    std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
    int pids[PROC_AMOUNT];
    int x0, y0, x1, y1;
    x0 = area_x0;
    y0 = area_y0;
    x1 = (x0 + arr_len) % WIDTH;
    y1 = (y0 + (x0 + arr_len) / WIDTH);
/*****************************************************************************************************/
/***************************************** Forking part **********************************************/
    for (int i = 0; i < PROC_AMOUNT; i++) {
        sems[i] = sem_open(&names[i+1], O_CREAT, 0777, 0);
        if (i != 0 && i == PROC_AMOUNT - 1) {
            x0 = x1;
            y0 = y1;
            x1 = area_x1;
            y1 = area_y1;
        } else if (i != 0) {
            x0 = x1;
            y0 = y1;
            x1 = (x0 + arr_len) % WIDTH;
            y1 = (y0 + (x0 + arr_len) / WIDTH);
        }
        pids[i] = fork();
        if (pids[i] == 0) {
            SingleWorker(x0, y0, x1, y1, i);
            exit(EXIT_SUCCESS);
        }
    }
/*****************************************************************************************************/
/************************ Checking if field doesn't change anymore ***********************************/
    int step = 0;
    *changed = 1;
    int is_end;
    while (true) {
        read(socket_id, point, sizeof(int)*WIDTH*HEIGHT);
        read(socket_id, &is_end, sizeof(int));
        if (is_end == 2) break;
        for (int i = 0; i < PROC_AMOUNT; i++) {
            sem_post(sems[i]);//начал 1 часть
        }
        for (int i = 0; i < PROC_AMOUNT; i++) {
            sem_wait(sem);//закончил 1 ч.
        }
        write(socket_id, changed, sizeof(int));
        for (int i = 0; i < PROC_AMOUNT; i++) {
            sem_post(sems[i]);//начал 2 ч.
        }
        for (int i = 0; i < PROC_AMOUNT; i++) {
            sem_wait(sem);//закончил 2 ч.
        }
        if (*changed) {
            write(socket_id, point+area_y0*WIDTH+area_x0,
                  sizeof(int)*(area_y1*WIDTH+area_x1-area_y0*WIDTH-area_x0));
        }
        *changed = 0;
        step++;
    }
    *changed = 2;
    for (int i = 0; i < PROC_AMOUNT; i++) {
        sem_post(sems[i]);
    }
/*****************************************************************************************************/
/******************************** Sync part to finish each process ***********************************/
    for (int i = 0; i < PROC_AMOUNT; i++) {
        wait();
    }
    std::cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓" << std::endl;
    std::cout << "┃ Game has ended without errors. ┃" << std::endl;
    std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛" << std::endl;
    for (int i = 0; i <= PROC_AMOUNT; i++) {
        sem_unlink(&names[i]);
    }
/*****************************************************************************************************/
/******************************* Clearing all the shared objects *************************************/
    munmap(point, sizeof(int)*WIDTH*HEIGHT*2 + sizeof(int));
    shm_unlink(str);
}