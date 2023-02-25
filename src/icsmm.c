#include "icsmm.h"
#include "debug.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

int first_request = 1;
unsigned int available_mem = 0;

/*
 * The allocator MUST store the head of its free list in this variable. 
 * Doing so will make it accessible via the extern keyword.
 * This will allow ics_freelist_print to access the value from a different file.
 */
ics_free_header *freelist_head = NULL;

/*
 * This is your implementation of malloc. It acquires uninitialized memory from  
 * ics_inc_brk() that is 16-byte aligned, as needed.
 *
 * @param size The number of bytes requested to be allocated.
 *
 * @return If successful, the pointer to a valid region of memory of at least the
 * requested size is returned. Otherwise, NULL is returned and errno is set to 
 * ENOMEM - representing failure to allocate space for the request.
 * 
 * If size is 0, then NULL is returned and errno is set to EINVAL - representing
 * an invalid request.
 */
void* ics_malloc(size_t size) {
    // if size == 0, errno = EINVAL and return NULL
    if(size == 0)
    {
        errno = EINVAL;
        return NULL;
    }
    // get memory size we will need to fulfill request (accounting for first request)
    unsigned int block_size;
    unsigned int padding;
    get_total_block_size(size, &block_size, &padding);
    // if head ptr is NULL -- no free blocks of memory left
    if(freelist_head == NULL)
    {
        unsigned int num_of_pages = ceiling(block_size + PROLOGUE_SIZE + EPILOGUE_SIZE, PAGE_SIZE);
        // ask for more pages
        void* new_page_address = ics_inc_brk(num_of_pages);
        // if asking fail return NULL (errno is already ENOMEM)
        if(new_page_address == (void*) -1) return NULL;
        // set epilogue accordingly
        set_epilogue(ics_get_brk() - EPILOGUE_SIZE);
        // set available space as one free block (whose next ptr is NULL)
        void* block_address = new_page_address;
        if(first_request){
            set_prologue(new_page_address);
            block_address += PROLOGUE_SIZE;
            first_request = 0;
        }
        set_free_block(block_address, (num_of_pages * PAGE_SIZE) - EPILOGUE_SIZE - PROLOGUE_SIZE, NULL, NULL);
        freelist_head = block_address;
    }

    // traverse the list to find first fit block
    ics_free_header* cur_header = freelist_head;

    set_allocated_block(freelist_head, block_size, padding);
    // if we can split without splinters, split the block
        // set this block allocated
        // set next block unallocated
        // add next block to list
    // set head to next ptr
    return cur_header;
}

/*
 * Marks a dynamically allocated block as no longer in use and coalesces with 
 * adjacent free blocks (as specified by Homework Document). 
 * Adds the block to the appropriate bucket according to the block placement policy.
 *
 * @param ptr Address of dynamically allocated memory returned by the function
 * ics_malloc.
 * 
 * @return 0 upon success, -1 if error and set errno accordingly.
 */
int ics_free(void *ptr) {
    // research immediate reverse coalescing
    return -99999;
}

/********************** EXTRA CREDIT ***************************************/

/*
 * Resizes the dynamically allocated memory, pointed to by ptr, to at least size 
 * bytes. See Homework Document for specific description.
 *
 * @param ptr Address of the previously allocated memory region.
 * @param size The minimum size to resize the allocated memory to.
 * @return If successful, the pointer to the block of allocated memory is
 * returned. Else, NULL is returned and errno is set appropriately.
 *
 * If there is no memory available ics_malloc will set errno to ENOMEM. 
 *
 * If ics_realloc is called with an invalid pointer, set errno to EINVAL. See ics_free
 * for more details.
 *
 * If ics_realloc is called with a valid pointer and a size of 0, the allocated     
 * block is free'd and return NULL.
 */
void *ics_realloc(void *ptr, size_t size) {
    return NULL;
}
