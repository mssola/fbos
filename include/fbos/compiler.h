#ifndef __FBOS_COMPILER_H
#define __FBOS_COMPILER_H

/*
 * Nicer looking versions of compiler attributes.
 */

#define __noreturn __attribute__((__noreturn__))

/*
 * Compiler attributes specific to linker sections.
 */

#define __section(s) __attribute__((__section__(s)))
#define __kernel __section(".kernel.text")

/*
 * Multiple aliases for 64-bit integers which have their definition on the
 * standard library.
 */

typedef long ssize_t;
typedef unsigned long size_t;
typedef unsigned long uint64_t;
typedef unsigned long uintptr_t;

// Helpful macro when prototyping.
#define __unused(x) (void)x

#endif /* __FBOS_COMPILER_H */
