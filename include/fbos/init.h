#ifndef __FBOS_INIT_H
#define __FBOS_INIT_H

#include <fbos/compiler.h>

// Tracks the amount of seconds that have elapsed since activating timer
// interrupts.
//
// Instantiated in kernel/trap.c, initialized in main.c.
extern uint64_t seconds_elapsed;

// Extract the executables from the initrd that is located at `base_addr` and
// has the given `size`.
void extract_initrd(const unsigned char *const base_addr, uint64_t size);

// Setup the interrupt vectors and the SBI timer.
void setup_interrupts(void);

// The entry point for the kernel.
void start_kernel(void *dtb);

#endif /* __FBOS_INIT_H */
