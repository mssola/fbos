#ifndef __FBOS_PRINTK_H_
#define __FBOS_PRINTK_H_

#include <fbos/compiler.h>

/*
 * This file might be pulled from user space tests. Hence, define alternatives
 * for this functions from glibc.
 */

#ifdef __KERNEL__
// Print the given message and loop indefinitely.
extern void die(const char *const message);

// Print the given number as a single digit.
extern void print_digit(uint64_t digit);

// Print the given message.
extern void printk(const char *const message);

// Print the given message which is exactly 'n' bytes long.
extern void write(const char *const message, size_t n);

// 'write' system call.
extern void sys_write(const char *const message, size_t n);
#else
#include <stdio.h>
#include <stdlib.h>

#define die(x)     \
	do {           \
		printf(x); \
		exit(1);   \
	} while (0)
#define printk(x) printf(x)
#endif /* __KERNEL */

#endif // __FBOS_PRINTK_H_
