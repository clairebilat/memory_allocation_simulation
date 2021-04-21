// creator = Claire Bilat
// for the course Operating Systems, Spring 2020, BFH

#ifndef MEMORY_SIMULATION_H
#define MEMORY_SIMULATION_H


#define PAGE_SIZE 100 //size (in bytes) of a page
#define FRAME_SIZE PAGE_SIZE // size (in bytes) of a frame
#define VIRTUAL_ADDRESS_SIZE 5000 // how many virtual memory a process may use
#define PHYSICAL_ADDRESS_SIZE 2000 //how many physical memory is installed in the system
#define NUMBER_FRAMES ((PHYSICAL_ADDRESS_SIZE / FRAME_SIZE))
#define NUMBER_PAGES ((VIRTUAL_ADDRESS_SIZE / PAGE_SIZE))
#define MAX_TLB_LENGTH 4 // entries in TLB

#define INVALID (-1)


//structure defining a free frame, containing the frame number 
//and the address of the next free frame
typedef struct free_frame {
  int frame;
  struct free_frame *next;
} free_frame;

//structure containing the head of the free frame list
//(pointer on the first free frame) and a mutex
typedef struct free_frame_list {
	free_frame* head;
	pthread_mutex_t mutex;
} free_frame_list;

typedef struct tlb_item{
  int page; //used as key in the hash table
  int frame; // used as value in the hash table
  int p_id; // used to identify the process corresponding to the entry
} tlb_item;

// structure containing an array of tlb_items and a mutex
typedef struct TLB{
	tlb_item* array;
	pthread_mutex_t mutex;
} TLB;

// structure containing both a pointer to TLB and to a free_frame_list
typedef struct memory_context{
	TLB* tlb;
	free_frame_list* ffl;
	//int virtual_address;
} memory_context;

int init_page_table(int* pt, int size);
int init_TLB(TLB* tlb, int size);
int init_free_frame_list(free_frame_list* ffl, int size);
void push_head(int frame, free_frame** head);
int pop_head(free_frame** head);

bool check_valid_virtual_address(int virtual_address);
int get_physical_address(int virtual_address, int frame_number);

int get_page_number(int virtual_address);
int tlb_get_hash(int page_number);
int ask_page_table(int page_number, int page_table[]);
int ask_tlb(int page_number, tlb_item tlb[]);
void tlb_set(int page_number, int frame_number, tlb_item tlb[], int p_id);


int load_page(int page_number, free_frame** head, int page_table[], tlb_item tlb[]);
int access_memory(int virtual_address, TLB* tlb, free_frame_list* ffl, int* pt);

void* routine(void* mc);

#endif