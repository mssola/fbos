#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <fbos/dt.h>

int main(void)
{
	FILE *fh = fopen("./test/testdata/qemu.dtb", "rb");
	assert(fh);

	fseek(fh, 0, SEEK_END);
	long fsize = ftell(fh);
	rewind(fh);

	uint32_t *contents = malloc((unsigned long)fsize + 1);
	fread(contents, (unsigned long)fsize, sizeof(uint32_t *), fh);
	fclose(fh);

	contents[fsize] = 0;

	struct dt_info info = {
		.cpu_freq = 0,
		.initrd_start = 0,
		.initrd_end = 0,
	};
	get_dt_info(contents, &info);
	free(contents);

	assert(info.initrd_start == 0x84200000);
	assert(info.initrd_end == 0x84200c00);
	assert(info.cpu_freq == 0x989680);

	exit(0);
}
