// fib.c
// Paul Freeman 2020
//
// Example usage of st_map.
// Calculates the nth Fibonacci Number.
// Comparison of three versions and their runtimes.


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../stlib.h"

int fib(int n);
int fibDP(int n, st_map * memo);
int fibBU(int n);

int main() {
    int f;
    int n = 46;

    clock_t begin;
    clock_t end;
    double total_time;


    

    begin = clock();
    f = fib(n);
    end = clock();
    total_time = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("fib(%d) = %d\n", n, f);
    printf("TIME: %f\n\n", total_time);
    
    begin = clock();
    st_map * memo = st_map_createmap(1000, sizeof(int));
    f = fibDP(n, memo);
    st_map_destroy(memo);
    end = clock();
    total_time = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("fibDP(%d) = %d\n", n, f);
    printf("TIME: %f\n\n", total_time);

    begin = clock();
    f = fibBU(n);
    end = clock();
    total_time = (double) (end - begin) / CLOCKS_PER_SEC;
    printf("fibBU(%d) = %d\n", n, f);
    printf("TIME: %f\n\n", total_time);
}

// Recursive Fibonacci (SLOW!)
int fib(int n){
    int f;
    if (n <= 2) {
        f = 1;
    } else {
         f = fib(n-1) + fib(n-2);
    }
    return f;
}

// Recursive Fibonacci with st_map & dynamic programming (memoization)
int fibDP(int n, st_map * memo){

    char key[20];
    int f;
    sprintf(key, "%d", n);
    if (st_map_inmap(memo, key)){
        return *(int *) st_map_getptr_direct(memo, key);
    }
    if (n <= 2) {
        f = 1;
    } else {
        f = fibDP(n-1, memo) + fibDP(n-2, memo);
    }
    st_map_addval(memo, key, f);
    return f;
}

// Fibonacci bottom-up with st_map
int fibBU(int n){
    int f;
    char key[20];

    st_map * fib_map = st_map_createmap(1000, sizeof(int));
    
    for (int i = 1; i <= n; i++){
        
        if (i <= 2) {
            f = 1;
        } else {
            sprintf(key, "%d", i-1);
            f  = *(int*) st_map_getptr_direct(fib_map, key);
            sprintf(key, "%d", i-2);
            f += *(int*) st_map_getptr_direct(fib_map, key);
        }
        sprintf(key, "%d", i);
        st_map_addval(fib_map, key, f);
    }
    sprintf(key, "%d", n);
    st_map_getcopy(fib_map, key, &f);
    st_map_destroy(fib_map);
    return f;
}
