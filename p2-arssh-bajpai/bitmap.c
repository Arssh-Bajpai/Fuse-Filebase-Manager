#include "bitmap.h"
#include <stdint.h>
#include <stdio.h> // For printf

// Get the value of a specific bit in the bitmap
int bitmap_get(void *bm, int i) {
    uint8_t *base = (uint8_t *)bm;
    // Shift right to isolate the desired bit and mask with 1
    return (base[i / 8] >> (i % 8)) & 1;
}

// Set or clear a specific bit in the bitmap
void bitmap_put(void *bm, int i, int v) {
    uint8_t *base = (uint8_t *)bm;
    if (v) {
        // Set the bit
        base[i / 8] |= (1 << (i % 8));
    } else {
        // Clear the bit
        base[i / 8] &= ~(1 << (i % 8));
    }
}

// Find the first unused (0) bit in the bitmap
int bitmap_first_unused(void *bm, int size) {
    for (int i = 0; i < size; i++) {
        if (!bitmap_get(bm, i)) {
            return i; // Return index of first free bit
        }
    }
    return -1; // No free bits
}

// Print the bitmap bits for debugging
void bitmap_print(void *bm, int size) {
    for (int i = 0; i < size; i++) {
        if (i % 8 == 0) printf(" "); // Add spacing every 8 bits
        printf("%d", bitmap_get(bm, i));
    }
    printf("\n");
}
