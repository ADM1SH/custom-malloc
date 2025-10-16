#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HEAP_SIZE (1024*1024)
#define ALIGN 8
#define NUM_BINS 6

static unsigned char heap[HEAP_SIZE];
static int heap_initialized = 0;

typedef struct Block {
    size_t size;                 // payload size
    int free;
    struct Block *prev_phys;     // previous physical block
    struct Block *next_phys;     // next physical block
    struct Block *prev_free;     // previous in free list
    struct Block *next_free;     // next in free list
} Block;

static Block *bin[NUM_BINS];     // segregated free lists
static Block *heap_start = NULL;

// utility
static size_t align8(size_t n) { return (n + 7) & ~7; }
static void write_footer(Block *b) {
    *((size_t*)((char*)b + sizeof(Block) + b->size)) = b->size;
}
static size_t read_footer(Block *b) {
    return *((size_t*)((char*)b + sizeof(Block) + b->size));
}

// size to bin index
static int size_to_bin(size_t size) {
    if (size <= 64) return 0;
    if (size <= 128) return 1;
    if (size <= 256) return 2;
    if (size <= 512) return 3;
    if (size <= 1024) return 4;
    return 5;
}

// insert into free list
static void insert_free(Block *b) {
    int idx = size_to_bin(b->size);
    b->prev_free = NULL;
    b->next_free = bin[idx];
    if (bin[idx]) bin[idx]->prev_free = b;
    bin[idx] = b;
}

// remove from free list
static void remove_free(Block *b) {
    int idx = size_to_bin(b->size);
    if (b->prev_free) b->prev_free->next_free = b->next_free;
    else bin[idx] = b->next_free;
    if (b->next_free) b->next_free->prev_free = b->prev_free;
    b->prev_free = b->next_free = NULL;
}

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

// find block in appropriate bin or larger bins
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

static void split_block(Block *b, size_t size) {
    size_t remaining = b->size - size;
    if (remaining < sizeof(Block) + sizeof(size_t) + ALIGN) return;
    Block *newb = (Block*)((char*)b + sizeof(Block) + size + sizeof(size_t));
    newb->size = remaining - sizeof(Block) - sizeof(size_t);
    newb->free = 1;
    newb->prev_phys = b;
    newb->next_phys = b->next_phys;
    if (b->next_phys) b->next_phys->prev_phys = newb;
    b->next_phys = newb;
    b->size = size;
    write_footer(b);
    write_footer(newb);
    insert_free(newb);
}

void *my_malloc(size_t size) {
    if (!heap_initialized) heap_init();
    size = align8(size);
    Block *b = find_fit(size);
    if (!b) return NULL;
    remove_free(b);
    split_block(b, size);
    b->free = 0;
    write_footer(b);
    return (char*)b + sizeof(Block);
}

static void coalesce(Block *b) {
    // merge next
    if (b->next_phys && b->next_phys->free) {
        remove_free(b->next_phys);
        b->size += sizeof(Block) + sizeof(size_t) + b->next_phys->size;
        b->next_phys = b->next_phys->next_phys;
        if (b->next_phys) b->next_phys->prev_phys = b;
        write_footer(b);
    }
    // merge prev
    if (b->prev_phys && b->prev_phys->free) {
        remove_free(b->prev_phys);
        b->prev_phys->size += sizeof(Block) + sizeof(size_t) + b->size;
        b->prev_phys->next_phys = b->next_phys;
        if (b->next_phys) b->next_phys->prev_phys = b->prev_phys;
        write_footer(b->prev_phys);
        b = b->prev_phys;
    }
    insert_free(b);
}

void my_free(void *ptr) {
    if (!ptr) return;
    Block *b = (Block*)((char*)ptr - sizeof(Block));
    b->free = 1;
    coalesce(b);
}

void *my_calloc(size_t n, size_t s) {
    size_t total = n * s;
    void *p = my_malloc(total);
    if (p) memset(p, 0, total);
    return p;
}

void *my_realloc(void *ptr, size_t size) {
    if (!ptr) return my_malloc(size);
    Block *b = (Block*)((char*)ptr - sizeof(Block));
    if (b->size >= size) return ptr;
    void *newp = my_malloc(size);
    if (!newp) return NULL;
    memcpy(newp, ptr, b->size);
    my_free(ptr);
    return newp;
}

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
