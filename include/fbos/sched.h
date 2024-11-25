#ifndef __FBOS_SCHED_H_
#define __FBOS_SCHED_H_

#include <fbos/compiler.h>

// All the possible IDs for the tasks on this kernel.
enum task_id {
	TASK_UNKNOWN = -1,
	TASK_INIT = 0,
	TASK_FIZZ = 1,
	TASK_BUZZ = 2,
	TASK_FIZZBUZZ = 3,
};

// All the information we need to grab for processes.
struct task_struct {
	// The stack allocated for the process. As you can see when initializing
	// each process on `kernel/main.c`, we go over the top for its size. There
	// is also the fact that we need to keep this as the first attribue to allow
	// for simple `sp` values.
	void *stack;

	// The address for the binary entry.
	const void *entry_addr;
};

// Tasks available on this kernel.
extern struct task_struct tasks[4];

// Switch execution to the given U-mode task. Note that this function will not
// do the actual returning, but it prepares the relevant registers for an
// eventual jump.
__kernel __always_inline void switch_to(int task_id)
{
	asm volatile("csrc sstatus, %0\n\t"
				 "mv t0, %1\n\t"
				 "csrw sepc, t0" ::"r"(1 << 8),
				 "r"(tasks[task_id].entry_addr)
				 : "t0");
}

#endif // __FBOS_SCHED_H_
