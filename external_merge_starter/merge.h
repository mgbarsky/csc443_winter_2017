#ifndef MERGE_H
#define MERGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_PATH_LENGTH 1024

//return values of all functions
#define SUCCESS 0
#define FAILURE 1
#define EMPTY 2

typedef struct record {
	int UID1;
	int UID2;
} Record;

typedef struct HeapElement {
	int UID1;
	int UID2;
	int run_id;
} HeapElement;

//Record-keeping struct, to pass around to all small functions
//has to be initialized before merge starts
typedef struct merge_manager {
	HeapElement *heap;  //keeps 1 from each buffer in top-down order - smallest on top (according to compare function)	
	FILE *inputFP; //stays closed, opens each time we need to reupload some amount of data from disk runs
	int *input_file_numbers;  //we need to know the run id to read from the corresponding run	
	FILE *outputFP; //flushes output from output buffer 
	Record *output_buffer; //buffer to store output elements until they are flushed to disk
	int current_output_buffer_position;  //where to add element in the output buffer
	int output_buffer_capacity; //how many elements max can it hold
	Record **input_buffers; //array of buffers to buffer part of runs
	int input_buffer_capacity; //how many elements max can each input buffer hold
	int *current_input_file_positions; //current position in each sorted run, can use -1 if the run is complete
	int *current_input_buffer_positions; //position in current input buffer
	int *total_input_buffer_elements;  //number of actual elements currently in input buffer - can be less than max capacity
	int current_heap_size;
	int heap_capacity;  //corresponds to the total number of runs (input buffers)
	char output_file_name [MAX_PATH_LENGTH]; //stores name of the file to which to write the final output
	char input_prefix [MAX_PATH_LENGTH] ; //stores the prefix of a path to each run - to concatenate with run id and to read the file
}MergeManager;

//1. main loop
int merge_runs (MergeManager * manager); 

//2. creates and fills initial buffers, initializes heap taking 1 top element from each buffer 
int init_merge (MergeManager * manager); 

//3. flushes output buffer to disk when full
int flush_output_buffer (MergeManager * manager); 

//4. returns top heap element, rearranges heap nodes
int get_top_heap_element (MergeManager * manager, HeapElement * result);

//5. inserts new element into heap, rearranges nodes to keep smallest on top
int insert_into_heap (MergeManager * manager, int run_id, Record *input);

//6. reads next element from an input buffer to transfer it to the heap. Uploads records from disk if all elements are processed
int get_next_input_element(MergeManager * manager, int file_number, Record *result); 

//7. refills input buffer from the corresponding run
int refill_buffer (MergeManager * manager, int file_number);

//8. Frees all dynamically allocated memory
void clean_up (MergeManager * merger);

//9. Application-specific comparison function
int compare_heap_elements (HeapElement *a, HeapElement *b);

#endif