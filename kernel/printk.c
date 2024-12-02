#include <fbos/printk.h>
#include <fbos/sched.h>
#include <fbos/string.h>
#include <fbos/sbi.h>

void __noreturn __kernel die(const char *const message)
{
	if (message) {
		printk(message);
	}

	for (;;)
		;
}

__kernel void write(const char *const message, size_t n)
{
	struct sbi_ret ret = sbi_ecall2(DBCN_EXT, DBCN_WRITE, n, (unsigned long)message);
	if (ret.error != SBI_SUCCESS) {
		die(nullptr);
	}
}

#ifdef __DEBUG__
__kernel void print_task_prefix(void)
{
	register struct task_struct *current asm("tp");

	if (!current) {
		return;
	}

	size_t len = strlen(current->name);
	if (!len) {
		return;
	}

	write("[=> ", 4);
	write(current->name, len);
	write("] ", 2);
}
#endif

__kernel void printk(const char *const message)
{
#ifdef __DEBUG__
	print_task_prefix();
#endif

	size_t len = strlen(message);
	if (!len) {
		return;
	}

	write(message, len);
}

__kernel void sys_write(const char *const message, size_t n)
{
#ifdef __DEBUG__
	print_task_prefix();
#endif

	write(message, n);
}
