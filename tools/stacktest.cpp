#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_stack_memory() {
    // This function prints the stack memory addresses and values
    // Note: This is just for demonstration; actual stack memory inspection
    // would require more complex handling.
    char str1[] = "Hello";
    char str2[] = "World";
    int int1 = 42;
    int int2 = 100;
    int int3 = 2023;

    printf("Stack Memory:\n");
    printf("String 1: %s at address %p\n", str1, (void*)&str1);
    printf("String 2: %s at address %p\n", str2, (void*)&str2);
    printf("Integer 1: %d at address %p\n", int1, (void*)&int1);
    printf("Integer 2: %d at address %p\n", int2, (void*)&int2);
    printf("Integer 3: %d at address %p\n", int3, (void*)&int3);
}

void print_heap_memory(int *heap_array, size_t size) {
    printf("Heap Memory:\n");
    for (size_t i = 0; i < size; i++) {
        printf("Integer %zu: %d at address %p\n", i, heap_array[i], (void*)&heap_array[i]);
    }
}

int main() {
    // Allocate memory on the heap for a few integers
    size_t heap_size = 5;
    int *heap_array = (int *)malloc(heap_size * sizeof(int));
    if (heap_array == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return 1;
    }

    // Initialize heap values
    for (size_t i = 0; i < heap_size; i++) {
        heap_array[i] = (i + 1) * 10;  // Values: 10, 20, 30, 40, 50
    }

    // Print stack memory
    print_stack_memory();

    // Print heap memory
    print_heap_memory(heap_array, heap_size);

    // Keep the application running
    printf("Press Enter to exit...\n");
    getchar();  // Wait for user input

    // Free the allocated heap memory
    free(heap_array);

    return 0;
}
