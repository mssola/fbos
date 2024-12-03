#ifndef __FBOS_MM_H
#define __FBOS_MM_H

/*
 * Page = 4KB. We don't actually use pagination, but this value is still useful
 * when aligning in the linker file.
 */
#define PAGE_SIZE 0x1000

// Size of the stack as used by processes on this kernel.
#define STACK_SIZE PAGE_SIZE

/*
 * The code will be linked to start at the first page, which will have a given
 * offset.
 */

#define PAGE_OFFSET 0x80200000
#define LOAD_OFFSET PAGE_OFFSET

#endif /* __FBOS_MM_H */
