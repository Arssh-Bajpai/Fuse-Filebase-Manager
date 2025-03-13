#include "blocks.h"
#include "bitmap.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Global pointers to in-memory block data and bitmap
static void *block_data = NULL;
static void *block_bitmap = NULL;

// Set up in-memory storage for all blocks and the block bitmap
void blocks_init(const char *path) {
    (void)path; // The path is not used in this simple implementation

    // Allocate memory for all blocks and the bitmap
    block_data = malloc(BLOCK_COUNT * BLOCK_SIZE);
    block_bitmap = malloc(BLOCK_COUNT / 8); // One bit per block

    if (!block_data || !block_bitmap) {
        perror("Failed to allocate block data or bitmap");
        exit(1);
    }

    // Initialize blocks and bitmap to zero
    memset(block_data, 0, BLOCK_COUNT * BLOCK_SIZE);
    memset(block_bitmap, 0, BLOCK_COUNT / 8);
}

// Return a pointer to the start of the given block number
void *blocks_get_block(int block_num) {
    if (block_num < 0 || block_num >= BLOCK_COUNT) {
        return NULL; // Invalid block number
    }
    return (char *)block_data + block_num * BLOCK_SIZE;
}

// Free the in-memory blocks and bitmap
void blocks_free() {
    free(block_data);
    free(block_bitmap);
}

// Get the block allocation bitmap
void *get_blocks_bitmap() {
    return block_bitmap;
}

// Allocate a free block by finding the first unused bit in the bitmap
int alloc_block() {
    int block_num = bitmap_first_unused(block_bitmap, BLOCK_COUNT);
    if (block_num < 0) {
        return -1; // No free blocks available
    }
    bitmap_put(block_bitmap, block_num, 1); // Mark block as allocated
    return block_num;
}
