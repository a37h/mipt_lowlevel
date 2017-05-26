#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <unistd.h>

#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

#include<iostream>
#include<iomanip>
#include<vector>
#include<semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <cstring>


#include "../Constants.h"
#define PROC_AMOUNT 4

int* point = NULL;
int* changed = NULL;

sem_t *sem;
sem_t *sems[PROC_AMOUNT];

void CreateWorld(std::vector<std::pair<int, int> > vec);
void PrintWorld();
void RandomWorld();
void Main_f(const int conn_amount);
void NextGeneration(int x0, int y0, int x1, int y1,
                    int fd, int conn_amount, int iter);

//pthread_mutex_t mutex;

int num_connected = 0;

int main(void)
{
    std::cout << "Connections amount (excluding server): ";
    int conn_amount;
    std::cin >> conn_amount;
    if (conn_amount <= 0) {
        std::cout << "Error! There must be at least one connection\n";
        return -1;
    }
    if (conn_amount*PROC_AMOUNT > WIDTH*HEIGHT) {
        std::cout << "Error! Too many connections & processes";
        return -1;
    }
    std::vector<std::pair<int, int> > vec;
    int a, b = 0;
    char x;
    bool exist = true;
    int data;
    if (access("data", 0) == -1) {
        exist = false;
        data = open("data", O_CREAT|O_RDWR, 0777);
    } else {
        std::cout << "Would you continue the last game? [y/n]\n";
        std::cin >> x;
        if (x == 'y') {
            data = open("data", O_RDWR, 0777);
        } else if (x == 'n') {
            remove("data");
            exist = false;
            data = open("data", O_CREAT|O_RDWR, 0777);
        } else {
            std::cout << "Wrong letter!";
            return -1;
        }
    }
    if (!exist) {
        ftruncate(data, sizeof(int)*WIDTH*HEIGHT + sizeof(int));
    }
    point = (int*)mmap(0, sizeof(int)*WIDTH*HEIGHT, PROT_WRITE|PROT_READ, MAP_SHARED, data, 0);

    changed = (int*)mmap(0, sizeof(int), PROT_WRITE|PROT_READ, MAP_SHARED, data, 0);
    changed += sizeof(int)*WIDTH*HEIGHT;

    if (!exist) {
        std::cout << "Print [r] to randomize start points. To write it print [w]\n";
        std::cin >> x;
        if (x == 'w') {
            std::cout << "Enter points: \nTo stop enter '-1'\n";
            while (true) {
                std::cin >> a;
                if (a == -1) {
                    break;
                }
                std::cin >> b;
                vec.push_back(std::pair<int, int>(a, b));
            }
            CreateWorld(vec);
        }
        else if (x == 'r') {
            RandomWorld();
        } else {
            std::cout << "Wrong letter!";
            return -1;
        }
        system("clear");
        std::cout << "Start of the world:\n";
        PrintWorld();
    }
    char names[conn_amount + 1];
    for (int i = 0; i <= conn_amount; i++) {
        names[i] = i + 97;
    }
    for (int i = 0; i <= conn_amount; i++) {
        sem_unlink(&names[i]);
    }
    sem = sem_open(&names[0], O_CREAT, 0777, 0);
    int arr_len = HEIGHT*WIDTH / conn_amount;   //длина массива для каждого узла
    int pids[conn_amount];
    int x0, y0, x1, y1;
    x0 = y0 = 0;
    x1 = arr_len % WIDTH;
    y1 = arr_len / WIDTH;

    //сервер
    int connections[conn_amount];
    struct sockaddr_in clients[conn_amount];
    int socket_id;
    struct protoent *protocol_record=NULL;
    struct sockaddr_in server_address;

    socklen_t len;

    //pthread_mutex_init(&mutex,NULL);

    protocol_record=getprotobyname("tcp");
    socket_id=socket(PF_INET,SOCK_STREAM,protocol_record->p_proto);
    int enable = 1;
    if (setsockopt(socket_id, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        close(socket_id);
        perror("SO_REUSEADDR error");
        return -1;
    }
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(PORT);
    server_address.sin_addr.s_addr=INADDR_ANY;

    if(bind(socket_id,(struct sockaddr *)&server_address,sizeof(struct sockaddr_in)))
    {
        close(socket_id);
        perror("Bind error");
        return -1;
    }
    std::cout << "Waiting for connections...\n";
    listen(socket_id,conn_amount);

    for(int i=0;i<conn_amount;i++) {
        connections[i] = -1;;
    }
    int i = 0;
    while(true) {
        int tmp_fd;

        if(num_connected<conn_amount)
        {
            for(i=0;i<conn_amount;i++)
            {
                if(connections[i] == -1)
                {
                    break;
                }
            }
            tmp_fd = accept(socket_id,(struct sockaddr *)&clients[i],&len);
            if (tmp_fd == -1) {
                perror("Acception error");
                return -1;
            }
            connections[i] = tmp_fd;
            num_connected++;
            sems[i] = sem_open(&names[i+1], O_CREAT, 0777, 0);
            if (i != 0 && i == conn_amount - 1) {
                x0 = x1;
                y0 = y1;
                x1 = 0;
                y1 = HEIGHT;
            } else if (i != 0) {
                x0 = x1;
                y0 = y1;
                x1 = (x0 + arr_len) % WIDTH;
                y1 = (y0 + (x0 + arr_len) / WIDTH);
            }
            pids[i] = fork();
            if (pids[i] == 0) {
                NextGeneration(x0, y0, x1, y1, connections[i], conn_amount, i);
                exit(EXIT_SUCCESS);
            }
            std::cout << "Connection № " << i << " is established\n";

        }
        else {
            break;
        }
    }
    //сервер - конец
    std::cout << "Pres any button\n";
    getchar();
    Main_f(conn_amount);
    for (int i = 0; i < conn_amount; i++) {
        wait();
    }
    std::cout << "\nend of game\n";
    for (int i = 0; i <= conn_amount; i++) {
        sem_unlink(&names[i]);
    }
    munmap(point, sizeof(int)*WIDTH*HEIGHT + sizeof(int));
    remove("data");
    close(socket_id);
    return 0;

}

void PrintWorld() {
    for (int i = 0; i < WIDTH; i++) {
        std::cout << std::setw(2) << "_";
    }
    std::cout << '\n';
    for (int i = 0; i < HEIGHT; i++) {
        std::cout << std::setw(2) << "|";
        for (int j = 0; j < WIDTH; j++) {
            if (point[i*WIDTH+j]) {
                std::cout << std::setw(2) << "* ";
            }
            else {
                std::cout << std::setw(2) << " ";
            }
        }
        std::cout << std::setw(2) << "|\n";
    }
    for (int i = 0; i < WIDTH; i++) {
        std::cout << std::setw(2) << "_";
    }
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
            //if (i % 3 != 2 && j % 3 != 2) point[i*WIDTH+j] = 1;
        }
    }
    //point[HEIGHT / 2*WIDTH+WIDTH / 2 - 1] = 1;
}

void Main_f(const int conn_amount) {
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
            std::cout << "The last " << step << " step:\n";
            PrintWorld();
            break;
        }
        if (step % 10 == 1) {
            system("clear");
            std::cout << "The " << step << " step:\n";
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

void NextGeneration(int x0, int y0, int x1, int y1,
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