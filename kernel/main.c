#include <fbos/init.h>
#include <fbos/printk.h>
#include <fbos/mm.h>
#include <fbos/sched.h>
#include <fbos/dt.h>

unsigned long init_stack[4][THREAD_SIZE / sizeof(unsigned long)];

struct task_struct tasks[4] = {
	[TASK_INIT] = { .stack = init_stack[0], .addr = nullptr, .entry_offset = 0, },
	[TASK_FIZZ] = { .stack = init_stack[1], .addr = nullptr, .entry_offset = 0, },
	[TASK_BUZZ] = { .stack = init_stack[2], .addr = nullptr, .entry_offset = 0, },
	[TASK_FIZZBUZZ] = { .stack = init_stack[3], .addr = nullptr, .entry_offset = 0, },
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
	extract_initrd((unsigned char *)addr.start, addr.end - addr.start);

	// At this point everything has already been handled: setup the interrupt
	// vector and enable the timer to start ticking and scheduling the three
	// tasks at hand.
	seconds_elapsed = 0;
	setup_interrupts();

	for (;;) {
		// Put the machine on low power consumption since we are not doing
		// anything fancy here.
		asm volatile("wfi");
	}
}
