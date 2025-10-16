/**
 * @file mymalloc_adv.c
 * @brief An advanced implementation of a custom memory allocator.
 *
 * This implementation uses a segregated free list with bins for different size
 * classes to improve allocation performance. It also supports coalescing of
 * adjacent free blocks to reduce external fragmentation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// The total size of the heap in bytes (1 MB).
#define HEAP_SIZE (1024*1024)
// The alignment for allocated memory in bytes.
#define ALIGN 8
// The number of bins in the segregated free list.
#define NUM_BINS 6

// The static array that represents our heap, aligned to ALIGN.
static unsigned char heap[HEAP_SIZE] __attribute__((aligned(ALIGN)));
// A flag to indicate if the heap has been initialized.
static int heap_initialized = 0;

/**
 * @brief Represents a block of memory in the heap.
 *
 * Each block has a header with metadata, including its size, free status,
 * and pointers for both the physical list of all blocks and the free list.
 */
typedef struct Block {
    size_t size;                 ///< The size of the payload area in bytes.
    int free;                    ///< 1 if the block is free, 0 if it is in use.
    struct Block *prev_phys;     ///< A pointer to the previous physical block in the heap.
    struct Block *next_phys;     ///< A pointer to the next physical block in the heap.
    struct Block *prev_free;     ///< A pointer to the previous block in the free list.
    struct Block *next_free;     ///< A pointer to the next block in the free list.
} Block;

// An array of pointers to the first block in each bin of the segregated free list.
static Block *bin[NUM_BINS];
// A pointer to the first block in the heap.
static Block *heap_start = NULL;

/**
 * @brief Aligns a size to the next multiple of ALIGN.
 * @param n The size to align.
 * @return The aligned size.
 */
static size_t align8(size_t n) { return (n + 7) & ~7; }

/**
 * @brief Writes the size of a block to its footer.
 * @param b A pointer to the block.
 */
static void write_footer(Block *b) {
    *((size_t*)((char*)b + sizeof(Block) + b->size)) = b->size;
}

/**
 * @brief Reads the size of a block from its footer.
 * @param b A pointer to the block.
 * @return The size of the block.
 */
static size_t read_footer(Block *b) {
    return *((size_t*)((char*)b + sizeof(Block) + b->size));
}

/**
 * @brief Determines the bin index for a given size.
 * @param size The size of the block.
 * @return The index of the bin for the given size.
 */
static int size_to_bin(size_t size) {
    if (size <= 64) return 0;
    if (size <= 128) return 1;
    if (size <= 256) return 2;
    if (size <= 512) return 3;
    if (size <= 1024) return 4;
    return 5;
}

/**
 * @brief Inserts a block into the appropriate free list.
 * @param b A pointer to the block to insert.
 */
static void insert_free(Block *b) {
    int idx = size_to_bin(b->size);
    b->prev_free = NULL;
    b->next_free = bin[idx];
    if (bin[idx]) bin[idx]->prev_free = b;
    bin[idx] = b;
}

/**
 * @brief Removes a block from its free list.
 * @param b A pointer to the block to remove.
 */
static void remove_free(Block *b) {
    int idx = size_to_bin(b->size);
    if (b->prev_free) b->prev_free->next_free = b->next_free;
    else bin[idx] = b->next_free;
    if (b->next_free) b->next_free->prev_free = b->prev_free;
    b->prev_free = b->next_free = NULL;
}

/**
 * @brief Initializes the heap.
 *
 * This function is called once when the allocator is first used. It sets up
 * a single large free block that covers the entire heap and initializes the
 * free list bins.
 */
static void heap_init() {
    heap_start = (Block*)heap;
    heap_start->size = HEAP_SIZE - sizeof(Block) - sizeof(size_t);
    heap_start->free = 1;
    heap_start->prev_phys = heap_start->next_phys = NULL;
    write_footer(heap_start);
    for (int i=0;i<NUM_BINS;i++) bin[i] = NULL;
    insert_free(heap_start);
    heap_initialized = 1;
}

/**
 * @brief Finds a suitable free block for a given size.
 *
 * This function searches the segregated free list for a block that is large
 * enough to hold the requested size. It starts with the appropriate bin for
 * the size and continues to larger bins if no suitable block is found.
 *
 * @param size The required size of the payload.
 * @return A pointer to a suitable free block, or NULL if none is found.
 */
static Block* find_fit(size_t size) {
    int idx = size_to_bin(size);
    for (int i = idx; i < NUM_BINS; i++) {
        Block *b = bin[i];
        while (b) {
            if (b->size >= size) return b;
            b = b->next_free;
        }
    }
    return NULL;
}

/**
 * @brief Splits a block into two if it is large enough.
 *
 * If a block is larger than the required size, this function splits it into
 * an allocated block of the required size and a new free block with the
 * remaining space. The new free block is then inserted into the free list.
 *
 * @param b The block to split.
 * @param size The required size of the payload for the new allocated block.
 */
static void split_block(Block *b, size_t size) {
    size_t remaining = b->size - size;
    if (remaining < sizeof(Block) + sizeof(size_t) + ALIGN) return;

    // Create a new block for the remaining space.
    Block *newb = (Block*)((char*)b + sizeof(Block) + size + sizeof(size_t));
    newb->size = remaining - sizeof(Block) - sizeof(size_t);
    newb->free = 1;
    newb->prev_phys = b;
    newb->next_phys = b->next_phys;
    if (b->next_phys) b->next_phys->prev_phys = newb;

    // Update the original block.
    b->next_phys = newb;
    b->size = size;
    write_footer(b);
    write_footer(newb);

    // Insert the new free block into the free list.
    insert_free(newb);
}

/**
 * @brief Allocates a block of memory of a given size.
 * @param size The number of bytes to allocate.
 * @return A pointer to the allocated memory, or NULL if the allocation fails.
 */
void *my_malloc(size_t size) {
    if (!heap_initialized) heap_init();
    size = align8(size);

    // Find a suitable free block.
    Block *b = find_fit(size);
    if (!b) return NULL; // Out of memory.

    // Remove the block from the free list.
    remove_free(b);
    // Split the block if it is large enough.
    split_block(b, size);
    b->free = 0;
    write_footer(b);

    // Return a pointer to the payload.
    return (char*)b + sizeof(Block);
}

/**
 * @brief Merges a free block with its adjacent free neighbors.
 * @param b A pointer to the block to coalesce.
 */
static void coalesce(Block *b) {
    // Merge with the next block if it is free.
    if (b->next_phys && b->next_phys->free) {
        remove_free(b->next_phys);
        b->size += sizeof(Block) + sizeof(size_t) + b->next_phys->size;
        b->next_phys = b->next_phys->next_phys;
        if (b->next_phys) b->next_phys->prev_phys = b;
        write_footer(b);
    }

    // Merge with the previous block if it is free.
    if (b->prev_phys && b->prev_phys->free) {
        remove_free(b->prev_phys);
        b->prev_phys->size += sizeof(Block) + sizeof(size_t) + b->size;
        b->prev_phys->next_phys = b->next_phys;
        if (b->next_phys) b->next_phys->prev_phys = b->prev_phys;
        write_footer(b->prev_phys);
        b = b->prev_phys;
    }

    // Insert the coalesced block into the free list.
    insert_free(b);
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
    Block *b = (Block*)((char*)ptr - sizeof(Block));

    // Check for double-free.
    if (b->free) {
        fprintf(stderr, "my_free: double free or free of already free block %p\n", ptr);
        return;
    }

    // Mark the block as free and coalesce it with its neighbors.
    b->free = 1;
    coalesce(b);
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
 * @param size The new size of the memory block.
 * @return A pointer to the resized memory block, or NULL on failure.
 */
void *my_realloc(void *ptr, size_t size) {
    if (!ptr) return my_malloc(size);

    Block *b = (Block*)((char*)ptr - sizeof(Block));
    if (b->size >= size) return ptr;

    // Allocate a new block, copy the data, and free the old block.
    void *newp = my_malloc(size);
    if (!newp) return NULL;
    memcpy(newp, ptr, b->size);
    my_free(ptr);
    return newp;
}

/**
 * @brief Dumps the current state of the heap to the console.
 *
 * This function prints the contents of each bin in the segregated free list.
 */
void my_dump() {
    printf("=== Heap bins ===\n");
    for (int i=0;i<NUM_BINS;i++) {
        printf("Bin[%d]: ", i);
        Block *b = bin[i];
        while (b) {
            printf("[%zu]", b->size);
            if (b->next_free) printf("->");
            b = b->next_free;
        }
        printf("\n");
    }
}