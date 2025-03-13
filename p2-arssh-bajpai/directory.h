#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "slist.h"

#define MAX_ENTRIES 32    /**< The maximum number of directory entries allowed in a single directory. */
#define MAX_NAME_LEN 27   /**< The maximum length of a directory entry name (excluding null terminator). */

/**
 * @brief Represents a single entry within a directory.
 *
 * Each directory entry contains:
 * - A file or directory name (up to MAX_NAME_LEN characters, not including a null terminator).
 * - An inode number (inum) that points to the file or directory’s inode.
 */
typedef struct directory_entry {
    char name[MAX_NAME_LEN]; /**< The name of the file or directory. */
    int inum;                /**< The inode number associated with this entry. */
} directory_entry_t;

/**
 * @brief Represents a directory structure that holds multiple directory entries.
 *
 * A directory is essentially a special type of file that maps names to inode numbers.
 * In this simplistic file system, a directory can hold up to MAX_ENTRIES entries.
 */
typedef struct directory {
    int entry_count;                           /**< The current number of valid entries in this directory. */
    directory_entry_t entries[MAX_ENTRIES];    /**< An array of directory entries. */
} directory_t;

/**
 * @brief Initializes a directory structure.
 *
 * This function sets up a directory to be empty, with zero entries. It should be called on 
 * a newly allocated directory or when re-initializing a directory data structure.
 *
 * @param dir A pointer to the directory to initialize.
 */
void directory_init(directory_t *dir);

/**
 * @brief Adds a new entry (name -> inum mapping) to the directory.
 *
 * If the directory is not full, this function will create a new directory entry with the given name
 * and inode number, incrementing the entry count.
 *
 * @param dir  A pointer to the directory.
 * @param name The name of the new file or directory entry (null-terminated).
 * @param inum The inode number associated with the entry.
 * @return 0 on success, or a negative value if the directory is full or the entry already exists.
 */
int directory_put(directory_t *dir, const char *name, int inum);

/**
 * @brief Removes an entry from the directory by name.
 *
 * This function searches the directory for an entry matching the specified name.
 * If found, it removes the entry and decrements the entry count.
 *
 * @param dir  A pointer to the directory.
 * @param name The name of the entry to remove (null-terminated).
 * @return 0 on success, or a negative value if the entry does not exist.
 */
int directory_delete(directory_t *dir, const char *name);

/**
 * @brief Looks up the inode number for a given name within the directory.
 *
 * This function searches the directory entries for one that matches the provided name.
 * If found, it returns the associated inode number; otherwise, it indicates that the name 
 * does not exist in this directory.
 *
 * @param dir  A pointer to the directory.
 * @param name The name to search for (null-terminated).
 * @return The inode number of the matched entry, or a negative value (e.g., -1) if not found.
 */
int directory_lookup(directory_t *dir, const char *name);

/**
 * @brief Lists all entries in the directory.
 *
 * This function creates a singly linked list (slist_t) of the entry names in the given directory.
 * Each node in the returned list corresponds to a directory entry. 
 * The caller is responsible for freeing this list using s_free().
 *
 * @param dir A pointer to the directory from which to list entries.
 * @return A singly linked list (slist_t) of entry names. Returns NULL if the directory is empty.
 */
slist_t *directory_list(const directory_t *dir);

#endif
