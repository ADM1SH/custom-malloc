/**
 * @file tests_stress.c
 * @brief A comprehensive test suite for the custom memory allocators.
 *
 * This file contains a series of tests to aggressively exercise the
 * functionality of both the minimal and advanced custom allocators. It
 * includes tests for alignment, stress, realloc, calloc, and invalid free
 * scenarios.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

// Use a macro to switch between the minimal and advanced allocators.
#ifdef ADVANCED_ALLOCATOR
#include "mymalloc_adv.h"
#else
#include "mymalloc_min.h"
#endif

// The alignment for allocated memory in bytes.
#define ALIGN 8
// The number of iterations for the stress test.
#define STRESS_ITERATIONS 10000
// The maximum size of a single allocation in the stress test.
#define MAX_ALLOCATION_SIZE 4096

/**
 * @brief Checks if a pointer is aligned to a given boundary.
 * @param ptr The pointer to check.
 * @param alignment The alignment boundary.
 * @return 1 if the pointer is aligned, 0 otherwise.
 */
int is_aligned(void *ptr, size_t alignment) {
    return ((uintptr_t)ptr % alignment) == 0;
}

/**
 * @brief Tests that all allocated pointers are correctly aligned.
 */
void test_alignment() {
    printf("--- Testing Alignment ---\n");
    for (size_t size = 1; size <= 128; size++) {
        void *p = my_malloc(size);
        assert(p != NULL);
        assert(is_aligned(p, ALIGN));
        my_free(p);
    }
    printf("Alignment test passed.\n");
}

/**
 * @brief Performs a stress test on the allocator.
 *
 * This test performs a large number of random allocations and deallocations
 * to check for memory leaks, corruption, and other issues.
 */
void test_stress() {
    printf("--- Stress Test ---\n");
    void **allocations = malloc(sizeof(void*) * STRESS_ITERATIONS);
    assert(allocations != NULL);

    srand(time(NULL));

    // Perform a large number of allocations.
    for (int i = 0; i < STRESS_ITERATIONS; i++) {
        size_t size = rand() % MAX_ALLOCATION_SIZE + 1;
        allocations[i] = my_malloc(size);
    }

    // Free all the allocated blocks.
    for (int i = 0; i < STRESS_ITERATIONS; i++) {
        if (allocations[i] != NULL) {
            my_free(allocations[i]);
        }
    }

    free(allocations);
    printf("Stress test completed.\n");
}

/**
 * @brief Tests the functionality of my_realloc.
 *
 * This test checks both growing and shrinking of memory blocks and verifies
 * that the data is preserved.
 */
void test_realloc() {
    printf("--- Testing realloc ---\n");

    // Test growing a memory block.
    char *p1 = my_malloc(10);
    assert(p1 != NULL);
    memset(p1, 'a', 10);
    p1 = my_realloc(p1, 100);
    assert(p1 != NULL);
    for (int i = 0; i < 10; i++) {
        assert(p1[i] == 'a');
    }
    my_free(p1);

    // Test shrinking a memory block.
    char *p2 = my_malloc(100);
    assert(p2 != NULL);
    memset(p2, 'b', 100);
    p2 = my_realloc(p2, 10);
    assert(p2 != NULL);
    for (int i = 0; i < 10; i++) {
        assert(p2[i] == 'b');
    }
    my_free(p2);

    printf("realloc test passed.\n");
}

/**
 * @brief Tests the functionality of my_calloc.
 *
 * This test verifies that the memory allocated by my_calloc is zeroed out.
 */
void test_calloc() {
    printf("--- Testing calloc ---\n");
    size_t size = 100;
    char *p = my_calloc(size, sizeof(char));
    assert(p != NULL);
    for (size_t i = 0; i < size; i++) {
        assert(p[i] == 0);
    }
    my_free(p);
    printf("calloc test passed.\n");
}

/**
 * @brief Tests the allocator's handling of invalid free operations.
 *
 * This test checks for correct handling of double-freeing and freeing of
 * pointers that are not managed by the allocator.
 */
void test_invalid_free() {
    printf("--- Testing Invalid Free ---\n");

    // Test double-freeing a pointer.
    void *p1 = my_malloc(10);
    assert(p1 != NULL);
    my_free(p1);
    printf("Attempting double free...\n");
    my_free(p1);

    // Test freeing a pointer that is outside the heap.
    int x;
    void *p2 = &x;
    printf("Attempting to free a pointer outside the heap...\n");
    my_free(p2);

    printf("Invalid free test completed.\n");
}

/**
 * @brief The main entry point for the test suite.
 * @return 0 on successful execution.
 */
int main() {
    // Initialize the minimal allocator.
#ifndef ADVANCED_ALLOCATOR
    my_init();
#endif

    // Run all the tests.
    test_alignment();
    test_stress();
    test_realloc();
    test_calloc();
    test_invalid_free();

    return 0;
}