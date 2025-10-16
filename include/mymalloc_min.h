#ifndef MYMALLOC_MIN_H
#define MYMALLOC_MIN_H
#include <stddef.h>

void my_init(void);
void *my_malloc(size_t size);
void my_free(void *ptr);
void *my_calloc(size_t n, size_t s);
void *my_realloc(void *ptr, size_t size);
void my_dump(void);

#endif
