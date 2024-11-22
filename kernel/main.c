#include <fbos/init.h>
#include <fbos/printk.h>
#include <fbos/mm.h>
#include <fbos/sched.h>
#include <fbos/dt.h>

unsigned long init_stack[4][THREAD_SIZE / sizeof(unsigned long)];

struct task_struct tasks[4] = {
	[0] = { .stack = init_stack[0] },
	[1] = { .stack = init_stack[1] },
	[2] = { .stack = init_stack[2] },
	[3] = { .stack = init_stack[3] },
};

/*
 * This is the main entry point of the kernel after head.S is done. This
 * function can (and will) assume that everything has been reset and that we can
 * start the whole thing.
 */
__noreturn __kernel void start_kernel(void *dtb)
{
	// TODO: disable irqs, etc.

	printk("Welcome to FizzBuzz OS!\n");

	struct initrd_addr addr = find_dt_initrd_addr(dtb);

	extract_initrd((unsigned char *)addr.start, addr.end - addr.start);

	// TODO: this is the jump address for the first task.
	const char *ddr = (const char *)tasks[1].addr + tasks[1].entry_offset;
	__unused(ddr);

	// TODO: reenable stuff

	for (;;)
		;
}
