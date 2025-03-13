#ifndef SLIST_H
#define SLIST_H

/**
 * @brief A singly linked list node that holds a string and a pointer to the next node.
 *
 * The slist_t structure represents a node in a singly linked list of strings. Each node
 * contains a pointer to a character array `data` and a pointer `next` to the next node.
 */
typedef struct slist {
    char *data;           /**< The string data stored in this list node. */
    struct slist *next;   /**< A pointer to the next node in the singly linked list. */
} slist_t;

/**
 * @brief Constructs a new singly linked list node.
 *
 * This function creates a new list node (slist_t), copies the given string `text` into
 * it, and sets the node's `next` pointer to the provided `rest`. The newly created node
 * becomes the head of the list when prepended.
 *
 * @param text A null-terminated string to store in the list node.
 * @param rest A pointer to the remaining part of the list (or NULL if this is the last node).
 * @return A pointer to the newly created slist_t node.
 */
slist_t *s_cons(const char *text, slist_t *rest);

/**
 * @brief Frees all the nodes in a singly linked list.
 *
 * This function deallocates memory for all nodes in the given list, as well as the strings
 * they contain. After this call, the list pointer and all nodes it references will no longer
 * be valid.
 *
 * @param list A pointer to the head of the list to be freed.
 */
void s_free(slist_t *list);

/**
 * @brief Splits a string into a linked list of substrings based on a delimiter.
 *
 * This function takes a string `str` and a character `delimiter`, then splits `str` at each
 * occurrence of `delimiter`. Each substring is placed in a new node of a singly linked list.
 * The resulting list contains one node per substring, in the order they are found in `str`.
 *
 * Example: `s_explode("hello:world:test", ':')` would produce a list with `"hello"`, `"world"`,
 * and `"test"`.
 *
 * @param str A null-terminated string to split.
 * @param delimiter The character at which to split `str`.
 * @return A pointer to the head of the resulting linked list. Returns NULL if `str` is empty
 *         or NULL.
 */
slist_t *s_explode(const char *str, char delimiter);

#endif
