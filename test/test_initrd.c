#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <fbos/init.h>
#include <fbos/sched.h>
#include <fbos/mm.h>

unsigned long stack[STACK_SIZE / sizeof(unsigned long)];

struct task_struct tasks[4] = {
	[TASK_INIT] = { .stack = stack, .entry_addr = NULL, },
	[TASK_FIZZ] = { .stack = stack, .entry_addr = NULL, },
	[TASK_BUZZ] = { .stack = stack, .entry_addr = NULL, },
	[TASK_FIZZBUZZ] = { .stack = stack, .entry_addr = NULL, },
};

int main(void)
{
	FILE *fh = fopen("./usr/initramfs.cpio", "rb");
	assert(fh);

	fseek(fh, 0, SEEK_END);
	long fsize = ftell(fh);
	rewind(fh);

	unsigned char *contents = malloc((unsigned long)fsize + 1);
	fread(contents, (unsigned long)fsize, 1, fh);
	fclose(fh);

	contents[fsize] = 0;

	extract_initrd(contents, (uint64_t)fsize, tasks);
	free(contents);

#ifdef __DEBUG__
	assert(((uintptr_t)tasks[0].entry_addr & 0xfff) == 0xbe8);
	assert(((uintptr_t)tasks[1].entry_addr & 0xfff) == 0x5bc);
	assert(((uintptr_t)tasks[2].entry_addr & 0xfff) == 0x7b4);
	assert(((uintptr_t)tasks[3].entry_addr & 0xfff) == 0x180);
#else
	assert(tasks[0].entry_addr);
	assert(tasks[1].entry_addr);
	assert(tasks[2].entry_addr);
	assert(tasks[3].entry_addr);
#endif

	exit(0);
}
