#include <stdio.h>
#include "mymalloc_adv.h"

int main() {
    printf("\n=== Testing Advanced Allocator ===\n");
    void *x = my_malloc(60);
    void *y = my_malloc(300);
    my_free(x);
    void *z = my_malloc(50);
    my_dump();
    return 0;
}

