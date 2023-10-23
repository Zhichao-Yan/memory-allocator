#ifndef MYALLOCATOR_H
#define MYALLOCATOR_H
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
typedef char ALIGN[16];

union header{
    ALIGN stub;
    struct{
        size_t size;
        unsigned free;
        union header *next;    
    } t;
};

typedef union header header_t;


header_t * get_free_block(size_t size);
void *mymalloc(size_t size);
void myfree(void *block);
void *mycalloc(size_t num, size_t nsize);
void *myrealloc(void *block, size_t size);
#endif