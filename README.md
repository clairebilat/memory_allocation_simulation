# memory allocation simulation

The `memory_simulation` program simulates a thread safe memory allocation system based on a pure demand paging mechanism.


## To compile it:

Use the _make all_ command via a linux terminal in the folder containing all files.


## To run it:

Run the _./memory_simulation_ program. 


## To modify the program parameters:

The variables listed below are set in the memory_simulation.h file and can be changed there as needed by the user. 
- `PAGE_SIZE` (size in bytes of a page): 100
- `FRAME_SIZE` (size in bytes of a frame): 100
- `VIRTUAL_ADDRESS_SIZE` (size of the virtual memory that the process can use): 5000
- `PHYSICAL_ADDRESS_SIZE` (size of the physical memory installed on the system): 2000
- `MAX_TLB_LENGTH` (maximum number of entries in the TLB): 4

The virtual addresses tested by the different threads (which simulate the different processes) are explained in the "routine" function in the _memory_simulation.c_ file and can be modified there according to the needs of the demonstration. 

