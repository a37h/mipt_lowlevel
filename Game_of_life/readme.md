Game_of_life is unfinished.

Right now it has good forking into several processes, has 2 pthreads in parent process, one to check if every child process has
ended it's work so we could move on, one to sync them using my custom semaphore. Actually, custom semaphore isn't done yet,
so code isn't working properly.

Reminder for myself:

1. Rewrite custom semaphore in next way:
1) Each process can use Qsem_start(...), after that counter of started processes rises and that process sleeps
but also that process triggers mutex1 to unlock
2) Each process can use Qsem_finish(...), after that counter of ended processes rises and that process sleeps
but also that process triggers mutex2 to unlock
3) Parent process should always check if there's started processes count is equal to total processes amount
but before check he has to lock mutex1
4) Parent process should always check if there's ended processes count is equal to processes amount
but before check he has to lock mutex2

By using those mutex1 and mutex2 mutexes we can reduce amount of accesses to the started and ended proccesses counters
Each proccess when enters that func opens a mutex so parent process can now check if he should refresh custom semaphore.

after that just check if every process syncronises well
if so, add features of the game of life
all of needed func are already present in lib_matrixwork.h

good luck,me
