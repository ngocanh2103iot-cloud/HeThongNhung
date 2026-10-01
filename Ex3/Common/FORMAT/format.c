#include "format.h" 
/* Ghep tien to, so nguyen va hau to; tra 0 neu bo dem khong du. */
size_t Format_U32(char *buffer, size_t capacity, const char *prefix, 
                  uint32_t value, const char *suffix) 
{
    char digits[10]; 
    size_t count = 0; 
    size_t prefix_len = 0; 
    size_t suffix_len = 0; 
    size_t length; 
    size_t i = 0; 
    /* Tach chu so theo thu tu nguoc va dem do dai hai chuoi. */
    do { 
        digits[count++] = (char)('0' + value % 10U);
        value /= 10U;
    } while (value != 0U); 
    /* Dem do dai tien to va hau to de kiem tra dung luong truoc khi chep. */
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
    /* Chep tien to, dao thu tu chu so, them hau to va ky tu ket thuc. */
    length = prefix_len + count + suffix_len;
    for (size_t j = 0; j < prefix_len; j++) { 
        buffer[i++] = prefix[j]; 
    }
    /* Chep nguoc mang digits de thu duoc thu tu chu so dung. */
    while (count > 0U) { 
        buffer[i++] = digits[--count]; 
    }
    /* Chep hau to, them ky tu ket thuc va tra do dai khong tinh ky tu ket thuc. */
    for (size_t j = 0; j < suffix_len; j++) { 
        buffer[i++] = suffix[j]; 
    }
    buffer[i] = '\0'; 
    return length; 
}
