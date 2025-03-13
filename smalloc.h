#ifndef _smalloc_H_
#define _smalloc_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
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

/* Gives information about the status of the `malloc()` call. */
typedef struct Malloc_Status{
  /* true if successful, false if unsuccessful. */
  bool success;
  
  /* Offset of the payload in the heap (in bytes). This is equal to
   * (unsigned long)payload_pointer - (unsigned long)heap_start_address.
   * If malloc fails, this is -1.
   */
  int payload_offset;

  /* Number of hops it takes to find the first-fit block.
   * e.g. hops = 0 if the first free block (the head) of the free list can fit
   * the paylod. hops = 1 if the second free block can fit the payload.
   * hops = -1 if malloc fails
   */
  int hops;
} Malloc_Status;

/* Called one time by the application program to allocate the initial heap area
 * and set global variables `head` and `heap_address`. The size of the heap area
 * must be a multiple of the page size, 4096.
 */
int my_init(unsigned int size_of_region);

/* Returns a pointer to the start of the payload or NULL if there is not enough
 * contiguous free space within the memory allocated by my_init() to satisfy
 * the request.
 */
void *smalloc(unsigned int size_of_payload, Malloc_Status* status);

/* Frees the target block. `ptr` points to the start of the payload.
 */
void sfree(void *ptr);

#endif