#include <fbos/init.h>
#include <fbos/printk.h>
#include <fbos/mm.h>
#include <fbos/sched.h>
#include <fbos/dt.h>

unsigned long init_stack[THREAD_SIZE / sizeof(unsigned long)];

struct task_struct init_task = { .stack = init_stack };

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

	extract_initrd((char *)addr.start, addr.end - addr.start);

	// TODO: reenable stuff

	for (;;)
		;
}
