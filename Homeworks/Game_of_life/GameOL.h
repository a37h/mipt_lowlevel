char Get_amount_of_neighbours(char** World, int size, int x, int y)
{
    if (x < 0 || x >= size || y < 0 || y >= size) return 9;
    char count = 0;
    if ((x!=0) && (y!=0) && (World[x-1][y-1] == 1)) count++;
    if ((x!=0) && (World[x-1][y] == 1)) count++;
    if ((x!=0) && (y!=size-1) && (World[x-1][y+1] == 1)) count++;
    if ((y!=size-1) && (World[x][y+1] == 1)) count++;
    if ((x!=size-1) && (y!=size-1) && (World[x+1][y+1] == 1)) count++;
    if ((x!=size-1) && (World[x+1][y] == 1)) count++;
    if ((x!=size-1) && (y!=0) && (World[x+1][y-1] == 1)) count++;
    if ((y!=0) && (World[x][y-1] == 1)) count++;
    return count;
}

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

void Swap(char ***p1, char ***p2) {
    char **tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}