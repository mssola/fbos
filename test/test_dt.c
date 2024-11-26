#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <fbos/dt.h>

void dt_info_from(const char *const path, struct dt_info *info)
{
	FILE *fh = fopen(path, "rb");
	assert(fh);

	fseek(fh, 0, SEEK_END);
	long fsize = ftell(fh);
	rewind(fh);

	char *contents = malloc((unsigned long)fsize + 1);
	assert(contents);

	fread(contents, (unsigned long)fsize, sizeof(char), fh);
	fclose(fh);

	contents[fsize] = 0;

	get_dt_info((uint32_t *)contents, info);
	free(contents);
}

void test_qemu(void)
{
	struct dt_info info = {
		.cpu_freq = 0,
		.initrd_start = 0,
		.initrd_end = 0,
	};

	dt_info_from("./test/testdata/qemu.dtb", &info);

	assert(info.initrd_start == 0x84200000);
	assert(info.initrd_end == 0x84200c00);
	assert(info.cpu_freq == 0x989680);
}

void test_vf2(void)
{
	struct dt_info info = {
		.cpu_freq = 0,
		.initrd_start = 0,
		.initrd_end = 0,
	};

	dt_info_from("./test/testdata/vf2.dtb", &info);

	assert(info.initrd_start == 0x46100000);
	assert(info.initrd_end == 0x47139ce3);
	assert(info.cpu_freq == 0x3d0900);
}

int main(void)
{
	test_qemu();
	test_vf2();

	exit(0);
}
