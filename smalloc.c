#include "smalloc.h"

#define PAGE_SIZE 4096
#define BYTE_ALIGN 8

mem_block *head;
void *heap_address;

int my_init(int size_of_region) {
  if (size_of_region % PAGE_SIZE) {
    size_of_region += PAGE_SIZE - (size_of_region % PAGE_SIZE);
  }
  int fd = open("/dev/zero", O_RDWR);
  void* heap = mmap(NULL, size_of_region, PROT_WRITE | PROT_READ,
                    MAP_SHARED, fd, 0);
  if (close(fd) < 0 || heap == MAP_FAILED)  {
    fprintf(stderr, "Requested heap size exceeds available memory\n");
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
    status->hops = -1;
    return NULL;
  }
  int alloc_size = size_of_payload + sizeof(mem_block), hops = 0;
  if (alloc_size % BYTE_ALIGN) {
    alloc_size += BYTE_ALIGN - (alloc_size % BYTE_ALIGN);
  }
  mem_block* block = head;
  while (block && block->size < alloc_size) {
    hops++;
    block = block->next;
  }
  if (!block) {
    status->success = 0;
    status->payload_offset = -1;
    status->hops = -1;
    return NULL;
  }
  block->allocated = 1;
  int bytes_left = block->size - alloc_size;
  void* payload_start = (void*)(block + 1);
  if (bytes_left >= sizeof(mem_block)) {
    void *head_ptr = (void*)(block) + alloc_size;
    mem_block* new_block = (mem_block*)head_ptr;
    new_block->size = bytes_left;
    block->size -= new_block->size;
    new_block->allocated = 0;
    new_block->prev = block->prev;
    if (new_block->prev) {
      new_block->prev->next = new_block;
    } else {
      head = new_block;
    }
    new_block->next = block->next;
    if (new_block->next) {
      new_block->next->prev = new_block;
    }
  } else {
    block->size += bytes_left;
    if (block->next) {
      block->next->prev = block->prev;
    }
    if (block->prev) {
      block->prev->next = block->next;
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
  if (!ptr) return;
  mem_block* alloc_block = (mem_block*)(ptr - sizeof(mem_block));
  alloc_block->allocated = 0;
  if (!head) {
    head = alloc_block;
    head->prev = NULL, head->next = NULL;
    return;
  }
  mem_block *left_block = NULL, *right_block = head;
  while (right_block && right_block < alloc_block) {
    right_block = right_block->next;
    if (left_block) {
      left_block = left_block->next;
    } else {
      left_block = head;
    }
  }
  bool merge_left = false, merge_right = false;
  char* p;
  if (left_block) {
    p = (char*)left_block + left_block->size;
    if ((mem_block*)p == alloc_block) {
      merge_left = true;
    }
  }
  p = (char*)alloc_block + alloc_block->size;
  if ((mem_block*)p == right_block) {
    merge_right = true;
  }
  if (!merge_left && !merge_right) {
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
  } else if (!merge_left && merge_right) {
    alloc_block->size += right_block->size;
    if (left_block) {
      left_block->next = alloc_block;
    }
    else {
      head = alloc_block;
    }
    alloc_block->prev = left_block;
    alloc_block->next = right_block->next;
    if (alloc_block->next) {
      alloc_block->next->prev = alloc_block;
    }
  } else if (merge_left && !merge_right) {
    left_block->size += alloc_block->size;
  } else {
    left_block->size += alloc_block->size + right_block->size;
    left_block->next = right_block->next;
    if (right_block->next) {
      right_block->next->prev = alloc_block;
    }
  }
}
