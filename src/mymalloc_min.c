#include <stdio.h>
#include <stddef.h>
#include <string.h>

#define HEAP_SIZE 1024 * 1024  // 1 MB heap
#define ALIGN     8

static unsigned char heap[HEAP_SIZE];

typedef struct Block {
    size_t size;        // payload size
    int free;           // 1 = free, 0 = used
    struct Block *next; // next block
} Block;

static Block *free_list = NULL;

// align helper
static size_t align8(size_t size) {
    return (size + 7) & ~7;
}

void my_init() {
    free_list = (Block*)heap;
    free_list->size = HEAP_SIZE - sizeof(Block);
    free_list->free = 1;
    free_list->next = NULL;
}

// find first-fit free block
static Block *find_free(size_t size) {
    Block *curr = free_list;
    while (curr) {
        if (curr->free && curr->size >= size)
            return curr;
        curr = curr->next;
    }
    return NULL;
}

// split block into two if large enough
static void split(Block *blk, size_t size) {
    if (blk->size >= size + sizeof(Block) + ALIGN) {
        Block *new_blk = (Block*)((char*)blk + sizeof(Block) + size);
        new_blk->size = blk->size - size - sizeof(Block);
        new_blk->free = 1;
        new_blk->next = blk->next;
        blk->size = size;
        blk->next = new_blk;
    }
}

void *my_malloc(size_t size) {
    if (!free_list) my_init();
    if (size == 0) return NULL;

    size = align8(size);
    Block *blk = find_free(size);
    if (!blk) return NULL; // out of memory

    split(blk, size);
    blk->free = 0;
    return (char*)blk + sizeof(Block);
}

void my_free(void *ptr) {
    if (!ptr) return;
    Block *blk = (Block*)((char*)ptr - sizeof(Block));
    blk->free = 1;
}

void *my_calloc(size_t n, size_t s) {
    size_t total = n * s;
    void *p = my_malloc(total);
    if (p) memset(p, 0, total);
    return p;
}

void *my_realloc(void *ptr, size_t new_size) {
    if (!ptr) return my_malloc(new_size);
    Block *blk = (Block*)((char*)ptr - sizeof(Block));
    if (blk->size >= new_size) return ptr;

    void *newp = my_malloc(new_size);
    if (!newp) return NULL;
    memcpy(newp, ptr, blk->size);
    my_free(ptr);
    return newp;
}

// debugging
void my_dump() {
    Block *curr = free_list;
    printf("Heap dump:\n");
    while (curr) {
        printf(" Block %p | size=%lu | free=%d | next=%p\n", 
            (void*)curr, (unsigned long)curr->size, curr->free, (void*)curr->next);
        curr = curr->next;
    }
}
