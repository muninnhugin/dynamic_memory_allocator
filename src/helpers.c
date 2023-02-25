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
    freelist_head->prev = block_address;
    set_free_header(block_address, block_size, freelist_head, NULL);
    freelist_head = block_address;
}

