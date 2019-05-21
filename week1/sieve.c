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
aco_t* main_co;
// lock for the shared variable nextbase
pthread_mutex_t nextbaselock = PTHREAD_MUTEX_INITIALIZER;
int number;
// ID structs for the threads
pthread_t id[MAX_THREADS];
aco_share_stack_t* sstk2;
// Remove odd multiples of K
void remove_num(int k)
{
    // assert(k != 0);
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
    aco_t* this_co = aco_get_co();
    int count = *iretp;
    printf("Found a prime -- %d\n", count);
    remove_num(count);
    // search for the first number that is one
    for (int i=count+1; i<limit; ++i) {
        if (prime[i]) {
            printf("Found a prime -- %d\n", i);
            count = i;
            *iretp = count;
            aco_t* co_help = aco_create(main_co, sstk2, 0, co_fp, &count);

        }
    }
    aco_exit();
    assert(0);
}




int main(int argc, char *argv[]) {

    //printf("How large do you want the sieve to be?");
    // limit = scanf("%d", &limit);
    limit = atoi(argv[1]);
    aco_thread_init(NULL);
    int number = 3;

    main_co =  aco_create(NULL, NULL, 0, NULL, NULL);

    aco_share_stack_t* sstk = aco_share_stack_new(0);
    sstk2 = aco_share_stack_new(0);

    aco_t* co = aco_create(main_co, sstk, 0, co_ms , &limit); // create the number sieve
    aco_resume(co);

    aco_t* co1 = aco_create(main_co, sstk2, 0, co_fp, &number);
    aco_resume(co1);


    // printf("main_co:%p\n", main_co);

    aco_destroy(co);
    co = NULL;

    aco_share_stack_destroy(sstk);
    sstk = NULL;
    aco_destroy(main_co);
    main_co = NULL;

    // printf("Limit is %d\n", limit);
    // for (int d = 0; d < limit; ++d) {
    //     printf("I got a %d for %d\n", prime[d], d);
    // }
    return 0;
}
