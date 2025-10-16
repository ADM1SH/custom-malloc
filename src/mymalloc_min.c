/**
 * @file mymalloc_min.c
 * @brief A minimal educational implementation of a custom memory allocator.
 *
 * This implementation uses a single static array as a heap and a simple
 * first-fit algorithm to manage memory blocks. It does not support coalescing
 * of free blocks, focusing instead on clarity and simplicity.
 */

#include <stdio.h>
#include <stddef.h>
#include <string.h>

// The total size of the heap in bytes (1 MB).
#define HEAP_SIZE 1024 * 1024
// The alignment for allocated memory in bytes.
#define ALIGN     8

// The static array that represents our heap.
static unsigned char heap[HEAP_SIZE];

/**
 * @brief Represents a block of memory in the heap.
 *
 * Each block has a header that contains metadata about the block, including
 * its size, whether it is free, and a pointer to the next block in the heap.
 */
typedef struct Block {
    size_t size;        ///< The size of the payload area in bytes.
    int free;           ///< 1 if the block is free, 0 if it is in use.
    struct Block *next; ///< A pointer to the next block in the heap.
} Block;

// A pointer to the first block in the heap.
static Block *free_list = NULL;

/**
 * @brief Aligns a size to the next multiple of ALIGN.
 * @param size The size to align.
 * @return The aligned size.
 */
static size_t align8(size_t size) {
    return (size + 7) & ~7;
}

/**
 * @brief Initializes the heap.
 *
 * This function is called once when the allocator is first used. It sets up
 * a single large free block that covers the entire heap.
 */
void my_init() {
    free_list = (Block*)heap;
    free_list->size = HEAP_SIZE - sizeof(Block);
    free_list->free = 1;
    free_list->next = NULL;
}

/**
 * @brief Finds the first free block that is large enough to hold a given size.
 * @param size The required size of the payload.
 * @return A pointer to the first suitable free block, or NULL if none is found.
 */
static Block *find_free(size_t size) {
    Block *curr = free_list;
    while (curr) {
        if (curr->free && curr->size >= size)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

/**
 * @brief Splits a block into two if it is large enough.
 *
 * If a block is larger than the required size, this function splits it into
 * an allocated block of the required size and a new free block with the
 * remaining space.
 *
 * @param blk The block to split.
 * @param size The required size of the payload for the new allocated block.
 */
static void split(Block *blk, size_t size) {
    // Check if there is enough space for a new block header and a minimal payload.
    if (blk->size >= size + sizeof(Block) + ALIGN) {
        // Create a new block header for the remaining free space.
        Block *new_blk = (Block*)((char*)blk + sizeof(Block) + size);
        new_blk->size = blk->size - size - sizeof(Block);
        new_blk->free = 1;
        new_blk->next = blk->next;

        // Update the original block.
        blk->size = size;
        blk->next = new_blk;
    }
}

/**
 * @brief Allocates a block of memory of a given size.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
void *my_malloc(size_t size) {
    // Initialize the heap if it has not been initialized yet.
    if (!free_list) my_init();
    if (size == 0) return NULL;

    // Align the requested size.
    size = align8(size);

    // Find a suitable free block.
    Block *blk = find_free(size);
    if (!blk) return NULL; // Out of memory.

    // Split the block if it is large enough.
    split(blk, size);
    blk->free = 0;

    // Return a pointer to the payload.
    return (char*)blk + sizeof(Block);
}

/**
 * @brief Frees a previously allocated block of memory.
 * @param ptr A pointer to the memory to free.
 */
void my_free(void *ptr) {
    if (!ptr) return;

    // Check if the pointer is within the heap range.
    if ((unsigned char*)ptr < heap || (unsigned char*)ptr >= heap + HEAP_SIZE) {
        fprintf(stderr, "my_free: pointer %p is outside heap\n", ptr);
        return;
    }

    // Get a pointer to the block header.
    Block *blk = (Block*)((char*)ptr - sizeof(Block));

    // Check for double-free.
    if (blk->free) {
        fprintf(stderr, "my_free: double free or free of already free block %p\n", ptr);
        return;
    }

    // Mark the block as free.
    blk->free = 1;
}

/**
 * @brief Allocates and zeros out a block of memory.
 * @param n The number of elements to allocate.
 * @param s The size of each element.
 * @return A pointer to the allocated and zeroed memory, or NULL on failure.
 */
void *my_calloc(size_t n, size_t s) {
    size_t total = n * s;
    void *p = my_malloc(total);
    if (p) memset(p, 0, total);
    return p;
}

/**
 * @brief Resizes a previously allocated block of memory.
 * @param ptr A pointer to the memory to resize.
 * @param new_size The new size of the memory block.
 * @return A pointer to the resized memory block, or NULL on failure.
 */
void *my_realloc(void *ptr, size_t new_size) {
    if (!ptr) return my_malloc(new_size);

    Block *blk = (Block*)((char*)ptr - sizeof(Block));
    if (blk->size >= new_size) return ptr;

    // Allocate a new block, copy the data, and free the old block.
    void *newp = my_malloc(new_size);
    if (!newp) return NULL;
    memcpy(newp, ptr, blk->size);
    my_free(ptr);
    return newp;
}

/**
 * @brief Dumps the current state of the heap to the console.
 *
 * This function iterates through all the blocks in the heap and prints
 * information about each block, including its address, size, and free status.
 */
void my_dump() {
    Block *curr = free_list;
    printf("Heap dump:\n");
    while (curr) {
        printf(" Block %p | size=%lu | free=%d | next=%p\n", 
            (void*)curr, (unsigned long)curr->size, curr->free, (void*)curr->next);
        curr = curr->next;
    }
}