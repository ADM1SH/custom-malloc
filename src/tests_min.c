#include <stdio.h>
#include "mymalloc_min.h"

int main() {
    printf("=== Testing Minimal Allocator ===\n");
    my_init();
    void *a = my_malloc(100);
    void *b = my_malloc(200);
    my_free(a);
    void *c = my_malloc(50);
    my_dump();
    return 0;
}

