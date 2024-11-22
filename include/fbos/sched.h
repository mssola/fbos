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

// TODO: if we only care about the absolute address, it can be further
// simplified.
struct task_struct {
	void *stack;
	const void *addr;
	uint64_t entry_offset;
};

// Tasks available on this kernel.
extern struct task_struct tasks[4];

// Identifier for the next task to be run.
extern int next_task;

// Bring the machine to idle mode.
__noreturn __kernel void idle(void);

#endif // __FBOS_SCHED_H_
