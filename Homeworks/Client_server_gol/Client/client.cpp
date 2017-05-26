#include <iostream>            // cin cout endl
#include <string.h>            // memcpy, strcmp
    // Tcp connections
#include <unistd.h>            // read, close, ftruncate, fork, write
#include <netdb.h>             // socket, gethostbyname, connect...
    // Shared memory related
#include <sys/mman.h>          // shm_open shm_unlink mmap ...
#include <fcntl.h>             // O_CREAT O_RDWR (for shm_open)
    // Semaphores
#include <semaphore.h>         // sem sem_wait sem_signal

    // Some constants (should be the same both for server and client)
#define STR_SIZE 1024
#include "../Constants.h"

void Main_f(int area_x0, int area_y0, int area_x1, int area_y1);

int* point = NULL;
int* is_change = NULL;
int* changed = NULL;
char str[STR_SIZE];

sem_t *sem;
sem_t *sems[PROC_AMOUNT];
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;

int socket_id;

#include "lib_NextGeneration.h"
#include "lib_Main_f.h"

int main(int argc,char **argv)
{
    //подключение к серверу
    struct protoent *protocol_record=NULL;
    struct hostent *host_record=NULL;
    struct sockaddr_in server_address;

    int sock_fd;

    if(argc<2)
    {
        printf("Write the client server_ip\n");
        return -1;
    }

    protocol_record=getprotobyname("tcp");
    socket_id=socket(PF_INET,SOCK_STREAM,protocol_record->p_proto);

    host_record=gethostbyname(argv[1]);
    if(host_record==NULL)
    {
        printf("Can't get address by name '%s'\n",argv[1]);
        return -1;
    }


    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(TCP_PORT_NUMBER);
    /*server_address.sin_addr.s_addr=inet_addr(argv[1]);*/
    memcpy(&server_address.sin_addr.s_addr, host_record->h_addr_list[0], host_record->h_length);

    printf("Connecting to server...\n");
    sock_fd=
            connect
                    (
                            socket_id,
                            (struct sockaddr *)&server_address,
                            sizeof(struct sockaddr_in)
                    );

    if(sock_fd)
    {
        printf("Can't connect to server\n");
        return -1;
    }

    read(socket_id,str,18);
    str[16]='\0';
    if(strcmp(str,"Server. Welcome."))
    {
        printf("Incorrect server invatation\n");
        printf("Server tells '%s'\n",str);
        return -1;
    }
    printf("Server connected!\n");
    //Подключение к серверу - конец

    int area_x0, area_y0, area_x1, area_y1;
    read(socket_id, &area_x0, sizeof(int));
    read(socket_id, &area_y0, sizeof(int));
    read(socket_id, &area_x1, sizeof(int));
    read(socket_id, &area_y1, sizeof(int));
    std::cout << area_x0 << ' ' << area_y0 << ' '
              <<  area_x1 << ' ' << area_y1 << std::endl;
    Main_f(area_x0, area_y0, area_x1, area_y1);

    close(socket_id);

    return 0;

}