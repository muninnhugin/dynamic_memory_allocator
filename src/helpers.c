#include "helpers.h"
#include "debug.h"
#include "icsmm.h"

/* Helper function definitions go here */
unsigned int ceiling(unsigned int a, unsigned int b)
{
    int result = (a / b) + ((a % b) != 0);
    return result;
}

void get_total_block_size(unsigned int requested_payload, unsigned int* block_size, unsigned int* padding)
{
    *block_size = 0;
    unsigned int total_payload = ceiling(requested_payload, PAYLOAD_UNIT) * PAYLOAD_UNIT;
    *padding = total_payload - requested_payload;
    *block_size += HEADER_SIZE + FOOTER_SIZE;
    *block_size += total_payload;
}

void set_epilogue(void* epilogue_address)
{
    ics_header* epilogue = epilogue_address;
    epilogue->block_size = 1;
    epilogue->hid = EPILOGUE_MAGIC;
    epilogue->padding_amount = 0;
}

void set_prologue(void* prologue_address)
{
    ics_footer* prologue = prologue_address;
    prologue->block_size = 1;
    prologue->fid = PROLOGUE_MAGIC;
}

void set_header(void* header_address, unsigned int allocated, unsigned int block_size, unsigned int padding)
{
    ics_header* header = header_address;
    header->block_size = block_size + allocated;
    header->padding_amount = padding;
    header->hid = HEADER_MAGIC;
}

void set_footer(void* footer_address, unsigned int allocated, unsigned int block_size)
{
    ics_footer* footer = footer_address;
    footer->block_size = block_size + allocated;
    footer->fid = FOOTER_MAGIC;
}

void set_free_header(void* free_header_address, unsigned int block_size,
                     struct ics_free_header *next, struct ics_free_header *prev)
{
    ics_free_header* free_header = free_header_address;
    set_header(free_header_address, NOT_ALLOCATED, block_size, 0);
    free_header->next = next;
    free_header->prev = prev;
}

unsigned int get_size_from_block_size(unsigned int block_size)
{
    return (block_size >> 1) << 1;
}

void set_allocated_block(void* block_address, unsigned int block_size, unsigned int padding)
{
    set_header(block_address, ALLOCATED, block_size, padding);
    void* footer_address = block_address + block_size - FOOTER_SIZE;
    set_footer(footer_address, ALLOCATED, block_size);
}

void set_free_block(void* block_address, unsigned int block_size,
                    ics_free_header* next, ics_free_header* prev)
{
    set_free_header(block_address, block_size, next, prev);
    void* footer_address = block_address + block_size - FOOTER_SIZE;
    set_footer(footer_address, NOT_ALLOCATED, block_size);
}

void insert_head(void* block_address, unsigned int block_size)
{
    if(freelist_head)   freelist_head->prev = block_address;
    set_free_header(block_address, block_size, freelist_head, NULL);
    freelist_head = block_address;
}

void* allocate(void* original_block_address, unsigned int block_size, unsigned int padding)
{
    ics_free_header* original_block = original_block_address;
    unsigned int leftover_memory = original_block->header.block_size - block_size;
    freelist_head = freelist_head->next;
    if(leftover_memory < 32) // allocate everything to request
    {
        set_allocated_block(original_block_address,
                            block_size + leftover_memory, padding + leftover_memory);
    }
    else    // split
    {
        set_allocated_block(original_block, block_size, padding);
        void* new_block_addr = original_block_address + block_size;
        set_free_block(new_block_addr, leftover_memory, NULL, NULL);
        insert_head(new_block_addr, leftover_memory);
    }
    return original_block_address;
}

ics_free_header* ask_for_memory(size_t size, unsigned int requested_size, int* first_request)
{
    if(*first_request)  requested_size += PROLOGUE_SIZE + EPILOGUE_SIZE;
    unsigned int num_of_pages = ceiling(requested_size, PAGE_SIZE);
    // ask for more pages
    void* new_page_address = ics_inc_brk(num_of_pages);
    // if asking fail return NULL (errno is already ENOMEM)
    if(new_page_address == (void*) -1) return NULL;
    // set epilogue accordingly
    set_epilogue(ics_get_brk() - EPILOGUE_SIZE);
    // set available space as one free block (whose next ptr is NULL)
    unsigned int block_size = (num_of_pages * PAGE_SIZE);
    void* block_address = new_page_address;
    if(*first_request){
        block_size -= EPILOGUE_SIZE + PROLOGUE_SIZE;

        set_prologue(new_page_address);
        block_address += PROLOGUE_SIZE;
        *first_request = 0;
    }
    else
        block_address -= EPILOGUE_SIZE;
    set_free_block(block_address, block_size, NULL, NULL);
    insert_head(block_address, block_size);
    return block_address;
}