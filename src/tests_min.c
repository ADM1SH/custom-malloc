/**
 * @file tests_min.c
 * @brief A simple test suite for the minimal custom memory allocator.
 *
 * This file contains a main function that performs a basic set of allocation
 * and deallocation operations to test the functionality of the minimal
 * allocator.
 */

#include <stdio.h>
#include "mymalloc_min.h"

/**
 * @brief The main entry point for the test suite.
 * @return 0 on successful execution.
 */
int main() {
    printf("=== Testing Minimal Allocator ===\n");

    // Initialize the allocator.
    my_init();

    // Allocate three blocks of memory.
    void *a = my_malloc(100);
    void *b = my_malloc(200);

    // Free the first block.
    my_free(a);

    // Allocate a third block.
    void *c = my_malloc(50);

    // Dump the final state of the heap to the console.
    my_dump();

    // Suppress unused variable warnings.
    (void)b;
    (void)c;

    return 0;
}