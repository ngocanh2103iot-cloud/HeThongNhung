#include "format.h"
size_t Format_U32(char *buffer, size_t capacity, const char *prefix,
                  uint32_t value, const char *suffix)
{
    char digits[10];
    size_t count = 0;
    size_t prefix_len = 0;
    size_t suffix_len = 0;
    size_t length;
    size_t i = 0;
    do {
        digits[count++] = (char)('0' + value % 10U);
        value /= 10U;
    } while (value != 0U);
    while (prefix[prefix_len] != '\0') {
        prefix_len++;
    }
    while (suffix[suffix_len] != '\0') {
        suffix_len++;
    }
    /* Kiem tra cho cho chuoi va ky tu ket thuc. */
    if (prefix_len >= capacity || count >= capacity - prefix_len ||
        suffix_len >= capacity - prefix_len - count) {
        if (capacity > 0U) {
            buffer[0] = '\0';
        }
        return 0;
    }
    length = prefix_len + count + suffix_len;
    for (size_t j = 0; j < prefix_len; j++) {
        buffer[i++] = prefix[j];
    }
    while (count > 0U) {
        buffer[i++] = digits[--count];
    }
    for (size_t j = 0; j < suffix_len; j++) {
        buffer[i++] = suffix[j];
    }
    buffer[i] = '\0';
    return length;
}
