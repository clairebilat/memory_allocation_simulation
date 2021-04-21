// creator = Claire Bilat
// for the course Operating Systems, Spring 2020, BFH

#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "memory_simulation.h"


// every entry in page table initialized to INVALID
int init_page_table(int* pt, int size){
	for (int i = 0; i < size; ++i){
    pt[i] = INVALID;
  }
  return EXIT_SUCCESS;
}

// Initialise TLB with INVALID value for page, frame number and process ID
int init_TLB(TLB* tlb, int size){
  tlb->array = (tlb_item*) calloc(size, sizeof(tlb_item));
  for (int i = 0; i < size; ++i) {
    tlb->array[i].page = INVALID;
	  tlb->array[i].frame = INVALID;
    tlb->array[i].p_id = INVALID;
  }
  pthread_mutex_init (&tlb->mutex, NULL);
  return EXIT_SUCCESS;
}

// initialization of the head of the list of free frames with the adress of the first frame
// every frame is marked as free and added to the list of free frames
int init_free_frame_list(free_frame_list* ffl, int size) {
  free_frame* head = NULL;
  ffl->head = head;
  for (int i = size; i > 0; --i) {
    push_head((i - 1), &(ffl->head));
  } 
  pthread_mutex_init (&ffl->mutex, NULL);
  return EXIT_SUCCESS;
}

// add a frame to the list of free frames
void push_head(int frame, free_frame** head) {
  free_frame* f = (free_frame*) malloc(sizeof(free_frame));
  f->frame = frame;
  f->next = *head;
  *head = f;
}

// find the first free frame, deletes it from the list and return the frame number (INVALID if empty)
int pop_head(free_frame** head) {
  if (*head == NULL) {
    return INVALID;
  }
  free_frame* f = *head;
  int r = f->frame;
  *head = f->next;
  free(f);
  return r;
}

//check whether given virtual address is in range
bool check_valid_virtual_address(int virtual_address) {
  if (virtual_address > VIRTUAL_ADDRESS_SIZE || virtual_address < 0){
    fprintf(stderr, "The requested virtual address is invalid.\n");
    return false;
  }
  return true;
}

int get_physical_address(int virtual_address, int frame_number){
  int physical_address = virtual_address - virtual_address/PAGE_SIZE*PAGE_SIZE + frame_number*PAGE_SIZE;
  return physical_address; 
}

//convert a virtual address to its page number
int get_page_number(int virtual_address){
  int page_number = virtual_address/PAGE_SIZE;
  return page_number;
}

int tlb_get_hash(int page_number){
  int hash = page_number % MAX_TLB_LENGTH;
  return hash;
}

int ask_page_table(int page_number, int* page_table){
  printf("\t\tsearching page %d in page table\n",  page_number);
  if (page_table[page_number] == INVALID){
    printf("\t\tPage not in page table\n");
    return INVALID;
  }
  printf("\t\tPage in page table, frame_number = %d\n", page_table[page_number]);
  return page_table[page_number];
}

int ask_tlb(int page_number, tlb_item tlb[]){
  int hash = tlb_get_hash(page_number); 
  printf("\t\tTLB lookup for page %d (hash %d)\n",  page_number, hash);
  if (tlb[hash].page != page_number || tlb[hash].p_id != pthread_self()){
    printf("\t\tTLB miss signal\n");
    return INVALID;
  }
  printf("\t\tFound in TLB, frame_number = %d\n", tlb[hash].frame);
  return tlb[hash].frame;
}

void tlb_set(int page_number, int frame_number, tlb_item tlb[], int p_id){
  int hash = tlb_get_hash(page_number);
  tlb[hash].page = page_number;
  tlb[hash].frame = frame_number;
  tlb[hash].p_id = p_id;
}

int load_page(int page_number, free_frame** head, int page_table[], tlb_item tlb[]){
  int frame_number = pop_head(head);
  if (frame_number == INVALID){
	  printf("\t\tNo more free frames\n");
    return frame_number;
  }
  printf("\t\tFound a free frame number %d\n", frame_number);
  printf("\t\tframe %d marked as used\n", frame_number);
  printf("\t\tAdd frame %d at page table entry %d\n", frame_number, page_number);
  page_table[page_number] = frame_number;
  printf("\t\tAdd couple (%d, %d) in TLB\n", page_number, frame_number);
  tlb_set(page_number, frame_number, tlb, pthread_self());
  printf("\t\tPage number %d is now in frame %d\n", page_number, frame_number);
  return frame_number;
}

int access_memory(int virtual_address, TLB* tlb, free_frame_list* ffl, int* pt){
  int page_number = get_page_number(virtual_address);
  printf("\tLookup address : virtual_address = %d\n", virtual_address);
  printf("\tAsking TLB...\n");
  int potential_frame = ask_tlb(page_number, tlb->array);
  if (potential_frame == INVALID){
    printf("\tnot found in TLB, asking page table...\n");
    potential_frame = ask_page_table(page_number, pt);
    if(potential_frame == INVALID){
      printf("\tnot found in page table, loading page...\n");
	  pthread_mutex_lock(&tlb->mutex);
	  pthread_mutex_lock(&ffl->mutex);
      potential_frame = load_page(page_number, &(ffl->head), pt, tlb->array);
	  pthread_mutex_unlock(&tlb->mutex);
	  pthread_mutex_unlock(&ffl->mutex);
      if(potential_frame == INVALID){
        fprintf(stderr, "\tNo free frame found, interrupting program\n");
        return EXIT_FAILURE;
      }
    }
  }
  return potential_frame;
}

void* routine(void* mc){
  memory_context* mem = (memory_context*) mc;
  int*pt = (int*) calloc(NUMBER_PAGES, sizeof(int));
	init_page_table(pt, NUMBER_PAGES);
	int frame_number;
  int virtual_address;
  int p_id =  (int)(pthread_self());
  virtual_address = 1001;
	printf("Requesting virtual address %d, process %d\n", virtual_address, p_id);
	frame_number = access_memory(virtual_address, mem->tlb, mem->ffl, pt);
	printf("virtual address %i, process %d is now physical address %i\n", virtual_address, p_id, get_physical_address(virtual_address, frame_number));

  virtual_address = 1201;
	printf("Requesting virtual address %d, process %d\n", virtual_address, p_id);
	frame_number = access_memory(virtual_address, mem->tlb, mem->ffl, pt);
printf("virtual address %i, process %d is now physical address %i\n", virtual_address, p_id, get_physical_address(virtual_address, frame_number));
  virtual_address = 1051;
	printf("Requesting virtual address %d, process %d\n", virtual_address, p_id);
	frame_number = access_memory(virtual_address, mem->tlb, mem->ffl, pt);
printf("virtual address %i, process %d is now physical address %i\n", virtual_address, p_id, get_physical_address(virtual_address, frame_number));
  pthread_exit(NULL);
}
	

