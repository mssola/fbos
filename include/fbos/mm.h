#ifndef FBOS_MM_H
#define FBOS_MM_H

/*
 * Page = 4KB.
 */
#define PAGE_SIZE 0x1000

/*
 * The code will be linked to start at the first page, which will have a given
 * offset.
 */
#define PAGE_OFFSET 0x80200000
#define LINK_ADDR PAGE_OFFSET

#endif /* FBOS_MM_H */
