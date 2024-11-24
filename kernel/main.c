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

// TODO
__kernel void switch_to(int task_id)
{
	const char *ddr = (const char *)tasks[task_id].addr + tasks[task_id].entry_offset;

	asm volatile("csrc sstatus, %[mask]" : : [mask] "r"(1 << 8));
	asm volatile("mv ra, %0" : : "r"(ddr));
	asm volatile("csrw sepc, ra");
}

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

	// Loop indefinitely while preserving power.
	for (;;) {
		asm volatile("wfi");
	}
}
