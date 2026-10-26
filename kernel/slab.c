#include "slab.h"

#include "kassert.h"
#include "portable.h"
#include "sync.h"

int Slab_Init(slab_t* slab, void* memory, uint32_t memory_size,
              uint32_t req_obj_size) {
  if (!slab || !memory || memory_size == 0) {
    return -1;
  }

  // Align object size
  uint32_t aligned_size = SLAB_ALIGN(req_obj_size);
  if (aligned_size < SLAB_MIN_OBJ_SIZE) {
    aligned_size = SLAB_MIN_OBJ_SIZE;
  }

  // Check if at least one object fits
  if (memory_size < aligned_size) {
    return -1;
  }

  spinlock_lock(&slab->lock);
  slab->obj_size = aligned_size;
  slab->capacity = memory_size / aligned_size;
  slab->free_count = slab->capacity;
  slab->memory = (uint8_t*)memory;

  // Initialize free list
  // We use the memory of objects themselves to store pointers to next free
  // objects
  slab->free_list = (struct Slab_Header*)slab->memory;

  struct Slab_Header* current = slab->free_list;
  for (uint32_t i = 1; i < slab->capacity; i++) {
    // Point to next block
    current = (struct Slab_Header*)(slab->memory + i * slab->obj_size);
    // Move to next block
    current = current->next;
  }
  current->next = NULL;

  spinlock_unlock(&slab->lock);
  return 0;
}

void* Slab_Alloc(slab_t* slab) {
  if (!slab || slab->free_count == 0) {
    return NULL;  // Memory exhausted
  }

  // Atomic operation: disable interrupts while modifying the list
  spinlock_lock(&slab->lock);

  // Take first element from free list
  struct Slab_Header* allocated_ptr = slab->free_list;

  // Head of list becomes next element
  slab->free_list = allocated_ptr->next;

  slab->free_count--;

  spinlock_unlock(&slab->lock);

  return allocated_ptr;
}

void Slab_Free(slab_t* slab, void* ptr) {
  if (!slab || !ptr) {
    return;
  }

  // Simple check for pointer belonging to this slab
  if (ptr < (void*)slab->memory ||
      ptr >= (void*)(slab->memory + slab->capacity * slab->obj_size)) {
    panic("Slab invalid free");
  }

  spinlock_lock(&slab->lock);

  // Add freed block to the beginning of free list
  ((struct Slab_Header*)ptr)->next = slab->free_list;
  slab->free_list = (struct Slab_Header*)ptr;

  slab->free_count++;

  spinlock_unlock(&slab->lock);
}