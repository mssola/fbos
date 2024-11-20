#ifndef __FBOS_DT_H_
#define __FBOS_DT_H_

#include <fbos/compiler.h>

// Pair of addresses where the initrd is located in memory.
struct initrd_addr {
	uintptr_t start;
	uintptr_t end;
};

// Returns the `initrd` addresses as parsed from the given DTB blob.
struct initrd_addr find_dt_initrd_addr(uint32_t *dtb);

#endif // __FBOS_DT_H_
