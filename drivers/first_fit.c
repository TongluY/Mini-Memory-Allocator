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
    while ((unsigned long) chp % 16)
        chp++;
    // chp now points to the address the first user pointer would have

    chp -= sizeof(Header);

    // Begin actual test stuff
    Mem_Init(chp, HEAP_SIZE);
    void *p = Mem_Alloc(120);
    void *q = Mem_Alloc(8);
    void *r = Mem_Alloc(24);
    void *s = Mem_Alloc(8);
    void *t = Mem_Alloc(40);
    void *u = Mem_Alloc(8);
    void *v = Mem_Alloc(8);
    void *w = Mem_Alloc(8);

    Mem_Free(p);
    Mem_Free(r);
    Mem_Free(t);
    Mem_Free(v);

    void *a = Mem_Alloc(8);
    void *b = Mem_Alloc(24);
    void *c = Mem_Alloc(40);

    assert(!((*(Header *)(v - sizeof(Header))) & 1));
    assert(!((*(Header *)(t - sizeof(Header))) & 1));
    assert(!((*(Header *)(r - sizeof(Header))) & 1));

 
    free(heap);
    return 0;
}
