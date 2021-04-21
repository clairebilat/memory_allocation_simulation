// creator = Claire Bilat
// for the course Operating Systems, Spring 2020, BFH

#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "memory_simulation.h"

//In the test program, care is taken that free frame list and
//TLB are shared among all the threads, and that page table is specific 
//for each of the threads.

int main(int argc, char* argv[]){

  //TLB is an struct containing an array of TLB_item. 
  // That is each entry of the TLB
  //is a pair (page_number, frame_number);
  TLB tlb;
  
  // ffl is a struct containing a pointer to the first free
  //frame, and every free frame is a struct containing a pointer
  // to the next free frame
  free_frame_list ffl;

  init_free_frame_list(&ffl, NUMBER_FRAMES);
  init_TLB(&tlb, MAX_TLB_LENGTH);
   
  // mc contains both adresses of ffl and tlb to be able to use them
  // in a thread routine
  memory_context mc;
  mc.ffl = &ffl;
  mc.tlb = &tlb;

  // three threads are created for the sake of the test, and they all 
  // are executing the same routine (they request three virtual addresses,
  // which are 1001, 1201, 1051)
  pthread_t thread1;
  pthread_t thread2;
  pthread_t thread3;

  if (pthread_create(&thread1, NULL, routine, &mc)) {
    perror("pthread_create");
    return EXIT_FAILURE;
  }
  if (pthread_create(&thread2, NULL, routine, &mc)) {
    perror("pthread_create");
    return EXIT_FAILURE;
  }
  if (pthread_create(&thread3, NULL, routine, &mc)) {
    perror("pthread_create");
    return EXIT_FAILURE;
  }

  if (pthread_join(thread1, NULL)){
    perror("pthread_join");
    return EXIT_FAILURE;
  }
  if (pthread_join(thread2, NULL)){
    perror("pthread_join");
    return EXIT_FAILURE;
  }
  if (pthread_join(thread3, NULL)){
    perror("pthread_join");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
