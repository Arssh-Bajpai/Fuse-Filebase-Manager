#include "inode.h"
#include "blocks.h"
#include <string.h>
#include <errno.h>

static inode_t inodes[INODE_COUNT];
static int root_inum = 0;

// Initialize the inode table
void inode_init() {
    memset(inodes, 0, sizeof(inodes));
    inodes[root_inum].refs = 1;      // Root directory exists
    inodes[root_inum].mode = 040755; // Directory with default permissions
    inodes[root_inum].size = 0;
}

// Retrieve an inode by its index
inode_t *get_inode(int inum) {
    if (inum < 0 || inum >= INODE_COUNT) return NULL;
    return &inodes[inum];
}

// Allocate a new inode
int alloc_inode() {
    for (int i = 0; i < INODE_COUNT; i++) {
        if (inodes[i].refs == 0) {
            inodes[i].refs = 1;
            return i;
        }
    }
    return -ENOSPC; // No space left on device
}

// Free an existing inode
void free_inode(int inum) {
    if (inum < 0 || inum >= INODE_COUNT) return;
    memset(&inodes[inum], 0, sizeof(inode_t));
}

// Lookup a path in the file system and return its inode number
int tree_lookup(const char *path) {
    if (strcmp(path, "/") == 0) {
        return root_inum; // Return the root inode number
    }
    // Extend this to handle additional files or directories
    return -ENOENT; // Not found
}

// Grow an inode to the specified size
int grow_inode(inode_t *node, int size) {
    if (node->size >= size) {
        return 0; // No need to grow
    }

    int new_block = alloc_block();
    if (new_block < 0) {
        return -ENOSPC; // No space left on device
    }

    node->block = new_block; // Update inode to point to the new block
    node->size = size;       // Update the inode size
    return 0;
}
