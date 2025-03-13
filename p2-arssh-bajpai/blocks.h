#ifndef BLOCKS_H
#define BLOCKS_H

#include <stddef.h>

#define BLOCK_SIZE 4096  /**< The size of each block in bytes. */
#define BLOCK_COUNT 256  /**< The total number of blocks available. */

/**
 * @brief Initializes the block layer for the file system.
 *
 * This function sets up internal data structures for managing blocks and may load block 
 * allocation information from the given disk image file if the file system state is 
 * persisted between runs.
 *
 * @param path The path to the disk image file that stores the block data.
 */
void blocks_init(const char *path);

/**
 * @brief Retrieves a pointer to the in-memory representation of the specified block.
 *
 * After initialization, the file system holds all blocks in memory. This function returns 
 * a pointer to the start of the requested block number, allowing read/write operations 
 * directly in memory.
 *
 * @param block_num The block number to retrieve (0-based index).
 * @return A pointer to the start of the requested block.
 */
void *blocks_get_block(int block_num);

/**
 * @brief Cleans up and frees any resources allocated by the block management system.
 *
 * This function is typically called during shutdown or unmount. It ensures that all 
 * necessary data is persisted to disk before freeing memory.
 */
void blocks_free();

/**
 * @brief Retrieves a pointer to the block allocation bitmap.
 *
 * The bitmap tracks which blocks are free and which are allocated. This function returns 
 * a pointer to that bitmap so the file system can query or update block availability.
 *
 * @return A pointer to the block allocation bitmap.
 */
void *get_blocks_bitmap();

/**
 * @brief Allocates a free block and returns its block number.
 *
 * This function searches the bitmap for a free block. If found, it marks the block as 
 * allocated and returns its block number. If no free blocks are available, it returns a 
 * negative error code.
 *
 * @return The allocated block number on success, or a negative value (e.g., -1) if none are free.
 */
int alloc_block();

#endif
