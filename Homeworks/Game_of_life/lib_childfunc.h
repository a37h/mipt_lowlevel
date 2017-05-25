/*********************************************************************************************************/
/********************************************** CHILD PROCESS ********************************************/
void Fork_function__child(int* ptr1, int* ptr2, int* ptr3, int size, int i)
{
    int myleftborder, myrightborder, mytopborder, mybottomborder;
    if (i % 2 == 0) { myleftborder = 1; myrightborder = size/2+1; }              // map looks like this
    else { myleftborder = size/2+2; myrightborder = size; }                                    // 0 1
    if (i < 2) { mytopborder = 1; mybottomborder = size/2+1; }                                 // 2 3
    else { mytopborder = size/2+2; mybottomborder = size; }

    int a = 0;
    for ( a = 0; a < 5; a++)
    {
//          Qsem_sem_start(ptr3);
//          printf("Im a child (%d)\n", i);

        printf("%i  ",a);
        sleep(1);

//          Qsem_sem_end(ptr3, ptr1, ptr2, size, &ptr1, &ptr2);
    }
    printf("Proccess %i died\n", i);
    exit(0);
}