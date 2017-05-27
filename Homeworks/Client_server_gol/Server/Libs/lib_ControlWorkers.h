void ControlWorkers(const int conn_amount) {
    *changed = 1;
    int step = 0;
    while (true) {
        for (int i = 0; i < conn_amount; i++) {
            sem_post(sems[i]);  //начало
        }
        for (int i = 0; i < conn_amount; i++) {
            sem_wait(sem);  //конец
        }
        if (*changed == 0) {
            system("clear");
            std::cout << "▶ Game has been finished on " << step << " evolution.\n";
            PrintWorld();
            break;
        }
        if (step % amount_of_evolutions_to_show == 0) {
            system("clear");
            std::cout << "▶ Showing " << step << " evolution:\n";
            PrintWorld();
        }
        *changed = 0;
        step++;
    }
    *changed = 2;
    for (int i = 0; i < conn_amount; i++) {
        sem_post(sems[i]);
    }
}