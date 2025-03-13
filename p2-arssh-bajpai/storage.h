#ifndef STORAGE_H
#define STORAGE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "slist.h"

/**
 * @brief Initializes the storage system using the specified disk image.
 *
 * This function sets up internal data structures and loads the file system metadata 
 * from the given disk image file. If the disk image does not exist, it may create one.
 *
 * @param path The path to the disk image file.
 */
void storage_init(const char *path);

/**
 * @brief Retrieves file or directory metadata for the given path.
 *
 * @param path The file or directory path within the file system.
 * @param st   A pointer to a stat structure where metadata will be stored.
 * @return 0 on success, or a negative value (e.g., -ENOENT) if the path does not exist.
 */
int storage_stat(const char *path, struct stat *st);

/**
 * @brief Reads data from a file at the given path.
 *
 * @param path   The file path.
 * @param buf    A buffer to store the read data.
 * @param size   The maximum number of bytes to read.
 * @param offset The position in the file from which to start reading.
 * @return The number of bytes actually read, or a negative error code on failure.
 */
int storage_read(const char *path, char *buf, size_t size, off_t offset);

/**
 * @brief Writes data to a file at the given path.
 *
 * This function may extend the file size if writing beyond the current end-of-file.
 *
 * @param path   The file path.
 * @param buf    A buffer containing the data to be written.
 * @param size   The number of bytes to write.
 * @param offset The position in the file at which to start writing.
 * @return The number of bytes written, or a negative error code on failure.
 */
int storage_write(const char *path, const char *buf, size_t size, off_t offset);

/**
 * @brief Creates a new file at the specified path with the given mode.
 *
 * @param path The file path.
 * @param mode The mode (permissions) for the new file.
 * @return 0 on success, or a negative error code on failure (e.g., -EEXIST if already exists).
 */
int storage_mknod(const char *path, int mode);

/**
 * @brief Removes (unlinks) a file from the file system.
 *
 * @param path The path of the file to remove.
 * @return 0 on success, or a negative error code on failure (e.g., -ENOENT if not found).
 */
int storage_unlink(const char *path);

/**
 * @brief Creates a new directory at the specified path with the given mode.
 *
 * @param path The directory path.
 * @param mode The mode (permissions) for the new directory.
 * @return 0 on success, or a negative error code on failure.
 */
int storage_mkdir(const char *path, mode_t mode);

/**
 * @brief Removes a directory from the file system.
 *
 * The directory must be empty before removal is possible.
 *
 * @param path The directory path.
 * @return 0 on success, or a negative error code on failure (e.g., -ENOTEMPTY if not empty).
 */
int storage_rmdir(const char *path);

/**
 * @brief Retrieves a list of entries (files and subdirectories) within a directory.
 *
 * @param path The directory path.
 * @return A linked list of directory entries (slist_t). Returns NULL if the directory does not exist.
 *         Caller is responsible for freeing this list.
 */
slist_t *storage_list(const char *path);

/**
 * @brief Flushes data to the disk image and shuts down the storage system.
 *
 * This function ensures that all pending changes are written to the disk image, and 
 * closes any open file descriptors. It should be called during unmount or program exit.
 */
void storage_shutdown();

#endif
