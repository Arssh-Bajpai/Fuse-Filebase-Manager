#include "slist.h"
#include <stdlib.h>
#include <string.h>

/**
 * @brief Creates a new singly linked list node containing the given text.
 *
 * This function allocates a new slist_t node, duplicates the provided string `text`, 
 * and sets the node's `next` pointer to the provided `rest`. The newly created node 
 * can be used as the head of a singly linked list, effectively prepending `text` to the 
 * front of the list.
 *
 * @param text A null-terminated string to store in the new node.
 * @param rest A pointer to the remainder of the list, or NULL if this is the last node.
 * @return A pointer to the newly created slist_t node.
 */
slist_t *s_cons(const char *text, slist_t *rest) {
    slist_t *node = malloc(sizeof(slist_t));
    node->data = strdup(text);   // Duplicate the string so it can be safely stored.
    node->next = rest;           // Link the new node to the existing list (rest).
    return node;
}

/**
 * @brief Deallocates memory for an entire singly linked list.
 *
 * This function frees all nodes in the provided list, as well as the strings they contain.
 * After this call, neither the list nor its data should be accessed. 
 *
 * @param list A pointer to the head of the list to be freed. If NULL, no operation is performed.
 */
void s_free(slist_t *list) {
    while (list) {
        slist_t *next = list->next; // Keep track of the next node before freeing current.
        free(list->data);           // Free the duplicated string.
        free(list);                 // Free the node.
        list = next;                // Move to the next node.
    }
}

/**
 * @brief Splits a string into substrings at each occurrence of the given delimiter, 
 *        returning these substrings as a singly linked list.
 *
 * This function searches through the input string `str` for the delimiter character `delimiter`. 
 * Each substring between occurrences of `delimiter` (or between the start/end of `str` and a `delimiter`) 
 * is added to a new list node. The final result is a linked list containing all these substrings 
 * in reverse order of appearance due to the manner of construction (you can reverse it or process 
 * it as needed afterward).
 *
 * Example:
 * Given `str = "hello:world:test"` and `delimiter = ':'`, the resulting list contains nodes for 
 * "test", "world", and "hello" in that order, because each substring is prepended to the list.
 *
 * @param str A null-terminated string to split. If `str` is empty, a single node with an empty 
 *            string will be returned.
 * @param delimiter The character used to split `str` into substrings.
 * @return A pointer to the head of a linked list of substrings. The caller is responsible for 
 *         freeing this list via s_free(). If `str` is NULL, behavior is undefined.
 */
slist_t *s_explode(const char *str, char delimiter) {
    slist_t *list = NULL;     // Start with an empty list.
    const char *start = str;  // Points to the beginning of the current segment.

    while (1) {
        const char *end = strchr(start, delimiter);

        if (!end) {
            // If no more delimiters are found, this means `start` points to the 
            // last substring or possibly an empty string.
            list = s_cons(start, list); 
            break;
        }

        // Calculate the length of the current segment.
        size_t length = end - start;

        // Allocate a temporary buffer to hold the substring.
        char *segment = malloc(length + 1);
        strncpy(segment, start, length);
        segment[length] = '\0'; // Null-terminate the substring.

        // Add this substring to the front of the list.
        list = s_cons(segment, list);

        // Free the temporary segment buffer since s_cons() already duplicated it.
        free(segment);

        // Move start to the character after the delimiter to begin the next substring.
        start = end + 1;
    }

    return list;
}
