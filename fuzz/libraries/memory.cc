/*
 * Implement a simple "arena" for dynamic memory management without leaks, and without GC:
 *   `allocate_arena` allocate a big chunk of memory that will be the `arena`;
 *   Every allocation using `malloc` or `calloc` will happen inside the `arena`;
 *   `free_arena` frees the whole `arena`.
 *
 * Note that `malloc` and `calloc` are weak symbols, so our overrides are the ones called at runtime.
 */

#include <stdlib.h>
#include <string.h>

#include <iostream>


extern "C" void * __libc_free(void * ptr);
extern "C" void * __libc_malloc(size_t);

char * arena;
unsigned int current = 0;

void *malloc(size_t size) {
    char * destination = arena + current;
    current += size;

    return destination;
}

void *calloc(size_t nmemb, size_t size) {
    char * destination = arena + current;
    size_t overall_size = size * nmemb;

    current += overall_size;

    memset(destination, 0, overall_size);

    return destination;
}

void allocate_arena() {
    arena = (char *) __libc_malloc(8192);
}

void free_arena() {
    __libc_free(arena);
}
