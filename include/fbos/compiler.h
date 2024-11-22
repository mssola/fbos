#ifndef __FBOS_COMPILER_H
#define __FBOS_COMPILER_H

/*
 * Nicer looking versions of compiler attributes.
 */

#define __noreturn __attribute__((__noreturn__))
#ifndef __always_inline
#define __always_inline __attribute__((__always_inline__)) inline
#endif
#define __s_interrupt __attribute__((interrupt("supervisor")))
#define __aligned(x) __attribute__((aligned(x)))

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
 * Multiple aliases for integer types which have their definition on the
 * standard library.
 */

typedef short int16_t;
typedef int int32_t;
typedef long ssize_t;
typedef long int64_t;

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long size_t;
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
