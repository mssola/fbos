#ifndef __FBOS_INIT_H
#define __FBOS_INIT_H

#include <fbos/compiler.h>
#include <fbos/dt.h>
#include <fbos/sched.h>

// Atomic value that holds how many harts have gone through the "hart lottery".
// This is in the same spirit as it happens on the Linux kernel: the RISC-V
// specification leaves open which hart will appear first into the kernel code.
// This greatly simplifies the specification and the hardware, but for the
// kernel this means that harts will appear randomly. In order to know which
// hart runs first, in Linux they run a "lottery": an atomic value holds how
// many harts have already been seen. The first hart to appear will actually
// initialize things before bringing the others up, while the others will simply
// wait until the first hart frees the lock for them.
//
// That being said, here we only want *one* hart running. Hence, whichever hart
// wins the lottery, it's not only going to initialize the kernel, but it will
// also be the only one running the show. This is of course a waste of
// resources, but it's not like running fizz/buzz needs SMP and cores running at
// full speed. Actually, keeping things under a single hart simplifies things a
// lot.
//
// Also note that on both platforms I have tested this they had the HSM
// extension from the SBI specification. In this case, only one hart is woken
// up, and it's up to the supervisor to wake up the others through the HSM
// extension. Hence, for the platforms I have tried this the lottery was rigged
// from the beginning :)
//
// Instantiated in kernel/main.c
extern atomic32_t hart_lottery;

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

/*
 * This is the main entry point of the kernel after head.S is done. This
 * function expects exactly two arguments, which are exactly the same as they
 * are passed to the `_start` function in kernel/head.S by the bootloader.
 */
void start_kernel(uint64_t hart_id, void *dtb);

#endif /* __FBOS_INIT_H */
