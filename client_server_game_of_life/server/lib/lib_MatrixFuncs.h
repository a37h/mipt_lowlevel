void PrintWorld() {
    std::cout << "┏";
    for (int i = 0; i < WIDTH+1; i++) {
        std::cout << "━━";
    }
    std::cout << "┓" <<std::endl;
    for (int i = 0; i < HEIGHT; i++) {
        std::cout << std::setw(2) << "┃";
        for (int j = 0; j < WIDTH; j++) {
            if (point[i*WIDTH+j]) {
                std::cout << " ◉";
            }
            else {
                std::cout << "  ";
            }
        }
        std::cout << std::setw(2) << "  ┃\n";
    }
    std::cout << "┗";
    for (int i = 0; i < WIDTH+1; i++) {
        std::cout << "━━";
    }
    std::cout << "┛";
    std::cout << '\n';
    sleep(1);
}

void CreateWorld(std::vector<std::pair<int, int> > vec) {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            point[i*WIDTH+j] = 0;
        }
    }
    for (size_t i = 0; i < vec.size(); i++) {
        point[vec[i].first*WIDTH+vec[i].second] = 1;
    }
}

void RandomWorld() {
    srand(time(0));
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            point[i*WIDTH+j] = rand() % 2;
        }
    }
}