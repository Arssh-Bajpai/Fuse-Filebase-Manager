#ifndef INODE_H
#define INODE_H

#include <sys/stat.h>

#define INODE_COUNT 128

/**
 * @brief Represents a file system inode, which contains metadata about a file or directory.
 *
 * Each inode holds information such as:
 * - Reference count (refs): How many directory entries or references point to this inode.
 * - File mode (mode): Permissions and file type bits (similar to st_mode in struct stat).
 * - File size (size): The size of the file in bytes.
 * - Block pointer (block): A single block pointer, which is sufficient for files up to 4KB in this simplistic design.
 */
typedef struct inode {
    int refs;   /**< Reference count (how many links to this inode exist) */
    int mode;   /**< File mode (includes permissions and type, e.g. S_IFREG, S_IFDIR) */
    int size;   /**< Size of the file in bytes */
    int block;  /**< Pointer (index) to a single data block that holds file contents */
} inode_t;

/**
 * @brief Retrieves a pointer to the inode structure corresponding to a given inode number.
 *
 * @param inum The inode number (index) to retrieve.
 * @return A pointer to the requested inode, or NULL if inum is invalid or out of range.
 */
inode_t *get_inode(int inum);

/**
 * @brief Allocates a new, free inode from the inode table.
 *
 * This function searches for an available inode in the inode array. If found, it initializes
 * that inode as needed and returns its index.
 *
 * @return The inode number of the allocated inode, or a negative value (e.g., -1) if no free inodes are available.
 */
int alloc_inode();

/**
 * @brief Frees an inode, making it available for reuse.
 *
 * This function resets the inode fields, effectively marking it as unused. After calling free_inode(),
 * the inode number can be reused by alloc_inode().
 *
 * @param inum The inode number to free.
 */
void free_inode(int inum);

/**
 * @brief Expands the file size associated with the given inode.
 *
 * If an inode needs to store more data than currently allocated, this function attempts to
 * grow the inode to accommodate the new size. It may involve allocating additional data blocks.
 *
 * @param node A pointer to the inode to grow.
 * @param size The desired new size of the file.
 * @return 0 on success, or a negative error code if there's not enough space or another error occurs.
 */
int grow_inode(inode_t *node, int size);

/**
 * @brief Shrinks the file size associated with the given inode.
 *
 * If a file is truncated or data is removed, this function adjusts the inode's size to the
 * specified smaller size. It may also free up no-longer-used blocks.
 *
 * @param node A pointer to the inode to shrink.
 * @param size The new desired size of the file, which must be less than or equal to the current size.
 * @return 0 on success, or a negative error code if an error occurs.
 */
int shrink_inode(inode_t *node, int size);

/**
 * @brief Retrieves the block number (on-disk block index) that corresponds to a given file block number.
 *
 * Files can be considered as a sequence of blocks. Given a zero-based block index within the file,
 * this function returns the actual block number on disk. This simple file system uses a single block pointer,
 * so `file_bnum` must be 0 for valid files.
 *
 * @param node A pointer to the inode.
 * @param file_bnum The file block number (starting from 0).
 * @return The disk block number if valid, or a negative value if the block doesn't exist.
 */
int inode_get_bnum(inode_t *node, int file_bnum);

/**
 * @brief Initializes the inode table and related data structures.
 *
 * This function is typically called during file system initialization. It sets up the global inode array,
 * marks all inodes as free or loads their state from a disk image if needed.
 */
void inode_init();

/**
 * @brief Performs a lookup in the directory tree to find the inode number associated with a given path.
 *
 * This function attempts to resolve a path (e.g., "/foo/bar") to its corresponding inode number by searching
 * the directory structure. If the path is found, the associated inode number is returned.
 *
 * @param path A null-terminated string representing the file system path.
 * @return The inode number if the path exists, or a negative value (e.g., -ENOENT) if it does not exist.
 */
int tree_lookup(const char *path);

#endif
