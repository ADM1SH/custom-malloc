/**
 * @file tests_adv.c
 * @brief A simple test suite for the advanced custom memory allocator.
 *
 * This file contains a main function that performs a basic set of allocation
 * and deallocation operations to test the functionality of the advanced
 * allocator.
 */

#include <stdio.h>
#include "mymalloc_adv.h"

/**
 * @brief The main entry point for the test suite.
 * @return 0 on successful execution.
 */
int main() {
    printf("\n=== Testing Advanced Allocator ===\n");

    // Allocate two blocks of memory.
    void *x = my_malloc(60);
    void *y = my_malloc(300);

    // Free the first block.
    my_free(x);

    // Allocate a third block.
    void *z = my_malloc(50);

    // Dump the final state of the heap to the console.
    my_dump();

    // Suppress unused variable warnings.
    (void)y;
    (void)z;

    return 0;
}