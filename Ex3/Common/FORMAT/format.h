#ifndef FORMAT_H 
#define FORMAT_H 
#include <stddef.h> 
#include <stdint.h> 
/* Ghep prefix + so nguyen + suffix, them ky tu '\0'. */
/* Tra so byte khong tinh '\0'; tra 0 neu bo dem khong du. */
size_t Format_U32(char *buffer, size_t capacity, const char *prefix, 
                  uint32_t value, const char *suffix); 
#endif 
