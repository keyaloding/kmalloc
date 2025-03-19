# K Malloc

This repository includes two dynamic memory allocation functions, `kmalloc()`
and `kcalloc()`, which are closely related to the `malloc()` and `calloc()`
functions included in the C standard library. Dynamic memory allocation in C
is used to allocate memory at runtime, allowing the program to adjust th
amount of memory it needs based on input or conditions. It provides flexibility,
as the programmer can request memory of varying sizes, unlike static allocation.
This helps in managing memory more efficiently, especially for large data
structures like arrays or linked lists that are not known in advance. After
the data is used, it should be freed to allow the memory to be reallocated,
although this is technically not necessary.

## How to Use

First, clone the repository and create a new C/C++ file in the
`kmalloc` repository:

```Unix
git clone https://github.com/keyaloding/kmalloc
cd kmalloc
```

### `kmalloc(unsigned int num_bytes)`

Here is an example of how you might use `kmalloc()` and `kfree()` to
dynamically allocate, then free an integer array of length `n`. Note that the
memory allocated by `kmalloc()` is uninitialized and contains garbage values.

```C
#include "kmalloc.h"

int main(int argc, char **argv) {
  unsigned int heap_size = ...;
  if (!heap_init(heap_size)) {
    // Heap cannot be allocated. Add desired error handling
    return;
  }
  int *arr = (int*)kmalloc(sizeof(int) * n);
  if (!arr) {
    // Insufficient contiguous memory. Add desired error handling
    return;
  }
  // Do something with `arr`
  kfree(arr);
}
```

### `kcalloc(unsigned int num_elements, unsigned int element_size)`

Here is an example of how you might use `kcalloc()` and `kfree()` to
dynamically allocate, then free an integer array of length `n`. Note that the
memory allocated by `kmalloc()` is initialized to zero.

```C
#include "kmalloc.h"

int main(int argc, char **argv) {
  unsigned int heap_size = ...;
  if (!heap_init(heap_size)) {
    // Heap cannot be allocated. Add desired error handling
    return;
  }
  int *arr = (int*)kcalloc(n, sizeof(int));
  if (!arr) {
    // Insufficient contiguous memory. Add desired error handling
    return;
  }
  // Do something with `arr`
  kfree(arr);
}
```

Note that unlike the C `malloc()` and `calloc()` functions, the heap area must
be allocated manually.

## Technical Details

For more information, see `kmalloc.h` for function descriptions or `kmalloc.c`
for source code.
