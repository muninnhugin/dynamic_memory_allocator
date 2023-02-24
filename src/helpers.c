#include "helpers.h"
#include "debug.h"
#include "icsmm.h"

/* Helper function definitions go here */
unsigned int calculate_ceiling(unsigned int a, unsigned int b)
{
    return (a / b) + ((a % b) != 0);
}

unsigned int get_total_payload(unsigned int requested_payload)
{
    return calculate_ceiling(requested_payload, PAYLOAD_UNIT);
}

void get_total_block_size(unsigned int requested_payload, unsigned int* block_size, unsigned int* padding)
{
    *block_size = 0;

    unsigned int total_payload = get_total_payload(requested_payload);
    *padding = total_payload - requested_payload;

    if(first_request) *block_size += PROLOGUE_SIZE + EPILOGUE_SIZE;
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

// TODO: add function to insert, which will call helper function set free header