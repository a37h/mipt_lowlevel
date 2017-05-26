void Main_f(int area_x0, int area_y0, int area_x1, int area_y1) {
    int conn_amount;
    int conn_number;
    read(socket_id, &conn_amount, sizeof(int));
    read(socket_id, &conn_number, sizeof(int));
    char str[1024];
    sprintf(str, "%d", conn_number+1000);
    std::cout << "shm name " << str << std::endl;
    int shm = shm_open(str, O_CREAT|O_RDWR, 0777);
    ftruncate(shm, sizeof(int)*WIDTH*HEIGHT*2 + sizeof(int));

    point = (int*)mmap(0, sizeof(bool)*WIDTH*HEIGHT, PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);

    is_change = (int*)mmap(0, sizeof(int)*WIDTH*HEIGHT, PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    is_change += sizeof(int)*WIDTH*HEIGHT;

    changed = (int*)mmap(0, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, shm, 0);
    changed += sizeof(int)*WIDTH*HEIGHT*2;

    char names[PROC_AMOUNT + 1];
    for (int i = 0; i <= PROC_AMOUNT; i++) {
        names[i] = i + 100 + conn_amount + PROC_AMOUNT*(conn_number+1);
        std::cout << "sem name begin: " << names[i] << std::endl;
    }
    for (int i = 0; i <= PROC_AMOUNT; i++) {
        sem_unlink(&names[i]);
    }
    sem = sem_open(&names[0], O_CREAT, 0777, 0);
    int area_len = area_y1*WIDTH+area_x1-area_y0*WIDTH-area_x0;
    int arr_len = area_len / PROC_AMOUNT;
    std::cout << "area len: " << area_len << std::endl;
    std::cout << "len of each array for worker: " << arr_len << std::endl;
    int pids[PROC_AMOUNT];
    int x0, y0, x1, y1;
    x0 = area_x0;
    y0 = area_y0;
    x1 = (x0 + arr_len) % WIDTH;
    y1 = (y0 + (x0 + arr_len) / WIDTH);
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
        std::cout << "x0 y0 " << x0 << ' ' << y0 << std::endl;
        std::cout << "x1 y1 " <<  x1 << ' ' << y1 << std::endl;
        pids[i] = fork();
        if (pids[i] == 0) {
            NextGeneration(x0, y0, x1, y1, i);
            exit(EXIT_SUCCESS);
        }
    }
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
    for (int i = 0; i < PROC_AMOUNT; i++) {
        wait();
    }
    std::cout << "\nend of game\n";
    for (int i = 0; i <= PROC_AMOUNT; i++) {
        sem_unlink(&names[i]);
    }
    munmap(point, sizeof(int)*WIDTH*HEIGHT*2 + sizeof(int));
    shm_unlink(str);
}