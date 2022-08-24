/*
 * =====================================================================================
 *
 *       Filename:  alloc_free_220.c
 *
 *    Description:  Allocates memory for 100 char arrays of length 8 and 100 char arrays
 *                  of length 24. Fills each with garbage data. Then checks that the
 *                  allocated arrays and original arrays match. Finally frees all memory
 *                  that was allocated
 *
 *        Version:  1.0
 *        Created:  08/02/2022 11:51:58 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Eric Harrigan 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "mem.h"
#include "header.h"

#define HEAP_SIZE       160000
#define INIT_VAL        5
#define NUM_INSERTS     1500

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
    Mem_Init(chp, HEAP_SIZE);
    //above taken from test file provided by Bujji Setty Summer 2022
    //
    //
       

    char checker16[100][8];
    char checker32[100][24];
    char **buffer16 = (char **)Mem_Alloc(100 * sizeof(char*));
    assert(buffer16);
    char **buffer32 = (char **)Mem_Alloc(100 * sizeof(char*));
    if(!buffer32) {
        Mem_Free(buffer16);
        printf("failed to allocate buffer32");
        exit(1);
    }
    for(size_t i = 0; i < 100; i++) {
        assert((*(buffer16 + i) = (char *)Mem_Alloc(8 * sizeof(char))));
        assert((*(buffer32 + i) = (char *)Mem_Alloc(24 * sizeof(char))));
        if(!*(buffer16 + i)) {
            for(size_t a = 0; a < i; a++) {
                Mem_Free(*(buffer16 + a));
            }
            for(size_t b = 0; b < i - 1; b++) {
                Mem_Free(*(buffer32 + b));
            }
            Mem_Free(buffer16);
            Mem_Free(buffer32);

        }
        if(!*(buffer32 + i)) {
            for(size_t a = 0; a < i; a++) {
                Mem_Free(*(buffer16 + a));
            }
            for(size_t b = 0; b < i ; b++) {
                Mem_Free(*(buffer32 + b));
            }
            free(buffer16);
            free(buffer32);
        }
        for(size_t j = 0; j < 8; j++) {
            checker16[i][j] = (char)(i + j);
            buffer16[i][j] = (char)(i+j);
        }
        for(size_t j = 0; j < 24; j++) {
            checker32[i][j] = (char)(i + j);
            buffer32[i][j] = (char)(i + j);
        }
    }
    
    for(size_t i = 0; i < 100; i++) {
        for(size_t j = 0; j < 24; j++) {

            assert(checker32[i][j] == buffer32[i][j]);
        }
        for(size_t j = 0; j < 8; j++) {
            assert(checker16[i][j] == buffer16[i][j]);

        }
        assert(Mem_Free(*(buffer16 + i)));
        assert(Mem_Free(*(buffer32 + i)));
    }
    assert(Mem_Free(buffer16));
    assert(Mem_Free(buffer32));

    free(heap);
    return 1;


}
