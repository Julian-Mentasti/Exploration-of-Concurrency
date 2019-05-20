#include "aco.h"
#include "aco_assert_override.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <inttypes.h>

#define MAX_N 10000
#define MAX_THREADS

// Shared variables; in threaded programs, all global vars are
// sared among all the co_routines.
int threads,         // number of threads
    n,               // sieve number count so far
    limit,           // limit of sieve
    prime[MAX_N+1],  // if prime[i] = 1 then its prime
    nextbase;        // next sieve multiplier to be used

// lock for the shared variable nextbase
pthread_mutex_t nextbaselock = PTHREAD_MUTEX_INITIALIZER;
// ID structs for the threads
pthread_t id[MAX_THREADS];

// Remove odd multiples of K
void remove_num(int k)
{
    int i;
    for (i = 3; i*k <= limit; i +=2) {
        prime[i*k] = 0;
    }
}

// Make the sieve, up to the arg
void co_ms()
{
    int *iretp = (int *)aco_get_arg();

    for (n=0; n<*iretp; n++) {
        if (n%2 == 0)
            if (n == 2) prime[n] = 1;
            else prime[n] = 0;
        else
            if (n == 1) prime[n] = 0;
            else prime[n] = 1;
    }

    aco_exit();
}

// Main co routine
void co_fp()
{
    int *iretp = (int *)aco_get_arg();
    remove_num(*iretp);
    aco_exit();
}

int main(int argc, char *argv[]) {

    //printf("How large do you want the sieve to be?");
    // limit = scanf("%d", &limit);
    limit = atoi(argv[1]);
    aco_thread_init(NULL);

    aco_t* main_co =  aco_create(NULL, NULL, 0, NULL, NULL);

    aco_share_stack_t* sstk = aco_share_stack_new(0);

    aco_t* co = aco_create(main_co, sstk, 0, co_ms, &limit); // create the number sieve
    aco_resume(co);


    /*
    for (n=0; n<limit; n++) {
        if (n%2 == 0)
            if (n == 2) prime[n] = 1;
            else prime[n] = 0;
        else
            if (n == 1) prime[n] = 0;
            else prime[n] = 1;
    }






    int ct = 0;
    while(ct <6) {
        aco_resume(co);
        printf("main_co:%p\n", main_co);
        ct++;
    }

    aco_resume(co);
    printf("main_co:%p\n", main_co);

    */
    // aco_destroy(co);
    // co = NULL;

    aco_share_stack_destroy(sstk);
    sstk = NULL;
    aco_destroy(main_co);
    main_co = NULL;

    printf("Limit is %d\n", limit);
    for (int i = 0; i < limit; ++i) {
        printf("I got a %d\n", prime[i]);
    }
    return 0;
}
