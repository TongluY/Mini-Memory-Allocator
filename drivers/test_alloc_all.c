/*
 * =====================================================================================
 *
 *       Filename:  test_alloc_all.c
 *        
 *
 *    Description:  Allocates the entire heap. Verifies that future calls to Mem_Alloc return NULL
 *        Version:  1.0
 *        Created:  08/02/2022 07:36:42 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mem.h"
#include "header.h"
#include "util.h"

#define HEAP_SIZE 1600
#define INIT_VAL 0x66

int main() {
    void *heap = NULL;    
    size_t alloc_size = 0;
    alloc_size += MIN_BLOCK_SIZE; // Prologue 
    alloc_size += sizeof(Header); // First header
    alloc_size += HEAP_SIZE;      // Heap space
    alloc_size += sizeof(Header); // Epilogue 
    alloc_size += MIN_BLOCK_SIZE - 1; // Padding to align heap space 
    while (!heap) 
        heap = malloc(alloc_size); 

    assert(heap);
    // Init heap to random value
    memset(heap, INIT_VAL, alloc_size);

    char *chp = (char *) heap;
    chp += MIN_BLOCK_SIZE; // prologue
    chp += sizeof(Header); // first header
    size_t skipped_bytes = 0;
    while ((unsigned long) chp % 16) {
        skipped_bytes++;
        chp++;
    }
    // chp now points to the address the first user pointer would have
    assert(skipped_bytes < MIN_BLOCK_SIZE);
    chp -= sizeof(Header);
    
    // Begin actual test stuff
    Mem_Init(chp, HEAP_SIZE);
    void * p = Mem_Alloc(1592);
    assert(!Mem_Alloc(1));
    assert(!Mem_Alloc(30));
    Mem_Free(p);
    free(heap);
    return 0;
}
