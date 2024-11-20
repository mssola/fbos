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

	struct initrd_addr addr = find_dt_initrd_addr(contents);
	free(contents);

	assert(addr.start == 0x84200000);
	assert(addr.end == 0x84200c00);

	exit(0);
}
