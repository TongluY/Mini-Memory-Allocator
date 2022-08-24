/*
 * =====================================================================================
 *
 *       Filename:  free_alignment.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/04/2022 03:19:44 PM
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

#define HEAP_SIZE       64
#define INIT_VAL        0xaa

typedef struct two_headers {
    Header h1;
    Header h2;
} two_headers;

typedef struct six_headers {
    Header h1;
    Header h2;
    Header h3;
    Header h4;
    Header h5;
    Header h6;
} six_headers;

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
    while ((unsigned long) chp % 16) 
        chp++;
    // chp now points to the address the first user pointer would have

    chp -= sizeof(Header);

    // Begin actual test stuff
    Mem_Init(chp, HEAP_SIZE);
    
    two_headers *two = Mem_Alloc(sizeof(two_headers));
    six_headers *six = Mem_Alloc(sizeof(six_headers));
    
    two->h1 = 65;
    two->h2 = 65;

    six->h1 = 65;
    six->h2 = 65;
    six->h3 = 65;
    six->h4 = 65;
    six->h5 = 65;
    six->h6 = 65;
    
    assert(!Mem_Free((void*)two + 4));
    assert(!Mem_Free((void*)six + 4));
    assert(!Mem_Free((void*)two + 8));
    assert(!Mem_Free((void*)six + 16));

    assert(Mem_Free(two));
    assert(Mem_Free(six));
    assert((*(Header *)((void*)two - sizeof(Header))) == 64); 
    assert(!Mem_Free(two));
    assert(!Mem_Free(six));

    free(heap);
    return 0;
}
