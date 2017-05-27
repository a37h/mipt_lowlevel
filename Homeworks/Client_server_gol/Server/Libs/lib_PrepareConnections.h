void PrepareConnections(int x0, int y0, int x1, int y1,
                        int fd, int conn_amount, int iter) {
    int changed_area;
    write(fd,"Server. Welcome.\n",18);
    write(fd, &x0, sizeof(int));
    write(fd, &y0, sizeof(int));
    write(fd, &x1, sizeof(int));
    write(fd, &y1, sizeof(int));
    write(fd, &conn_amount, sizeof(int));
    write(fd, &iter, sizeof(int));
    while (true) {
        sem_wait(sems[iter]);   //начало
        write(fd, point, sizeof(int)*WIDTH*HEIGHT);
        write(fd, changed, sizeof(int));
        if (*changed == 2) {
            break;
        }
        read(fd, &changed_area, sizeof(int));
        if (changed_area == 1) {
            *changed = 1;
            read(fd, point+y0*WIDTH+x0, sizeof(int)*(y1*WIDTH+x1-y0*WIDTH-x0));
        }
        sem_post(sem);  //конец
    }
}