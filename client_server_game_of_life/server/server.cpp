#include <iostream>            // cin cout endl
#include<iomanip>              // setw
#include<vector>
// Tcp connections
#include <unistd.h>            // read, close, ftruncate, fork, write
#include <netdb.h>             // socket, gethostbyname, connect...
// Shared memory related
#include <sys/mman.h>          // shm_open shm_unlink mmap ...
#include <fcntl.h>             // O_CREAT O_RDWR (for shm_open)
// Semaphores
#include <semaphore.h>         // sem sem_wait sem_signal
// Client and server same constants
#include "../Constants.h"
// Client constants and global variables
#include "libs/GlobalVariables.h"
// Some custom includes (uses global variables described earlier)
#include "libs/lib_MatrixFuncs.h"
#include "libs/lib_PrepareConnections.h"
#include "libs/lib_ControlWorkers.h"

int main(void)
{
    if (system("CLS")) system("clear");
    std::cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
    std::cout << "┃ Connections amount (excluding server): ";
    int conn_amount;
    std::cin >> conn_amount;
    if (conn_amount <= 0) {
        std::cout << "Minimal amount is 1. Error code: -1" << std::endl;
        return -1;
    }
    if (conn_amount*PROC_AMOUNT > WIDTH*HEIGHT) {
        std::cout << "Maximal amount is " << WIDTH*HEIGHT << " (size of the playing field). Error code: -2" << std::endl;
        return -2;
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
        std::cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
        std::cout << "┃ Saved field is present, continue? yes(y) or no(n): ";
        std::cin >> x;
        if (x == 'y') {
            data = open("data", O_RDWR, 0777);
            std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
        } else if (x == 'n') {
            remove("data");
            exist = false;
            data = open("data", O_CREAT|O_RDWR, 0777);
        } else {
            std::cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
            std::cout << "┃ Something went wrong. Exit code: -3"<< std::endl;
            std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
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
        std::cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
        std::cout << "┃ For a random field type 'r', custom 'c': ";
        std::cin >> x;
        if (x == 'c') {
            std::cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
            std::cout << "┃ Enter points, type negative number to stop." << std::endl;
            std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl << std::endl;
            while (true) {
                std::cin >> a;
                if (a < 0) {
                    break;
                }
                std::cin >> b;
                if (b < 0) {
                    break;
                }
                vec.push_back(std::pair<int, int>(a, b));
            }
            CreateWorld(vec);
        }
        else if (x == 'r') {
            RandomWorld();
            std::cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
            std::cout << "┃ Game is ready, enter 's' anything to begin" << std::endl;
            std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶"  << std::endl;
            while (true)
            {
                char temp;
                std::cin >> temp;
                if (temp == 's') break;
            }
        } else {
            std::cout << "┣━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
            std::cout << "┃ Something went wrong. Exit code: -3";
            std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶" << std::endl;
            return -3;
        }
        system("clear");
        std::cout << "\n";
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
    std::cout << "┏━━━━━━━━━━━━━━━━━━━━━━━▶"  << std::endl;
    std::cout << "┃ Waiting for connections..." << std::endl;
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
                PrepareConnections(x0, y0, x1, y1, connections[i], conn_amount, i);
                exit(EXIT_SUCCESS);
            }
            std::cout << "┣━━━▶ Worker #" << i << " successfully connected" <<std::endl;
        }
        else {
            break;
        }
    }
    //сервер - конец
    std::cout << "┃ Game is ready, enter 's' anything to begin" << std::endl;
    std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶"  << std::endl;
    while (true)
    {
        char temp;
        std::cin >> temp;
        if (temp == 's') break;
    }
    ControlWorkers(conn_amount);
    for (int i = 0; i < conn_amount; i++) {
        wait();
    }
    std::cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶"  << std::endl;
    std::cout << "┃ Game has been successfully ended." << std::endl;
    std::cout << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━▶"  << std::endl;
    for (int i = 0; i <= conn_amount; i++) {
        sem_unlink(&names[i]);
    }
    munmap(point, sizeof(int)*WIDTH*HEIGHT + sizeof(int));
    remove("data");
    close(socket_id);
    return 0;

}