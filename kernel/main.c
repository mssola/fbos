#include <fbos/init.h>
#include <fbos/printk.h>
#include <fbos/mm.h>
#include <fbos/sched.h>
#include <fbos/string.h>
#include <fbos/dt.h>

// Stack to be used by our processes. "Blasphemy!" I hear you say. "How dare you
// use the same stack for kernel and user space?" It's not like this is some
// sort of utopian system in which everyone shares everything, but since this
// stupidly simple kernel does not even bother to implement paging nor any other
// memory protection of any kind, it's not like separating stacks for each
// process and kernel space would make much of a difference. Hence, let's keep
// it simple and have the same stack everwhere.
uint64_t stack[STACK_SIZE / sizeof(uint64_t)];

// Initialize the list of structs by providing a fixed stack address and empty
// values everywhere else.
struct task_struct tasks[4] = {
	[TASK_INIT] = { .stack = stack, .name = "init", .entry_addr = nullptr, },
	[TASK_FIZZ] = { .stack = stack, .name = "fizz", .entry_addr = nullptr, },
	[TASK_BUZZ] = { .stack = stack, .name = "buzz", .entry_addr = nullptr, },
	[TASK_FIZZBUZZ] = { .stack = stack, .name = "fizzbuzz", .entry_addr = nullptr, },
};

// Defined in fbos/init.h.
struct dt_info info = {
	.model = { '\0' },
	.cpu_freq = 0,
	.initrd_start = 0,
	.initrd_end = 0,
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
	get_dt_info(dtb, &info);
	extract_initrd((unsigned char *)info.initrd_start, info.initrd_end - info.initrd_start, tasks);

	// If we were able to fetch the model, print it now.
	if (info.model[0] != '\0') {
		printk("Running on: ");
		write(info.model, strlen(info.model));
		write("\n", 1);
	}

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
