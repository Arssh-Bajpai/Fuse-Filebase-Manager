#include "storage.h"
#include "slist.h"
#include "inode.h"
#include "blocks.h"
#include "directory.h"
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>   // For perror and printf
#include <stdlib.h>  // For exit

// Global file descriptor for the disk image file.
// This represents the "backend" storage the filesystem uses.
static int fd = -1; 

// Initialize the storage system with the provided disk image path.
// This function:
// 1. Opens (or creates) the disk image file.
// 2. Reads the entire file system data (all blocks) into memory.
// 3. Calls blocks_init() and inode_init() to set up in-memory structures
//    (e.g., free block lists, inode tables).
void storage_init(const char *path) {
    printf("[INFO] Initializing storage system with file: %s\n", path);

    // Open the disk image file with read/write access.
    // O_CREAT ensures the file is created if it does not exist.
    // 0666 sets the file's default permissions (modified by umask).
    fd = open(path, O_RDWR | O_CREAT, 0666);
    if (fd < 0) {
        perror("[ERROR] Failed to open data file");
        exit(1); // If we cannot open the storage file, we must exit.
    }

    // block_zero points to the start of our in-memory blocks array.
    // We read the entire file system (all blocks) into memory.
    void *block_zero = blocks_get_block(0);
    if (pread(fd, block_zero, BLOCK_COUNT * BLOCK_SIZE, 0) < 0) {
        perror("[ERROR] Failed to read data from file");
        exit(1);
    }

    // Initialize our block and inode management layers.
    blocks_init(path);
    inode_init();

    printf("[INFO] Storage initialized from %s.\n", path);
}

// Retrieve file metadata (stat information) for a given path.
// This uses tree_lookup() to find the inode number, then get_inode()
// to retrieve inode data. If the file or directory doesn't exist,
// we return -ENOENT. Otherwise, we fill the stat structure with
// the file's mode, size, and user ID.
int storage_stat(const char *path, struct stat *st) {
    printf("[DEBUG] storage_stat: path=%s\n", path);

    int inum = tree_lookup(path);
    if (inum < 0) {
        printf("[ERROR] File not found: %s\n", path);
        return -ENOENT;
    }

    inode_t *node = get_inode(inum);
    if (!node) {
        printf("[ERROR] Failed to retrieve inode for path: %s\n", path);
        return -EIO;
    }

    memset(st, 0, sizeof(struct stat));
    st->st_uid = getuid();    // Set the user ID to the current user.
    st->st_mode = node->mode; // File mode (permissions, directory/file)
    st->st_size = node->size; // File size in bytes

    printf("[INFO] Retrieved metadata for %s: size=%d, mode=%o\n", path, node->size, node->mode);
    return 0;
}

// Read data from the file at the given path into 'buf'.
// Starts at 'offset' and reads up to 'size' bytes, or until the end of the file.
// If the path does not exist, returns -ENOENT.
// Adjusts 'size' to ensure we do not read beyond the file's size.
// Copies data from our in-memory block into the user buffer.
int storage_read(const char *path, char *buf, size_t size, off_t offset) {
    printf("[DEBUG] storage_read: path=%s, size=%zu, offset=%lld\n", path, size, (long long)offset);

    int inum = tree_lookup(path);
    if (inum < 0) {
        printf("[ERROR] File not found: %s\n", path);
        return -ENOENT;
    }

    inode_t *node = get_inode(inum);
    if (offset >= node->size) {
        // If the offset is beyond the file size, no data can be read.
        return 0;
    }

    // Adjust the size to read only what's available up to the end of the file.
    if (offset + size > node->size) {
        size = node->size - offset;
    }

    // Retrieve the block associated with this inode.
    void *block = blocks_get_block(node->block);

    // Copy data from the filesystem block into buf.
    memcpy(buf, (char*)block + offset, size);
    printf("[INFO] Read %zu bytes from file: %s\n", size, path);
    return size;
}

// Write 'size' bytes of data from 'buf' into the file at 'path', starting at 'offset'.
// If writing beyond the current file size, we try to grow the inode (if possible).
// Once done, we flush the changes to disk using pwrite() so that the file system is persistent.
int storage_write(const char *path, const char *buf, size_t size, off_t offset) {
    printf("[DEBUG] storage_write: path=%s, size=%zu, offset=%lld\n", path, size, (long long)offset);

    int inum = tree_lookup(path);
    if (inum < 0) {
        // If the file doesn't exist, we cannot write to it.
        printf("[ERROR] File not found: %s\n", path);
        return -ENOENT;
    }

    inode_t *node = get_inode(inum);
    if (!node) {
        printf("[ERROR] Failed to retrieve inode for path: %s\n", path);
        return -EIO;
    }

    // If we are writing beyond the current file size, we need to grow the file.
    if (offset + size > node->size) {
        printf("[INFO] Growing inode: current size=%d, new size=%llu\n", node->size, (unsigned long long)(offset + size));
        if (grow_inode(node, offset + size) < 0) {
            printf("[ERROR] Not enough space to grow inode for path: %s\n", path);
            return -ENOSPC; // Not enough space on the "disk".
        }
    }

    // Retrieve the block for this inode and check that it exists.
    void *block = blocks_get_block(node->block);
    if (!block) {
        printf("[ERROR] Failed to retrieve block for inode: %d\n", inum);
        return -EIO;
    }

    // Write data into the in-memory block at the given offset.
    memcpy((char*)block + offset, buf, size);
    printf("[DEBUG] Data written to memory block %d: %.*s\n", node->block, (int)size, buf);

    // Write changes to disk using pwrite to ensure persistence.
    off_t disk_offset = node->block * BLOCK_SIZE + offset;
    ssize_t written = pwrite(fd, buf, size, disk_offset);
    if (written < 0) {
        perror("[ERROR] Failed to write data to disk");
        return -EIO;
    }

    printf("[INFO] Persisted %zd bytes to disk at offset %lld\n", written, (long long)disk_offset);

    // Update the file size to reflect new writes.
    node->size = offset + size;
    printf("[INFO] Updated file size for %s: %d bytes\n", path, node->size);

    return size;
}

// Create a new file at 'path' with the given 'mode'.
// If the file already exists, return -EEXIST.
// This involves allocating an inode and adding an entry in the root directory.
int storage_mknod(const char *path, int mode) {
    printf("[DEBUG] storage_mknod: path=%s, mode=%o\n", path, mode);

    // Check if file already exists.
    if (tree_lookup(path) >= 0) return -EEXIST;

    // Allocate a new inode for the file.
    int inum = alloc_inode();
    if (inum < 0) return -ENOSPC;

    inode_t *node = get_inode(inum);
    memset(node, 0, sizeof(inode_t));
    node->refs = 1;
    node->mode = mode;
    node->size = 0;

    // Insert the file into the root directory ("/").
    return directory_put((directory_t *)get_inode(tree_lookup("/")), path, inum);
}

// Delete (unlink) a file at 'path'.
// This removes the directory entry and frees the inode.
int storage_unlink(const char *path) {
    printf("[DEBUG] storage_unlink: path=%s\n", path);

    int inum = tree_lookup(path);
    if (inum < 0) return -ENOENT;

    free_inode(inum);
    return directory_delete((directory_t *)get_inode(tree_lookup("/")), path);
}

// Create a directory at 'path' with the given 'mode'.
// Directories are also represented by inodes. This function allocates an inode,
// marks it as a directory, and adds it to the parent directory ("/").
int storage_mkdir(const char *path, mode_t mode) {
    printf("[DEBUG] storage_mkdir: path=%s, mode=%o\n", path, mode);

    if (tree_lookup(path) >= 0) return -EEXIST;

    int inum = alloc_inode();
    if (inum < 0) return -ENOSPC;

    inode_t *node = get_inode(inum);
    memset(node, 0, sizeof(inode_t));
    node->refs = 1;
    node->mode = mode | S_IFDIR;
    node->size = 0;

    int parent_inum = tree_lookup("/");
    if (parent_inum < 0) return -ENOENT;

    return directory_put((directory_t *)get_inode(parent_inum), path, inum);
}

// Remove a directory at 'path'.
// The directory must be empty before removal. If it's not empty, return -ENOTEMPTY.
// If it's not a directory, return -ENOTDIR.
int storage_rmdir(const char *path) {
    printf("[DEBUG] storage_rmdir: path=%s\n", path);

    int inum = tree_lookup(path);
    if (inum < 0) return -ENOENT;

    inode_t *node = get_inode(inum);
    if (!S_ISDIR(node->mode)) return -ENOTDIR;

    // Check if directory is empty by listing its contents.
    slist_t *entries = directory_list((directory_t *)node);
    if (entries) {
        // If we have any entries, the directory is not empty.
        s_free(entries);
        return -ENOTEMPTY;
    }

    int parent_inum = tree_lookup("/");
    if (parent_inum < 0) return -ENOENT;

    // Remove the directory entry from the parent and free the inode.
    directory_delete((directory_t *)get_inode(parent_inum), path);
    free_inode(inum);
    return 0;
}

// Return a linked list of entries (filenames) within the directory specified by 'path'.
// If 'path' is not a directory or doesn't exist, return NULL.
slist_t *storage_list(const char *path) {
    printf("[DEBUG] storage_list: path=%s\n", path);

    int inum = tree_lookup(path);
    if (inum < 0) return NULL;

    inode_t *node = get_inode(inum);
    if (!S_ISDIR(node->mode)) return NULL;

    // Use directory_list() to get a list of the entries in this directory.
    return directory_list((directory_t *)node);
}

// Shut down the storage system:
// 1. Writes all in-memory data back to the disk image file using pwrite().
// 2. Closes the disk image file descriptor.
// This function is typically called from the FUSE 'destroy' callback when the file system is unmounted.
void storage_shutdown() {
    printf("[DEBUG] storage_shutdown: Flushing data to disk\n");

    if (fd >= 0) {
        // Write all blocks back to disk so that changes are persistent.
        if (pwrite(fd, blocks_get_block(0), BLOCK_COUNT * BLOCK_SIZE, 0) < 0) {
            perror("[ERROR] Failed to write data to disk");
        }
        close(fd);
        fd = -1;
        printf("[INFO] Storage successfully flushed and closed.\n");
    }
}
