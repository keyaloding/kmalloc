# K Malloc

K Malloc is a dynamic memory allocator, similar to the `malloc()` function
built in to the C standard library. Dynamic memory allocation in C is used to
allocate memory at runtime, allowing the program to adjust the amount of memory
it needs based on input or conditions. It provides flexibility, as the
programmer can request memory of varying sizes, unlike static allocation.
This helps in managing memory more efficiently, especially for large data
structures like arrays or linked lists that are not known in advance. After
the data is used, it should be freed to allow the memory to be reallocated,
although this is technically not necessary.

## How to Use

Here is an example of how you might use `kmalloc()` and `kfree()` to dynamically allocate an integer array of length `n`.

```C
#include "kmalloc.h"

int main(int argc, char** argv) {
  unsigned int heap_size = ...;
  if (!heap_init(heap_size)) {
    // Heap cannot be allocated. Add desired error handling
    return;
  }
  int* arr = (int*)kmalloc(sizeof(int) * n);
  if (!arr) {
    // Insufficient contiguous memory. Add desired error handling
    return;
  }
  // Do something with `arr`
  kfree(arr);
}
```

Note that unlike the C `malloc()` function, the heap area must be allocated manually.

## Technical Details

For more information, see `kmalloc.h` for function descriptions or `kmalloc.c` for source code.
