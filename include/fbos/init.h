#ifndef __FBOS_INIT_H
#define __FBOS_INIT_H

#include <fbos/compiler.h>
#include <fbos/dt.h>
#include <fbos/sched.h>

// Tracks the amount of seconds that have elapsed since activating timer
// interrupts.
//
// Instantiated in kernel/trap.c, initialized in main.c.
extern uint64_t seconds_elapsed;

// General information from the DTB blob. This will be properly initialized at
// the very beginning of kernel initialization.
//
// Instantiated and initialized in kernel/main.c.
extern struct dt_info info;

// Extract the executables from the initrd that is located at `base_addr` and
// has the given `size`.
void extract_initrd(const unsigned char *const base_addr, uint64_t size,
					struct task_struct tasks[4]);

// Setup the interrupt vectors and the SBI timer.
void setup_interrupts(void);

// The entry point for the kernel.
void start_kernel(void *dtb);

#endif /* __FBOS_INIT_H */
