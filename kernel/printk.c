#include <fbos/printk.h>
#include <fbos/sched.h>
#include <fbos/string.h>
#include <fbos/sbi.h>

__kernel __noreturn void die(const char *const message)
{
	if (message) {
		printk(message);
	}

	for (;;)
		;
}

__kernel void print_digit(uint64_t digit)
{
	char buffer[2];

	if (digit > 9) {
		die("We cannot print numbers with two or more digits :D\n");
	}

	buffer[0] = '0' + digit;
	buffer[1] = '\0';
	write(buffer, 2);
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
