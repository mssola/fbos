#ifndef __FBOS_PRINTK_H_
#define __FBOS_PRINTK_H_

/*
 * This file might be pulled from user space tests. Hence, define alternatives
 * for this functions from glibc.
 */

#ifdef __KERNEL__
extern void die(const char *const message);
extern void printk(const char *const message);
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
