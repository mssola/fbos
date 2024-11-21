#ifndef __FBOS_SCHED_H_
#define __FBOS_SCHED_H_

enum task_id {
	TASK_UNKNOWN = -1,
	TASK_INIT = 0,
	TASK_FOO = 1,
	TASK_BAR = 2,
	TASK_FOOBAR = 3,
};

struct task_struct {
	void *stack;
};

#endif // __FBOS_SCHED_H_
