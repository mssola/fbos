#ifndef __FBOS_DT_H_
#define __FBOS_DT_H_

#include <fbos/compiler.h>

// Default value for the 'dt_info.cpu_freq' property if none could be retrieved.
#define DEFAULT_CPU_FREQ 10000000

// Holds all the information that we gather from the initial DeviceTree blob.
struct dt_info {
	// Ticks per second. If `get_dt_info` fails at setting this value, then
	// `DEFAULT_CPU_FREQ` is used.
	uint64_t cpu_freq;

	// Start and end addresses of the initramfs blob as stored in memory.
	uintptr_t initrd_start;
	uintptr_t initrd_end;
};

// Set 'info' by parsing the given 'dtb' blob.
void get_dt_info(uint32_t *dtb, struct dt_info *info);

#endif // __FBOS_DT_H_
