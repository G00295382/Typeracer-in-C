#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "safe_alloc.h"

// Wrapper for malloc() that exits the program if memory allocation fails
void* safe_malloc(size_t size) {
    void* memoryBlockPointer = malloc(size);
    if (memoryBlockPointer == NULL) {
        exit(ALLOCATION_FAILURE);
    }
    return memoryBlockPointer;
}

// Wrapper for realloc() that exits the program if memory reallocation fails
void* safe_realloc(void* memoryBlockPointer, size_t newSize) {
    void* resizedBlockPointer = realloc(memoryBlockPointer, newSize);
    if (resizedBlockPointer == NULL) {
        free(memoryBlockPointer);
        exit(REALLOCATION_FAILURE);
    }
}

// Wrapper for _strdup() that exits the program if memory allocation fails
char* safe_strdup(char* str) {
    char* newStr = _strdup(str);
    if (newStr == NULL) {
        exit(ALLOCATION_FAILURE);
    }
    return newStr;
}