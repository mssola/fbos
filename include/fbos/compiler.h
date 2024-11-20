#ifndef __FBOS_COMPILER_H
#define __FBOS_COMPILER_H

/*
 * Nicer looking versions of compiler attributes.
 */

#define __noreturn __attribute__((__noreturn__))

/*
 * Compiler attributes specific to linker sections.
 */

#ifdef __KERNEL__
#define __section(s) __attribute__((__section__(s)))
#define __kernel __section(".kernel.text")
#else
#define __kernel
#endif /* __KERNEL__ */

/*
 * Multiple aliases for 32/64-bit integers which have their definition on the
 * standard library.
 */

typedef int int32_t;
typedef long ssize_t;
typedef long int64_t;

typedef unsigned long size_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;
typedef unsigned long uintptr_t;

/*
 * NULL
 */

#ifdef __KERNEL__
#define NULL (void *)0
#define nullptr NULL
#endif /* __KERNEL__ */

// Helpful macro when prototyping.
#define __unused(x) (void)x

#endif /* __FBOS_COMPILER_H */
