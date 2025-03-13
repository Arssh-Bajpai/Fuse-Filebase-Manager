#include "directory.h"
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "slist.h"

// Set the directory to empty
void directory_init(directory_t *dir) {
    dir->entry_count = 0;
    memset(dir->entries, 0, sizeof(dir->entries));
}

// Add a new entry (name -> inum) if there's space and it doesn't already exist
int directory_put(directory_t *dir, const char *name, int inum) {
    if (dir->entry_count >= MAX_ENTRIES) {
        return -ENOSPC;
    }

    // Check for duplicate names
    for (int i = 0; i < dir->entry_count; i++) {
        if (strcmp(dir->entries[i].name, name) == 0) {
            return -EEXIST;
        }
    }

    // Insert the new entry
    strncpy(dir->entries[dir->entry_count].name, name, MAX_NAME_LEN);
    dir->entries[dir->entry_count].inum = inum;
    dir->entry_count++;
    return 0;
}

// Remove an entry by name if it exists
int directory_delete(directory_t *dir, const char *name) {
    for (int i = 0; i < dir->entry_count; i++) {
        if (strcmp(dir->entries[i].name, name) == 0) {
            // Shift entries down
            for (int j = i; j < dir->entry_count - 1; j++) {
                dir->entries[j] = dir->entries[j + 1];
            }
            dir->entry_count--;
            return 0;
        }
    }
    return -ENOENT;
}

// Find the inode number for a given name
int directory_lookup(directory_t *dir, const char *name) {
    for (int i = 0; i < dir->entry_count; i++) {
        if (strcmp(dir->entries[i].name, name) == 0) {
            return dir->entries[i].inum;
        }
    }
    return -ENOENT;
}

// Return a list of all entry names in the directory
slist_t *directory_list(const directory_t *dir) {
    slist_t *list = NULL;
    for (int i = 0; i < dir->entry_count; i++) {
        list = s_cons(dir->entries[i].name, list);
    }
    return list;
}
