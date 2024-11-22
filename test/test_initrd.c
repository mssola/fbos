#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <fbos/init.h>
#include <fbos/sched.h>
#include <fbos/mm.h>

unsigned long init_stack[4][THREAD_SIZE / sizeof(unsigned long)];

struct task_struct tasks[4] = {
	[0] = { .stack = init_stack[0] },
	[1] = { .stack = init_stack[1] },
	[2] = { .stack = init_stack[2] },
	[3] = { .stack = init_stack[3] },
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

	extract_initrd(contents, (uint64_t)fsize);
	free(contents);

	// TODO

	exit(0);
}
