#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <fbos/init.h>
#include <fbos/sched.h>
#include <fbos/mm.h>

struct task_struct tasks[4] = {
	[TASK_INIT] = { .entry_addr = NULL, },
	[TASK_FIZZ] = { .entry_addr = NULL, },
	[TASK_BUZZ] = { .entry_addr = NULL, },
	[TASK_FIZZBUZZ] = { .entry_addr = NULL, },
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

	assert(tasks[0].entry_addr);
	assert(tasks[1].entry_addr);
	assert(tasks[2].entry_addr);
	assert(tasks[3].entry_addr);

	exit(0);
}
