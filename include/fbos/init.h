#ifndef __FBOS_INIT_H
#define __FBOS_INIT_H

#include <fbos/compiler.h>

extern struct task_struct init_task;

// The entry point for the kernel.
__noreturn __kernel void start_kernel(uintptr_t *dtb);

#endif /* __FBOS_INIT_H */
