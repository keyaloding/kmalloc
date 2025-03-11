#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "smalloc.h"

typedef struct free_block free_block;
struct free_block {
  int size, allocated;
  free_block *prev, *next;
};

free_block *head=NULL;
void *heap_address=NULL;

/*
 * my_init() is called one time by the application program to to perform any
 * necessary initializations, such as allocating the initial heap area.
 * size_of_region is the number of bytes that you should request from the OS using
 * mmap().
 * Note that you need to round up this amount so that you request memory in
 * units of the page size, which is defined as 4096 Bytes in this project.
 */
int my_init(int size_of_region) {
  if (size_of_region % 4096) {
    size_of_region += 4096 - (size_of_region % 4096);
  }
  int fd = open("/dev/zero", O_RDWR);
  void* heap = mmap(NULL, size_of_region, PROT_WRITE | PROT_READ,
                    MAP_SHARED, fd, 0);
  if (close(fd) < 0 || heap == MAP_FAILED)  {
    return -1;
  }
  heap_address = heap;
  head = (free_block*)heap;
  head->size = size_of_region;
  head->allocated = 0;
  head->prev = NULL; head->next = NULL;
  return 0;
}

/*
 * smalloc() takes as input the size in bytes of the payload to be allocated and
 * returns a pointer to the start of the payload. The function returns NULL if
 * there is not enough contiguous free space within the memory allocated
 * by my_init() to satisfy this request.
 */
void* smalloc(int size_of_payload, Malloc_Status *status) {
  if (size_of_payload < 0) {
    status->success = 0;
    status->payload_offset = -1;
    status->hops = 0;
    return NULL;
  }
  int alloc_size = size_of_payload + 24, hops = 0;
  if (alloc_size % 8) {
    alloc_size += 8 - (alloc_size % 8);
  }
  free_block* block = head;
  while (block && block->size < alloc_size) {
    block = block->next;
    hops++;
  }
  if (!block) {
    status->success = 0;
    status->payload_offset = -1;
    status->hops = 0;
    return NULL;
  }
  block->allocated = 1;
  int bytes_left = block->size - alloc_size;
  void* payload_start = (void *)(block + 1);
  if (bytes_left >= 24) {
    void *head_ptr = (void *)(block) + alloc_size;
    free_block* new_block = (free_block*)head_ptr;
    new_block->size = bytes_left;
    new_block->allocated = 0;
    if (block->prev) {
      new_block->prev = block->prev;
      new_block->prev->next = new_block;
    } else {
      head = new_block;
    }
    if (block->next) {
      new_block->next = block->next;
      new_block->next->prev = new_block;
    }
  } else {
    block->size += bytes_left;
    if (block->prev) {
      block->prev->next = block->next;
      if (block->next) {
        block->next->prev = block->prev;
      }
    } else {
      head = block->next;
    }
  }
  status->hops = hops;
  status->payload_offset = (unsigned long)payload_start - (unsigned long)heap_address;
  status->success = 1;
  return payload_start;
}

/*
 * sfree() frees the target block. "ptr" points to the start of the payload.
 * NOTE: "ptr" points to the start of the payload, rather than the block (header).
 */
void sfree(void *ptr) {
  free_block* block = (free_block*)(ptr - 24);
  // printf("payload address: %p\n", ptr);
  // printf("block size: %d\n", block->size);
  block->allocated = 0;
  // printf("--1--\n");
  // if (!block->prev && block->next && !block->next->allocated) {
  //   block->size += block->next->size;
  //   block->next = block->next->next;
  // } else if (block->prev && !block->next && !block->prev->allocated) {
  //   block->prev->size += block->size;
  //   block->prev->next = block->next;
  // } else if (block->prev && block->next) {
  //   if (!block->next->allocated) {
  //     block->size += block->next->size;
  //     block->next = block->next->next;
  //   }
  //   if (!block->prev->allocated) {
  //     block->prev->size += block->size;
  //     block->prev->next = block->next;
  //   }
  // }
  if (block->next) {
    if (block->next->allocated) {

    }
  }
  if (block->prev) {
    if (block->prev->allocated) {

    }
  }
}
