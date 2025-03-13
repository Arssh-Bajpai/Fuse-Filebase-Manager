#ifndef BITMAP_H
#define BITMAP_H

/**
 * @brief Retrieves the value of a bit in the bitmap at a given index.
 *
 * This function checks the bitmap `bm` at bit position `i`. It returns the bit value (0 or 1).
 *
 * @param bm A pointer to the bitmap array.
 * @param i The zero-based index of the bit to check.
 * @return The value of the bit (0 or 1).
 */
int bitmap_get(void *bm, int i);

/**
 * @brief Sets or clears a bit in the bitmap.
 *
 * This function updates the bitmap `bm` at bit position `i` to the provided value `v`.
 * If `v` is 1, the bit is set; if `v` is 0, the bit is cleared.
 *
 * @param bm A pointer to the bitmap array.
 * @param i The zero-based index of the bit to set or clear.
 * @param v The value to set (0 or 1).
 */
void bitmap_put(void *bm, int i, int v);

/**
 * @brief Finds the first unused (clear) bit in the bitmap.
 *
 * This function scans the bitmap `bm` (with `size` bits) for the first bit that is 0.
 * If found, it returns the index of that bit. If all bits are set, it returns a negative value.
 *
 * @param bm A pointer to the bitmap array.
 * @param size The number of bits in the bitmap.
 * @return The zero-based index of the first unused bit, or a negative value if none are free.
 */
int bitmap_first_unused(void *bm, int size);

/**
 * @brief Prints the bitmap to standard output.
 *
 * This function prints a representation of the bitmap, `size` bits long, showing which bits are set and which are clear.
 * Useful for debugging.
 *
 * @param bm A pointer to the bitmap array.
 * @param size The number of bits in the bitmap.
 */
void bitmap_print(void *bm, int size);

#endif
