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
 * offset from the base address where a kernel can be allocated on a RISC-V
 * machine.
 */
#define LOAD_BASE_ADDR 0x80000000
#define LOAD_BASE_OFFSET 0x00200000
#define LOAD_OFFSET (LOAD_BASE_ADDR + LOAD_BASE_OFFSET)

#endif /* __FBOS_MM_H */
