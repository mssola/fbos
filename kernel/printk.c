#include <fbos/printk.h>
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

void __kernel printk(const char *const message)
{
	size_t len = strlen(message);
	if (!len) {
		return;
	}

	struct sbi_ret ret = sbi_ecall2(DBCN_EXT, DBCN_WRITE, len, (unsigned long)message);
	if (ret.error != SBI_SUCCESS) {
		die(nullptr);
	}
}
