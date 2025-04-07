#pragma once

#define ALLOCATION_FAILURE 10
#define REALLOCATION_FAILURE 11

void* safe_malloc(size_t size);

void* safe_realloc(void* memoryBlockPointer, size_t newSize);

char* safe_strdup(char* str);