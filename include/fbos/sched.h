#ifndef __FBOS_SCHED_H_
#define __FBOS_SCHED_H_

#include <fbos/compiler.h>

enum task_id {
	TASK_UNKNOWN = -1,
	TASK_INIT = 0,
	TASK_FIZZ = 1,
	TASK_BUZZ = 2,
	TASK_FIZZBUZZ = 3,
};

struct task_struct {
	void *stack;
	const void *addr;
	uint64_t entry_offset;
};

extern struct task_struct tasks[4];

#endif // __FBOS_SCHED_H_
