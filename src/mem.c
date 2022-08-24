/******************************************************************************
 * @file: mem.c
 *
 * WISC NETID
 * CANVAS USERNAME
 * WISC ID NUMBER
 * OTHER COMMENTS FOR THE GRADER (OPTIONAL)
 *
 * @creator: YOUR PREFERED NAME (YOUR WISC EMAIL)
 * @modified: SUBMISSION DATE
 *****************************************************************************/

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "header.h"
#include "mem.h"

// Set this to 1 to enable dbgprintf  and dbgassert statements, make sure to 
// set it back to 0 before submitting!
#define DEBUG               0 
#define dbgprintf(...)      if (DEBUG) { printf(__VA_ARGS__); }
#define dbgassert(...)      if (DEBUG) { assert(__VA_ARGS__); }

/******************************************************************************
 * Helper functions
 *****************************************************************************/

// ADD ANY HELPER FUNCTIONS YOU MIGHT WRITE HERE 

/******************************************************************************
 * Global Variables
 *****************************************************************************/
void *heap_start;
size_t heap_size;

/******************************************************************************
 * Memory Allocator functions
 *****************************************************************************/

/*
 * This function initializes the heap space for future calls to Mem_Alloc and 
 * Mem_Free. You may safely assume that this function is only called once in 
 * a given program.
 *
 * @param Heap_Start : A pointer to the start of the heap space
 * @param Heap_Size : The size the heap
 * @return void
 */
void Mem_Init(void *Heap_Start, size_t Heap_Size) {

    // Register the start of the heap to the global variable
    heap_start = Heap_Start;

    // Register the size of the heap to the global variable
    heap_size = Heap_Size;

    /* BEGIN MODIFYING CODE HERE */

    // (Mandatory) Set up heap space

    /* Your first task is initialize the header for the heap. The heap may 
     * contain garbage values, your job is to mark the first header to denote
     * that heap can be treated as one large free block. To do so, you can use
     * the following steps:
     *
     * 1) Find the pointer that will be returned on the first call to malloc.
     *    Remember, this pointer must be aligned to a 16 byte boundary.
     * 2) Subtract sizeof(Header) bytes to get a pointer to the first header
     * 3) Set this header to the correct value. Ensure the LSB is 0 to mark
     *    the block as free.
     * 4) (Optional) if you are using footers, ensure you set the last 
     *    sizeof(Header) bytes to the same value as the header.
     */
    Header *fisrt_payload = (Header*)heap_start;
    int i = 0;
    while(((unsigned long)fisrt_payload)%16){
        fisrt_payload = (Header *)((unsigned long)fisrt_payload+1);
        i++;
    }
    Header *first_header = (Header*)((unsigned long)fisrt_payload - sizeof(Header));
    *first_header = heap_size-i+sizeof(Header);
    printf("%p\n",first_header);
    printf("%d\n",*first_header);
    heap_start = (void*)first_header;
    Header *last_footer = (Header*)((void*)first_header + *first_header- sizeof(Header));
    *last_footer = *first_header;
    heap_size = heap_size - i + sizeof(Header);

    // (Extra Credit) Set up prologue blocks
    Header *prologue_header = (Header*)((void*)heap_start-16);
    *prologue_header = 16+1;
    Header *prologue_footer = (Header*)((void*)heap_start-sizeof(Header));
    *prologue_footer = *prologue_header;
    // (Extra Credit) Set up epilogue block
    Header *epilogue = (Header*)((void*)heap_start+heap_size);
    *epilogue = 1;

    printf("%p\n",first_header);
    printf("%d\n",*first_header);
    /* END MODIFYING CODE HERE */
    return;
}

/*
 * This function  allows a user to request space on the heap. The type of param
 * payload is defined in mem.h and may not be changed. If param payload is ever
 * 0, this function should return NULL immediately. 
 *
 * @param payload : The number of bytes the user wants on the heap
 * @return A 16-byte aligned pointer to payload bytes on the heap on success,
 *         NULL on failure
 */
void* Mem_Alloc(Payload payload){
    /* BEGIN MODIFYING CODE HERE */

    // First, verify non-zero payload size
    if(payload==0)return NULL;
    // Determine what size block is needed to satisfy this payload request.
    // Remember, user pointers must be 16-byte aligned, and the minimum block
    // size is 16 bytes. 
    int header_footer_size = sizeof(Header) *2;
    int alloc_size = payload + header_footer_size;
    if((alloc_size)%16!=0) alloc_size += 16-(alloc_size)%16;
    // Search the heap space for a free block that can satisfy the request. 
    // Search should be done in address order.
        // if found, determine in the block can be split
            // if split is possible, split block and set headers
            // else allocate directly into block
        // else retrun NULL 
    for(Header *ptr = heap_start; *ptr != 1; ptr+= (*ptr & ~0x1)/sizeof(Header)){
        // if found
        if(*ptr%2==0 && *ptr>=alloc_size){
            // if slpit is possible
            if(*ptr-alloc_size>=16){
                Header *free_blk_header = (Header*)((void*)ptr+alloc_size);
                *free_blk_header = *ptr-alloc_size;
                Header *free_blk_footer = (Header*)((void*)free_blk_header + *free_blk_header - sizeof(Header));
                *free_blk_footer = *free_blk_header;
            }
            // else allocate directly into block
            Header *alloc_blk_header = (Header*)((void*)ptr);
            *alloc_blk_header = alloc_size+1;
            Header *alloc_blk_footer = (Header*)((void*)ptr+alloc_size-sizeof(Header));
            *alloc_blk_footer = *alloc_blk_header;
            return (Header*)((void*)alloc_blk_header+sizeof(Header));
        }
    }
    return NULL;
}


/*
 * This function allows a user to tell the memory allocator that they finished
 * using space that they had requested on the heap.
 *
 * @param ptr: A pointer
 * @return 0 on error, 1 on success 
 */
int Mem_Free(void *ptr) {
    // First, search through the allocated blocks to see if these ptr is indeed
    // one that was returned by a call to Mem_Alloc

    // If ptr is not in heap, return 0 immediately
    // If ptr is not after a Header, return 0 immediately
    // If the header before ptr is not allocated, return 0 immediately
    if(ptr == NULL) return 0;
    if((unsigned long)ptr%16!=0) return 0;
    Header * ptr_header = NULL;
    for(Header *pointer = heap_start; *pointer != 1; pointer+= (*pointer & ~0x1)/sizeof(Header)){
            if(pointer == ptr-4){
                     ptr_header = pointer;
            }
    }
    if(ptr_header==NULL) {
        // printf("a");
        return 0;}
    if(ptr_header<(Header*)heap_start||ptr_header>(Header*)(heap_start+heap_size)){
        // printf("b");
        return 0;}
    if(*ptr_header%2!=1){
        // printf("c");
        return 0;}

    // Free the block
    *ptr_header -= 1;
    Header * ptr_footer = (Header*)((void*)ptr_header+*ptr_header-sizeof(Header));
    *ptr_footer = *ptr_header;
    //printf("after freeing the block ptr_header:%p\n",ptr_header);
    //printf("after freeing the block *ptr_header:%d\n",*ptr_header);
    //printf("after freeing the block ptr_footer:%p\n",ptr_footer);
    //printf("after freeing the block *ptr_footer:%d\n",*ptr_footer);
    // Coalesce adjacent free blocks
    Header * prev_footer = (Header*)((void*)ptr_header-sizeof(Header));
    //printf("check if prev is free prev_footer:%p\n",prev_footer);
    //printf("check if prev is free *prev_footer:%d\n",*prev_footer);
    // if the prev is free
    if(*prev_footer%2==0){
	int ori_ptr_header = *ptr_header;
	ptr_header = (Header*)((void*)ptr_header-*prev_footer);
	*ptr_header = ori_ptr_header + *prev_footer;
        *ptr_footer = * ptr_header;
	//printf("if prev is free ptr_header:%p\n",ptr_header);
    	//printf("if prev is free *ptr_header:%d\n",*ptr_header);
    	//printf("if prev is free ptr_footer:%p\n",ptr_footer);
    	//printf("if prev is free *ptr_footer:%d\n",*ptr_footer);

    }
    // if the next if free
    Header * next_header = (Header*)((void*)ptr_header+*ptr_header);
    //printf("check if next is free next_header:%p\n",next_header);
    //printf("check if next is free *next_header:%d\n",*next_header);
 
    // if epilogue, ends
    if(*next_header!=1 && *next_header%2==0){
	*ptr_header += *next_header;
        ptr_footer = (Header*)((void*)ptr_footer+*next_header);
        *ptr_footer = *ptr_header;
	//printf("if next is free ptr_header:%p\n",ptr_header);
    	//printf("if next is free *ptr_header:%d\n",*ptr_header);
    	//printf("if next is free ptr_footer:%p\n",ptr_footer);
    	//printf("if next is free *ptr_footer:%d\n",*ptr_footer);

    }
    // ptr = (Header *)((void*)ptr_header + sizeof(Header));
    return 1;
}

