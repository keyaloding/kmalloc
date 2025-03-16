#ifndef _kmalloc_H_
#define _kmalloc_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGE_SIZE 4096
#define BYTE_ALIGN 8

/* Represents the header of an allocated/free memory block. The format in
 * memory is as follows:
 * - Block size (int, 4 bytes)
 * - Allocated (int, 4 bytes): 0 if free, 1 if allocated
 * - Next free block (pointer, 8 bytes)
 * - Previous free block (pointer, 8 bytes)
 * - Payload (allocated block only)
 * - Padding (only if extra bytes are needed to enforce 8-byte alignment)
 */
typedef struct mem_block {
  unsigned int size, allocated;
  mem_block *prev, *next;
} mem_block;

/* Allocates the initial heap area and sets global variables `head` and
 * `heap_address`. The page size is given by `PAGE_SIZE`, which is 4096.
 * If `size_of_region % PAGE_SIZE != 0`, extra bytes will be added to the heap
 * area. Returns true if the heap area is successfully set and false if there
 * is insufficient contiguous memory.
 */
bool heap_init(unsigned int size_of_region);

/* Returns a pointer to the start of the payload or `NULL` if there is not
 * enough contiguous free space within the memory allocated by `heap_init()`
 * to satisfy the request.
 */
void *kmalloc(unsigned int size_of_payload);

/* Frees the target block given the address of the start of the payload. */
void kfree(void *ptr);

#endif