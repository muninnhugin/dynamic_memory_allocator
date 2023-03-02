#ifndef HELPERS_H
#define HELPERS_H
#include <stdlib.h>
#include <stdint.h>
#include "icsmm.h"

// memory constants in bytes
#define HEADER_SIZE 8
#define FOOTER_SIZE 8
#define PROLOGUE_SIZE FOOTER_SIZE
#define EPILOGUE_SIZE HEADER_SIZE
#define PAYLOAD_UNIT 16
#define MIN_BLOCK_SIZE 32
#define ALLOCATED 1
#define NOT_ALLOCATED 0

unsigned int ceiling(unsigned int a, unsigned int b);
void get_total_block_size(unsigned int requested_payload, unsigned int* block_size, unsigned int* padding);
void set_epilogue(void* epilogue_address);
void set_prologue(void* prologue_address);
void set_footer(void* footer_address, unsigned int allocated, unsigned int block_size);
void set_free_header(void* free_header_address, unsigned int block_size, ics_free_header* next, ics_free_header* prev);
unsigned int get_size_from_block_size(unsigned int block_size);
void set_allocated_block(void* block_address, unsigned int block_size, unsigned int padding);
void set_free_block(void* block_address, unsigned int block_size, ics_free_header* next, ics_free_header* prev);
void insert_head(void* block_address, unsigned int block_size);
void* allocate(void* original_block_address, unsigned int block_size, unsigned int padding);
ics_free_header* ask_for_memory(size_t size, unsigned int requested_size, int* first_request);
int is_free_block(void* block_address);
void remove_from_freelist(void* block_address);

#endif
