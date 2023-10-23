#include "myallocator.h"


static header_t *head = NULL,*tail = NULL;

pthread_mutex_t malloc_lock = PTHREAD_MUTEX_INITIALIZER;


// get_free_block

header_t * get_free_block(size_t size)
{
    header_t *cur = head;
    while(cur)
    {
        if(cur->t.free && cur->t.size >= size)
            break;
        cur = cur->t.next;
    }
    return cur;
}

// malloc size bytes of memory
void *mymalloc(size_t size)
{
    size_t total_size;
    void *block;
    header_t *header;
    if(size == 0)
        return NULL;
    pthread_mutex_lock(&malloc_lock);
    header = get_free_block(size); // search the block list 
    if(header) // if sucessfully find a block greater than size
    {
        header->t.free = 0; // set it as not free
        pthread_mutex_unlock(&malloc_lock); // now we can unlock the malloc_lock
        return (void*)(header + 1);
    }
    // what if we cannot find a block greater than size,we need to sbrk()
    total_size = sizeof(header_t) + size; // total size need to be allocated
    
    block = sbrk(total_size); // since sbrk is not thread safe,still locked
    // failed to sbrk, maybe because of reasons as follows
    // 1. The maximum possible size of a data segment was exceeded
    // 2. Insufficient space existed in the swap area to support the expansion
    // 3. The limit, as set by setrlimit(2), was exceeded
    if(block == (void*)-1) 
    {
        pthread_mutex_unlock(&malloc_lock);
        return NULL;
    }
    // successfully sbrk
    // 
    header = block;
    header->t.free = 0;
    header->t.size = size;
    header->t.next = NULL;
    if(!head)
    {
        head = header;
        tail = header;
    }else{
        tail->t.next = header;
        tail = header;
    }
    pthread_mutex_unlock(&malloc_lock);
    printf("mymalloc() successfully\n");
    return (void*)(header + 1);
}

void myfree(void *block)
{
    header_t *header,*temp;
    void *brk;
    if(!block) // when block is NULL
    {
        printf("%p is a NULL pointer\n",block);
        return;
    }
    pthread_mutex_lock(&malloc_lock);
    header = (header_t*)block - 1; // acquire the header of the block

    brk = sbrk(0);  // acquire the brk
    if((char*)block + header->t.size == brk) // means this block is at the end of heap
    {
        if(head == tail)
        {
            head = NULL;
            tail = NULL;
        }else{
            temp = head;
            while(temp)
            {
                if(temp->t.next == tail)
                {
                    temp->t.next = NULL;
                    tail = temp;
                }
                temp = temp->t.next;
            }
        }
        sbrk(0 - header->t.size - sizeof(header_t));
        pthread_mutex_unlock(&malloc_lock); // unlock it
        printf("%p freed successfully\n",block);
        return;
    }
    if(header->t.free == 1|| block > brk) // if block has been freed
    {
        pthread_mutex_unlock(&malloc_lock); // unlock it
        printf("%p has been freed before\n",block);
        return;
    }
    header->t.free = 1;
    pthread_mutex_unlock(&malloc_lock); // unlock it
    printf("%p freed successfully\n",block);
    return;
}
/*
    If the multiplication of nmemb and size would result in integer
    overflow, then calloc() returns an error.  By contrast, an
    integer overflow would not be detected in the following call to
    malloc(), with the result that an incorrectly sized block of
    memory would be allocated,in this case,we would get smaller 
    size of block than nmemb * size,but we didn't know that

        malloc(nmemb * size); 
*/
void *mycalloc(size_t num, size_t nsize)
{
    size_t size;
    void *block;
    if(num == 0 || nsize == 0)
    {
        return NULL;
    }
    size = num * nsize;
    if(nsize != size / num) // do a check for multiplicative overflow
    {
        errno = ERANGE;
        printf("num * nsize 乘法溢出\n");
        return NULL;
    }
    block = mymalloc(size);
    if(!block)
        return NULL;
    memset(block,0,size);
    return block;
}
void *myrealloc(void *block, size_t size)
{
    void *ret;
    header_t *header;
    // If block is NULL, then the call is equivalent to malloc(size), for
    // all values of size.
    if(block == NULL)
    {
        return mymalloc(size);
    }
    // If size is equal to zero, and block is not NULL, then the call is
    // equivalent to free(ptr)
    if(block != NULL && size == 0)
    {
        myfree(block);
        return NULL;
    }
    header = (header_t*)block - 1;
    if (header->t.size >= size)
		return block;
    ret = mymalloc(size);
    if(ret){
        memcpy(ret, block, header->t.size);
        myfree(block);
    }
    return ret;
}