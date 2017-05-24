#include "stdio.h"
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock;
pthread_mutex_t lock1;

char Get_amount_of_neighbours(char** World, int size, int x, int y)
{
    pthread_mutex_lock(&lock1);
    int count = World[x-1][y-1] + World[x-1][y] + World[x][y-1] + World[x-1][y+1];
    count += World[x+1][y-1] + World[x+1][y] + World[x][y+1] + World[x+1][y+1];
    pthread_mutex_unlock(&lock1);
    return count;
}

void show_matrix(int **p1, int m)
{
    printf("------------------------------------------");
    int i, j;
    for (i = 0; i < m; i++)
    {
        printf("\n");
        for (j = 0; j < m; j++)
        {
            printf("%i ",p1[i*m+j]);
        }
    }
    printf("\n------------------------------------------\n");
}

void fill_randomization(char** World, int size) {
    time_t t;
    srand((unsigned) time(&t));
    int world_space = size * size;

    int i;

    for (i = 0; i < world_space; i++) {
        World[i] = 0;
    }

    for (i = 0; i < world_space * 0.3; i++) {
        World[rand() % world_space] = 1;
    }

    for (int i = 0; i < size; i++)
    {
        World[i] = 0;
        World[size*size-size+i] = 0;
        World[size*i] = 0;
        World[size*i+size-1] = 0;
    }
}

//char Get_ending_flag(char** World, int size)
//{
//    int amount = 0;
//    for (int i = 0; i < size; ++i)
//        for (int j = 0; j < size; ++j)
//            if (World[i][j] == 1) {
//                amount++;
//            }
//    return amount == 0;
//}