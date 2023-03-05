#include "icsmm.h"
#include "debug.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

int first_request = 1;

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

    int allocated = 0;
    void* result = NULL;
    // traverse the list to find first fit block
    ics_free_header* cur_header = freelist_head;
    while(cur_header != NULL)
    {
        if(cur_header->header.block_size >= block_size) {
            result = allocate(cur_header, block_size, padding);
            allocated = 1;
            break;
        }
        cur_header = cur_header->next;
    }

    if(!allocated)
    {
        ics_free_header* new_memory = ask_for_memory(size, block_size, &first_request);
        if(!new_memory)  return NULL;
        result = allocate(new_memory, block_size, padding);
    }

    return result + HEADER_SIZE;
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

    void* block_address = ptr - HEADER_SIZE;
    if(is_not_valid_allocated_block(block_address))
        return -1;
    ics_header* header =  block_address;
    unsigned int block_size = header->block_size;
    block_size -= 1;

    higher_address_coalesce(block_address, &block_size);
    /********************** UNCOMMENT FOR EXTRA CREDIT ************************************/
    //lower_address_coalesce(&block_address, &block_size);

    set_free_block(block_address, block_size, NULL, NULL);
    insert_head(block_address, block_size);

    return 0;
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
    void* block_address = ptr - HEADER_SIZE;
    if(is_not_valid_allocated_block(block_address))
        return NULL;
    if(size == 0)
    {
        ics_free(block_address);
        return NULL;
    }

    ics_header* header = block_address;
    unsigned int block_size = header->block_size;
    higher_address_coalesce(block_address, &block_size);
    unsigned int usable_payload = block_size - FOOTER_SIZE - HEADER_SIZE;
    if(usable_payload >= size)
    {
        allocate(block_address, block_size, usable_payload - size);
        return block_address + HEADER_SIZE;
    }

    // TODO: implement what to do if new coalesced block cannot accommodate resize
    // consider calling ics_malloc()
    // if malloc unsuccessful
        // do as requirements
    // copy payload of old block to new block
    // set old block as free
    // test of course

    return block_address + HEADER_SIZE;
}