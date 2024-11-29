#ifndef __FBOS_MM_H
#define __FBOS_MM_H

/*
 * Page = 4KB.
 */
#define PAGE_SIZE 0x1000

/*
 * Initial size of the thread, which coincides with the size of the stack for a
 * given thread.
 */
#define THREAD_SIZE_ORDER 2
#define THREAD_SIZE (PAGE_SIZE << THREAD_SIZE_ORDER)

/*
 * The code will be linked to start at the first page, which will have a given
 * offset.
 */

#define PAGE_OFFSET 0x80200000
#define LOAD_OFFSET PAGE_OFFSET

#endif /* __FBOS_MM_H */
