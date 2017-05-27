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
    // Client and server same constants
#include "../Constants.h"
    // Client constants and global variables
#define STR_SIZE 1024
int* point = NULL;
int* is_change = NULL;
int* changed = NULL;
char str[STR_SIZE];
sem_t *sem;
sem_t *sems[PROC_AMOUNT];
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
int socket_id;
    // Some custom includes (uses global variables described earlier)
#include "Libs/lib_SingleWorkerh.h"
#include "Libs/lib_CreatingForks.h"

int main(int argc,char **argv) {
    if (system("CLS")) system("clear");
    std::cout << "┏━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┓" << std::endl;

/*****************************************************************************************************/
/************************************* Connection preparations ***************************************/
    // Establishing a connection to the server
    struct protoent *protocol_record=NULL;
    struct hostent *host_record=NULL;
    struct sockaddr_in server_address;
    int sock_fd;
    // Server ip is taken as first argument after ./client (otherwise, by default, it's "localhost")
    char *connection_address;
    if(argc<2) { connection_address =  (char*)"localhost"; }
    else { connection_address = argv[1]; }
    // Preparing tcp connection
    protocol_record=getprotobyname("tcp");
    socket_id=socket(PF_INET,SOCK_STREAM,protocol_record->p_proto);
    host_record=gethostbyname(connection_address);

/*****************************************************************************************************/
/********************************** Establishing a connection ****************************************/
    if(host_record==NULL) { printf("┃ Can't get address by name '%s'           ┃\n",argv[1]); return -1; }
    server_address.sin_family=AF_INET;
    server_address.sin_port=htons(TCP_PORT_NUMBER);
    /*server_address.sin_addr.s_addr=inet_addr(argv[1]);*/
    memcpy(&server_address.sin_addr.s_addr, host_record->h_addr_list[0], host_record->h_length);
    printf("┃ Trying to connect                           ┃\n");
    sock_fd=connect(socket_id, (struct sockaddr *)&server_address, sizeof(struct sockaddr_in));
    if(sock_fd) { std::cout << "┃ Can not connect to server                   ┃" << std::endl
    << "┃ Some problem occured. Exit code: -1         ┃" << std::endl
    << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛" << std::endl; return -1;}

/*****************************************************************************************************/
/***************************** Connection success, checking for problems *****************************/
    read(socket_id,str,18);
    str[16]='\0';
    if(strcmp(str,"Server. Welcome.")) {
        std::cout << "┃ Can not connect to server                   ┃" << std::endl
        << "┃ Some problem occured. Exit code: -2         ┃" << std::endl
        << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛" << std::endl;
        std::cout << "Error info: '" << str << "'" << std::endl;
        return -2; }
    std::cout << "┃ Connected to the server!                    ┃" << std::endl
    << "┗━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━┛" << std::endl;

/*****************************************************************************************************/
/***************************************** Reading some inputs ***************************************/
    int area_x0, area_y0, area_x1, area_y1;
    read(socket_id, &area_x0, sizeof(int));
    read(socket_id, &area_y0, sizeof(int));
    read(socket_id, &area_x1, sizeof(int));
    read(socket_id, &area_y1, sizeof(int));
    std::cout << "┏━━━ WORKING AREA ━━━▶" << std::endl;
    std::cout << "┃ x0: " << area_x0 << "; y0: " << area_y0 << "; " << std::endl;
    std::cout << "┃ x1: " << area_x1 << "; y1: " << area_y1 << "; " << std::endl;
    std::cout << "┣━━━━━━━━━━━━━━━━━━━━▶" << std::endl;

/*****************************************************************************************************/
/******************************************* Proceed to work *****************************************/
    CreatingForks(area_x0, area_y0, area_x1, area_y1);
    close(socket_id);
    return 0;
}