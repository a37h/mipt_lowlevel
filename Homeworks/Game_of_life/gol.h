#include "stdio.h"
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock;


char Get_ending_flag(char** World, int size)
{
    int amount = 0;
    for (int i = 0; i < size; ++i)
        for (int j = 0; j < size; ++j)
            if (World[i][j] == 1) {
                amount++;
            }
    return amount == 0;
}

char Get_amount_of_neighbours(char** World, int size, int x, int y)
{
    if (x < 0 || x >= size || y < 0 || y >= size) return 9;
    char count = 0;
    if ((x!=0) && (y!=0) && (World[(x-1)*size+y-1] == 1)) count++;
    if ((x!=0) && (World[(x-1)*size+y] == 1)) count++;
    if ((x!=0) && (y!=size-1) && (World[(x-1)*size+y+1] == 1)) count++;
    if ((y!=size-1) && (World[(x)*size+y+1] == 1)) count++;
    if ((x!=size-1) && (y!=size-1) && (World[(x+1)*size+y+1] == 1)) count++;
    if ((x!=size-1) && (World[(x+1)*size+y] == 1)) count++;
    if ((x!=size-1) && (y!=0) && (World[(x+1)*size+y-1] == 1)) count++;
    if ((y!=0) && (World[(x)*size+y-1] == 1)) count++;
    return count;
}

// LB LeftBorder --- RB RightBorder --- TB TopBorder --- BB BottomBorder
// (0,y)             (size-1,y)         (x,size-1)       (x,0)
// We're working with frame with (LB,BB) left-bottom corner and (RB,TB) right-top corner
void Work_over_an_area(char **World, char **New_World, int size,
                       int LB, int  RB, int TB, int BB)
{
    pthread_mutex_lock(&lock);
    for (int i = LB; i <= RB; ++i) {
        for (int j = BB; j <= TB; ++j) {
            int Amount_of_neighbours = Get_amount_of_neighbours(World,size,i,j);
            if (Amount_of_neighbours < 2 || Amount_of_neighbours > 3) New_World[i][j]=0;
            else if (Amount_of_neighbours == 2) New_World[i][j]=World[i][j];
            else New_World[i][j]=1;
        }
    }
    pthread_mutex_unlock(&lock);
}









void show_matrix(int **p1, int m)
{
    printf("\n------------------------------------------");
    int i, j;
    for (i = 0; i < m; i++)
    {
        printf("\n");
        for (j = 0; j < m; j++)
        {
            printf("%i ",p1[i*m+j]);
        }
    }
    printf("\n------------------------------------------");
}
void fill_randomization(char** World, int size)
{
    time_t t;
    srand((unsigned) time(&t));
    int world_space = size*size;

    int i;

    for (i = 0; i < world_space; i++)
    {
        World[i] = 0;
    }

    for( i = 0 ; i < world_space*0.2 ; i++ )
    {
        World[rand() % world_space] = 1;
    }
}

void Swap(int ***p1, int ***p2) {
    printf("me");
    int **tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}