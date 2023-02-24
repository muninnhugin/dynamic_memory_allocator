#ifndef HELPERS_H
#define HELPERS_H
#include <stdlib.h>
#include <stdint.h>

// memory constants in bytes
#define HEADER_SIZE 8
#define FOOTER_SIZE 8
#define PROLOGUE_SIZE FOOTER_SIZE
#define EPILOGUE_SIZE HEADER_SIZE
#define PAYLOAD_UNIT 16
#define MIN_BLOCK_SIZE 32

int first_request = 1;

unsigned int calculate_ceiling(unsigned int a, unsigned int b);
void get_total_block_size(unsigned int requested_payload, unsigned int* block_size, unsigned int* padding);
void set_epilogue(void* epilogue_address);
void set_prologue(void* prologue_address);
void set_header(void* header_address, unsigned int allocated, unsigned int block_size, unsigned int padding);
void set_footer(void* footer_address, unsigned int allocated, unsigned int block_size);


#endif
