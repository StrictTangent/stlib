// randlist.c
// Paul Freeman 2020
//
// Example usage of st_arr
// Creates and modifies an integer array of random size with random values

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "stlib.h"


#define MAXNUM 1000

void print_arr_old(int * arr){
  int len = st__arr_len(arr);
  for (int i = 0; i < len; i++){
    printf("%d ", arr[i]);
  }
  printf("\n");
  
}

void print_arr(int * arr){
  int * element;
  st_arr_for(element, arr){
    printf("%d ", *element);
  }
  printf("\n");
}

int main () {

  time_t t;
  int arr_size, i, value;

  srand((unsigned) time(&t));
  arr_size = (rand() % 20) + 20;

  int * myarray = NULL;
  
  for (i =0; i < arr_size; i++){
    value = rand() % MAXNUM;
    st_arr_push(myarray, value);
  }

  printf("Random list of size %d:\n", st_arr_len(myarray));
  print_arr(myarray);
  
  printf("\nDelete first 5 elements:\n");
  st_arr_deleten(myarray, 0, 5);
  print_arr(myarray);
  
  printf("\nInsert 1987 at index 1\n");
  st_arr_insert(myarray, 1, 1987);
  print_arr(myarray);

  printf("\nInsert 3 new elements at index 7 and populate as 1, 2, 3:\n");
  st_arr_insertn(myarray, 7, 3);
  myarray[7] = 1;
  myarray[8] = 2;
  myarray[9] = 3;
  print_arr(myarray);

  printf("\nPop last value off the array:\n");
  value = st_arr_pop(myarray);
  printf("Popped value: %d\n", value);
  print_arr(myarray);
  
  st_arr_destroy(myarray);

}
