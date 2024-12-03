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

// Maximum length for the name of the process.
#define TASK_NAME_LEN 16

// All the information we need to grab for processes.
struct task_struct {
	// Name of the task to be printed for debugging purposes.
	const char name[TASK_NAME_LEN];

	// The address for the binary entry.
	const void *entry_addr;
};

// Instantiated in kernel/main.c.
extern uint64_t stack[];

// Tasks available on this kernel.
extern struct task_struct tasks[4];

// Prepare for switching to the given task. Note that this will not actually
// jump into the given task, but it will prepare the relevant registers before
// performing the actual jump.
__kernel __always_inline void prepare_switch_to(int task_id)
{
	register struct task_struct *current asm("tp");

	asm volatile("csrc sstatus, %0\n\t"
				 "mv t0, %1\n\t"
				 "csrw sepc, t0" ::"r"(1 << 8),
				 "r"(tasks[task_id].entry_addr)
				 : "t0");

	current = &tasks[task_id];

	// Not really unused, but I was getting into lots of petty trouble with GCC
	// depending if running with/without DEBUG on. Hence, let's set 'tp' in C
	// instead of with inline assembly, even if the compiler thinks the variable
	// is set but not used.
	__unused(current);
}

#endif // __FBOS_SCHED_H_
