#include <fbos/init.h>
#include <fbos/printk.h>
#include <fbos/mm.h>
#include <fbos/sched.h>
#include <fbos/string.h>
#include <fbos/dt.h>

// Defined in fbos/init.h.
atomic32_t hart_lottery __section(".sdata");

// Defined in fbos/sched.h.
uint64_t stack[STACK_SIZE / sizeof(uint64_t)];

// Defined in fbos/sched.h.
struct task_struct tasks[4] = {
	[TASK_INIT] = { .name = "init", .entry_addr = nullptr, },
	[TASK_FIZZ] = { .name = "fizz", .entry_addr = nullptr, },
	[TASK_BUZZ] = { .name = "buzz", .entry_addr = nullptr, },
	[TASK_FIZZBUZZ] = { .name = "fizzbuzz", .entry_addr = nullptr, },
};

// Defined in fbos/init.h.
struct dt_info info = {
	.model = { '\0' },
	.cpu_freq = 0,
	.initrd_start = 0,
	.initrd_end = 0,
};

__noreturn __kernel void start_kernel(uint64_t hart_id, void *dtb)
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

	// And print the hart ID where this is being run.
	printk("Running on Hart ID: ");
	print_digit(hart_id);
	write("\n", 1);

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
