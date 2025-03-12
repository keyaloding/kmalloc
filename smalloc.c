#include "smalloc.h"

mem_block *head;
void *heap_address;

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
  head = (mem_block*)heap;
  head->size = size_of_region;
  head->allocated = 0;
  head->prev = NULL; head->next = NULL;
  return 0;
}

void* smalloc(int size_of_payload, Malloc_Status* status) {
  if (size_of_payload < 0) {
    status->success = 0;
    status->payload_offset = -1;
    status->hops = 0;
    return NULL;
  }
  int alloc_size = size_of_payload + sizeof(mem_block), hops = 0;
  if (alloc_size % 8) {
    alloc_size += 8 - (alloc_size % 8);
  }
  mem_block* block = head;
  while (block && block->size < alloc_size) {
    hops++;
    block = block->next;
  }
  if (!block) {
    status->success = 0;
    status->payload_offset = -1;
    status->hops = 0;
    return NULL;
  }
  block->allocated = 1;
  int bytes_left = block->size - alloc_size;
  void* payload_start = (void*)(block + 1);
  if (bytes_left >= sizeof(mem_block)) {
    void *head_ptr = (void*)(block) + alloc_size;
    mem_block* new_block = (mem_block*)head_ptr;
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

void sfree(void* ptr) {
  printf("ptr at %p\n", ptr);
  mem_block* alloc_block = (mem_block*)(ptr - 24);
  printf("alloc_block at %p\n", alloc_block);
  // printf("ptr - alloc_block = %ld\n", (uintptr_t)ptr - (uintptr_t)alloc_block);
  alloc_block->allocated = 0;
  mem_block *right_block = head, *left_block = NULL;
  while (right_block && right_block < alloc_block) {
    right_block = right_block->next;
    if (!left_block) left_block = head;
    else left_block = left_block->next;
  }
  if ((!left_block || (left_block && left_block->allocated)) &&
      (!right_block) || (right_block && right_block->allocated)) {
    // Case 1: No coalescing
    if (left_block) {
      left_block->next = alloc_block;
    } else {
      head = alloc_block;
    }
    alloc_block->prev = left_block;
    if (right_block) {
      right_block->prev = alloc_block;
    } 
    alloc_block->next = right_block;
  } else if (1) {
    // Case 2: Coalesce with right block
    if (left_block) {
      left_block->next = alloc_block;
      alloc_block->prev = left_block;
      alloc_block->next = right_block->next;
      alloc_block->next->prev = alloc_block;
    } else {

    }
  } else if (1) {
    // Case 3: Coalesce with left block
    left_block->size += alloc_block->size;
  } else {
    // Case 4: Coalesce all 3 blocks
    left_block->size += alloc_block->size + right_block->size;
    left_block->next = right_block->next;
    if (left_block->next) {
      left_block->next->prev = left_block;
    }
  }
  if (left_block + left_block->size / 24 == alloc_block) {
    left_block->size += alloc_block->size;
  } else {
    left_block->next = alloc_block;
    alloc_block->prev = left_block;
    if (right_block) {
      right_block->prev = alloc_block;
      alloc_block->next = right_block;
    }
  }
}
