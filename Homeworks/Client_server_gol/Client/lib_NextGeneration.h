void NextGeneration(int x0, int y0, int x1, int y1, int iter) {
    int count = 0;
    int start, end;
    while (true) {
        sem_wait(sems[iter]);
        if (*changed == 2) break;
        for (int i = y0; i <= y1; i++) {
            if (i == HEIGHT) break;
            if (i == y0) start = x0;
            else start = 0;
            if (i == y1) end = x1;
            else end = WIDTH;
            for (int j = start; j < end; j++) {
                is_change[i*WIDTH+j] = 0;
                count = 0;
                for (int i1 = i - 1; i1 <= i + 1; i1++) {
                    for (int j1 = j - 1; j1 <= j + 1; j1++) {
                        if (i1 >= 0 && i1 < HEIGHT &&
                            j1 >= 0 && j1 < WIDTH &&
                            (i1 != i || j1 != j) &&
                            point[i1*WIDTH+j1]) {
                            count++;
                        }
                    }
                }
                if ((point[i*WIDTH+j] && (count < 2 || count > 3)) ||
                    (!point[i*WIDTH+j] && count == 3)) {
                    is_change[i*WIDTH+j] = 1;
                    *changed = 1;
                }
            }
        }
        sem_post(sem);
        sem_wait(sems[iter]);
        for (int i = y0; i <= y1; i++) {
            if (i == y0) start = x0;
            else start = 0;
            if (i == y1) end = x1;
            else end = WIDTH;
            for (int j = start; j < end; j++) {
                point[i*WIDTH+j] = (point[i*WIDTH+j] + is_change[i*WIDTH+j]) % 2;
            }
        }
        sem_post(sem);
    }
}