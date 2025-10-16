/**
 * @file mymalloc_min.h
 * @brief Header file for the minimal custom memory allocator.
 *
 * This file contains the function declarations for the minimal implementation
 * of a custom memory allocator.
 */

#ifndef MYMALLOC_MIN_H
#define MYMALLOC_MIN_H

#include <stddef.h>

/**
 * @brief Initializes the heap.
 */
void my_init(void);

/**
 * @brief Allocates a block of memory of a given size.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
void *my_malloc(size_t size);

/**
 * @brief Frees a previously allocated block of memory.
 * @param ptr A pointer to the memory to free.
 */
void my_free(void *ptr);

/**
 * @brief Allocates and zeros out a block of memory.
 * @param n The number of elements to allocate.
 * @param s The size of each element.
 * @return A pointer to the allocated and zeroed memory, or NULL on failure.
 */
void *my_calloc(size_t n, size_t s);

/**
 * @brief Resizes a previously allocated block of memory.
 * @param ptr A pointer to the memory to resize.
 * @param size The new size of the memory block.
 * @return A pointer to the resized memory block, or NULL on failure.
 */
void *my_realloc(void *ptr, size_t size);

/**
 * @brief Dumps the current state of the heap to the console.
 */
void my_dump(void);

#endif // MYMALLOC_MIN_H