#include <fbos/init.h>
#include <fbos/printk.h>
#include <fbos/mm.h>
#include <fbos/sched.h>
#include <fbos/dt.h>

// Stacks to be used by our processes.
unsigned long stack[4][THREAD_SIZE / sizeof(unsigned long)];

// Initialize the list of structs by providing a fixed stack address and empty
// values everywhere else.
struct task_struct tasks[4] = {
	[TASK_INIT] = { .stack = stack[0], .entry_addr = nullptr, },
	[TASK_FIZZ] = { .stack = stack[1], .entry_addr = nullptr, },
	[TASK_BUZZ] = { .stack = stack[2], .entry_addr = nullptr, },
	[TASK_FIZZBUZZ] = { .stack = stack[3], .entry_addr = nullptr, },
};

/*
 * This is the main entry point of the kernel after head.S is done. This
 * function can (and will) assume that everything has been reset and that we can
 * start the whole thing.
 */
__noreturn __kernel void start_kernel(void *dtb)
{
	printk("Welcome to FizzBuzz OS!\n");

	// Extract information from the DTB blob.
	struct initrd_addr addr = find_dt_initrd_addr(dtb);
	extract_initrd((unsigned char *)addr.start, addr.end - addr.start, tasks);

	// At this point everything has already been handled: setup the interrupt
	// vector and enable the timer to start ticking and scheduling the three
	// tasks at hand.
	seconds_elapsed = 0;
	setup_interrupts();

	// Loop indefinitely while preserving power.
	for (;;) {
		asm volatile("wfi");
	}
}
