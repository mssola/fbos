#ifndef __FBOS_STRING_H_
#define __FBOS_STRING_H_

#include <fbos/compiler.h>

extern size_t strlen(const char *);
extern int strcmp(const char *, const char *);
extern int memcmp(const void *, const void *, size_t);
extern void *memcpy(void *, const void *, size_t);

#endif // __FBOS_STRING_H_
