#ifndef SLAB_H
#define SLAB_H

#include <stddef.h>
#include <stdint.h>

#include "sync.h"

#define SLAB_MIN_OBJ_SIZE sizeof(void*)
// Align memory to word boundary (4 bytes for 32-bit MCUs)
#define SLAB_ALIGN(size) (((size) + sizeof(void*) - 1) & ~(sizeof(void*) - 1))

struct Slab_Header {
  struct Slab_Header* next;
};

typedef struct Slab {
  uint32_t obj_size;    // Actual size of one object (with alignment)
  uint32_t capacity;    // Total number of objects in this slab
  uint32_t free_count;  // Current number of free objects
  struct Slab_Header*
      free_list;    // Pointer to first free object (head of free list)
  uint8_t* memory;  // Start of memory region allocated for this slab
  mutex_t mutex;    // Mutex to access Slab's structure
} slab_t;

/**
    Initialize slab allocator over pre-allocated memory array
    slab - Pointer to slab_t structure
    memory - Pointer to start of raw memory (e.g., global array)
    memory_size - Total size of allocated memory in bytes
    req_obj_size - Requested size of one object
    return 0 on success, -1 on error (e.g., memory too small)
 */
int Slab_Init(slab_t* slab, void* memory, uint32_t memory_size,
              uint32_t req_obj_size);

/**
    Allocate one object from slab
    slab - Pointer to initialized slab
    return pointer to allocated object or NULL if memory exhausted
 */
void* Slab_Alloc(slab_t* slab);

/**
    Free object, returning it to slab
    slab - Pointer to slab
    ptr - Pointer to object to free (must have been obtained via slab_alloc)
 */
void Slab_Free(slab_t* slab, void* ptr);

#endif