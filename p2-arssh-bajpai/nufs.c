#define FUSE_USE_VERSION 29
#include <fuse.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

#include "storage.h"   // Contains functions for interacting with the "disk" and filesystem data structures
#include "slist.h"     // Linked list structure used for directory listings

// The nufs_access function checks if the given path can be accessed with the specified mask (e.g., read/write/execute).
// It calls storage_stat() to see if the file exists and returns 0 on success or an error code on failure.
int nufs_access(const char *path, int mask) {
    printf("[DEBUG] nufs_access: path=%s, mask=%04o\n", path, mask);
    struct stat st;
    int rv = storage_stat(path, &st);
    printf("[INFO] access(%s, %04o) -> %d\n", path, mask, rv);
    return rv < 0 ? rv : 0;
}

// The nufs_getattr function retrieves file attributes (e.g., size, mode, timestamps) for the given path.
// It uses storage_stat() to populate a stat structure.
int nufs_getattr(const char *path, struct stat *st) {
    printf("[DEBUG] nufs_getattr: path=%s\n", path);
    int rv = storage_stat(path, st);
    printf("[INFO] getattr(%s) -> %d\n", path, rv);
    return rv;
}

// The nufs_readdir function is called by FUSE when a directory is listed (e.g., via `ls`).
// It retrieves the directory contents using storage_list() and then uses filler() to pass these entries back to the caller.
int nufs_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi) {
    printf("[DEBUG] nufs_readdir: path=%s\n", path);

    // Get a list of entries in the directory specified by 'path'.
    slist_t *entries = storage_list(path);
    if (!entries) {
        // If we can't find the directory, return an error.
        printf("[ERROR] Directory not found: %s\n", path);
        return -ENOENT;
    }

    // We set up a stat structure for the directory entries we're going to list.
    struct stat st;
    memset(&st, 0, sizeof(struct stat));
    st.st_mode = S_IFDIR | 0755; // Indicates a directory with certain permissions.
    st.st_nlink = 2;             // Typical directory link count.

    // Add the current directory and parent directory entries: "." and ".."
    filler(buf, ".", &st, 0);
    filler(buf, "..", &st, 0);

    // Now we iterate over the linked list of directory entries returned by storage_list()
    slist_t *curr = entries;
    while (curr) {
        memset(&st, 0, sizeof(struct stat));
        // For each entry, we retrieve its stat info and pass it along to FUSE.
        storage_stat(curr->data, &st);
        filler(buf, curr->data, &st, 0);
        curr = curr->next;
    }

    // Free the memory for the directory list now that we're done.
    s_free(entries);
    printf("[INFO] Directory contents listed for: %s\n", path);
    return 0;
}

// The nufs_mknod function creates a new file with the specified mode.
// It calls storage_mknod() which handles inode allocation and updates the file system structures.
int nufs_mknod(const char *path, mode_t mode, dev_t rdev) {
    printf("[DEBUG] nufs_mknod: path=%s, mode=%04o\n", path, mode);
    int rv = storage_mknod(path, mode);
    printf("[INFO] mknod(%s, %04o) -> %d\n", path, mode, rv);
    return rv;
}

// The nufs_unlink function removes a file from the file system.
// It calls storage_unlink() to update directory entries and free the associated inode.
int nufs_unlink(const char *path) {
    printf("[DEBUG] nufs_unlink: path=%s\n", path);
    int rv = storage_unlink(path);
    printf("[INFO] unlink(%s) -> %d\n", path, rv);
    return rv;
}

// The nufs_read function is called whenever a file is read (e.g., `cat` or `less`).
// It reads 'size' bytes from 'path' starting at 'offset' into the buffer 'buf', using storage_read().
int nufs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("[DEBUG] nufs_read: path=%s, size=%zu, offset=%lld\n", path, size, (long long)offset);
    int rv = storage_read(path, buf, size, offset);
    printf("[INFO] read(%s, %zu bytes, @%lld) -> %d\n", path, size, (long long)offset, rv);
    return rv;
}

// The nufs_write function handles writing data to a file.
// It writes 'size' bytes from 'buf' into the file at 'path' starting at 'offset', via storage_write().
int nufs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    printf("[DEBUG] nufs_write: path=%s, size=%zu, offset=%lld\n", path, size, (long long)offset);
    int rv = storage_write(path, buf, size, offset);
    printf("[INFO] write(%s, %zu bytes, @%lld) -> %d\n", path, size, (long long)offset, rv);
    return rv;
}

// The nufs_mkdir function creates a new directory at the specified 'path' with 'mode' permissions.
int nufs_mkdir(const char *path, mode_t mode) {
    printf("[DEBUG] nufs_mkdir: path=%s, mode=%04o\n", path, mode);
    int rv = storage_mkdir(path, mode);
    printf("[INFO] mkdir(%s) -> %d\n", path, rv);
    return rv;
}

// The nufs_rmdir function removes a directory from the file system.
// It ensures the directory is empty before removing it.
int nufs_rmdir(const char *path) {
    printf("[DEBUG] nufs_rmdir: path=%s\n", path);
    int rv = storage_rmdir(path);
    printf("[INFO] rmdir(%s) -> %d\n", path, rv);
    return rv;
}

// The nufs_destroy function is called when the file system is unmounted.
// It provides a chance to flush data and perform cleanup operations.
// Here, we call storage_shutdown() to write any pending changes and close the disk image.
static void nufs_destroy(void *private_data) {
    printf("[INFO] Unmounting file system and flushing data...\n");
    storage_shutdown();
    printf("[INFO] File system unmounted successfully.\n");
}

// The nufs_init_ops function initializes the fuse_operations structure (nufs_ops) by
// assigning our callback functions to the appropriate FUSE operations.
void nufs_init_ops(struct fuse_operations *ops) {
    memset(ops, 0, sizeof(struct fuse_operations));
    ops->access = nufs_access;
    ops->getattr = nufs_getattr;
    ops->readdir = nufs_readdir;
    ops->mknod = nufs_mknod;
    ops->unlink = nufs_unlink;
    ops->read = nufs_read;
    ops->write = nufs_write;
    ops->mkdir = nufs_mkdir;
    ops->rmdir = nufs_rmdir;
    ops->destroy = nufs_destroy;
}

// This global structure holds all the operations for FUSE to call.
struct fuse_operations nufs_ops;

int main(int argc, char *argv[]) {
    // We expect at least 3 arguments: the executable name, the mount point, and the disk image.
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <mount-point> <disk-image> [options]\n", argv[0]);
        return 1;
    }

    const char *mount_point = argv[1];
    const char *disk_image = argv[2];

    // Print some basic info about what we're doing.
    printf("[INFO] Initializing file system with disk image: %s\n", disk_image);
    printf("[INFO] Mount point: %s\n", mount_point);
    for (int i = 0; i < argc; i++) {
        printf("[DEBUG] Arg[%d]: %s\n", i, argv[i]);
    }

    // Initialize the storage layer with the given disk image.
    // This sets up in-memory structures, reads metadata, etc.
    storage_init(disk_image);

    // Initialize the FUSE operation callbacks.
    // Without this call, the nufs_ops structure would remain uninitialized and FUSE would not know which callbacks to use.
    nufs_init_ops(&nufs_ops);

    // We'll run the filesystem by passing the mount point to FUSE.
    // Here, we remove the '-f' flag so that the filesystem runs in the background,
    // which is often the expected behavior for tests and normal usage.
    char *fuse_argv[] = {argv[0], (char *)mount_point};
    int fuse_argc = 2;

    printf("[INFO] Mounting file system at: %s\n", mount_point);
    // fuse_main() will run the FUSE event loop, handling filesystem operations until it is unmounted.
    return fuse_main(fuse_argc, fuse_argv, &nufs_ops, NULL);
}
