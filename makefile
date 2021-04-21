all : memory_simulation.c memory_test.c
	gcc -pthread -g -Wall -pedantic -Wno-unused-variable -c memory_simulation.c
	gcc -pthread -g -Wall -pedantic -Wno-unused-variable -c memory_test.c
	gcc -pthread -o memory_simulation memory_test.o memory_simulation.o
