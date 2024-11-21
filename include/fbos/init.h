#ifndef __FBOS_INIT_H
#define __FBOS_INIT_H

#include <fbos/compiler.h>

extern struct task_struct init_task;

// Extract the executables from the initrd that is located at `base_addr` and
// has the given `size`.
void extract_initrd(const char *const base_addr, uint64_t size);

// The entry point for the kernel.
void start_kernel(void *dtb);

#endif /* __FBOS_INIT_H */
