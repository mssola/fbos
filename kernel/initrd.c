#include <fbos/init.h>
#include <fbos/sched.h>
#include <fbos/printk.h>
#include <fbos/string.h>

#define BUFFER_SIZE 32

#define CPIO_HEADER_FILESIZE 54
#define CPIO_HEADER_NAMESIZE 94
#define CPIO_HEADER_SIZE 110

// TODO: move to assembly
__kernel void *memcpy(void *dest, const void *src, size_t count)
{
	char *destc = dest;
	const char *srcc = src;

	for (uint64_t i = 0; i < count; i++) {
		*destc++ = *srcc++;
	}
	return dest;
}

// TODO: this is required by GCC which must be doing some optimization
// underneath. For now let's keep it simple (and wrong) by just calling memcpy.
__kernel void *memmove(void *dest, const void *src, size_t count)
{
	return memcpy(dest, src, count);
}

__kernel uint64_t strtoul16(const char *str, size_t count)
{
	char c;
	uint64_t ret = 0;
	uint64_t aux = 0;

	for (uint64_t i = 1; count > 0; i *= 16, count--) {
		c = str[count - 1];
		if (c >= 'A' && c <= 'F') {
			aux = 10 + (uint64_t)(c - 'A');
		} else if (c >= 'a' && c <= 'f') {
			aux = 10 + (uint64_t)(c - 'a');
		} else if (c < '0' || c > '9') {
			die("Bad number\n");
		} else {
			aux = (uint64_t)c - '0';
		}

		ret += aux * i;
	}
	return ret;
}

// Returns the 'enum taks_id' that can be gathered by the given path.
__kernel int get_task_id_from_path(const char *const path)
{
	if (strcmp(path, "usr/bin/init") == 0) {
		return TASK_INIT;
	} else if (strcmp(path, "usr/bin/fizz") == 0) {
		return TASK_FIZZ;
	} else if (strcmp(path, "usr/bin/buzz") == 0) {
		return TASK_BUZZ;
	} else if (strcmp(path, "usr/bin/fizzbuzz") == 0) {
		return TASK_FIZZBUZZ;
	}
	return TASK_UNKNOWN;
}

// Fetch the 'entry_addr' for the ELF binary pointed by 'addr'. If everything
// goes right, the task identified by 'task_id' will finally be initialized with
// said address.
__kernel void get_task_entry_addr(int task_id, const unsigned char *const addr)
{
	uint64_t offset;

	if (addr[0] != 0x7F || memcmp(&addr[1], "ELF", 3) != 0) {
		die("Bad ELF format\n");
	}
	if (addr[4] != 2) {
		die("64-bit format is mandatory\n");
	}
	if (addr[5] != 1) {
		die("Little-endian only\n");
	}

	offset = (uint64_t)addr[0x18];
	tasks[task_id].entry_addr = (const void *)(addr + offset);
}

__kernel void extract_initrd(const unsigned char *const initrd_addr, uint64_t size)
{
	char buffer[BUFFER_SIZE];
	uint64_t name_size, file_size, padding, base = 0;
	int task_id;

	// The `base` is the index from `initrd_addr` which points to the first byte
	// of the header of the currently evaluated file inside of the CPIO archive.
	while (base < size) {
		// Only the "newc" format is supported, without checksums nor fancy
		// stuff.
		if (memcmp(&initrd_addr[base], "070701", 6) != 0) {
			if (memcmp(&initrd_addr[base], "070702", 6) == 0 ||
				memcmp(&initrd_addr[base], "070707", 6) == 0) {
				die("Incorrect cpio format: stick to 'newc'");
			} else {
				die("No cpio magic number");
			}
		}

		// We identify the task being extracted by looking at the file's path,
		// so let's first get the size of it.
		memcpy(buffer, &initrd_addr[base + CPIO_HEADER_NAMESIZE], 8);
		buffer[8] = '\0';
		name_size = strtoul16(buffer, 8);
		if (name_size >= BUFFER_SIZE) {
			die("Path too large for initrd executable");
		}

		// Right after the header (hence current header + its size) there is the
		// actual file's path, which is exactly `name_size` long. Fetch it now
		// to identify the task at hand.
		memcpy(buffer, &initrd_addr[base + CPIO_HEADER_SIZE], name_size);
		buffer[name_size] = '\0';
		task_id = get_task_id_from_path(buffer);

		// Note that this is not necessarily a bad CPIO archive, it might just
		// be the end "TRAILER!!!" delimiter. Either way, just quit at this
		// point.
		if (task_id == TASK_UNKNOWN) {
			break;
		}

		// Fetch the size of the executable, which is needed for `extract_elf`,
		// as well as for advancing the `base` to the next file.
		memcpy(buffer, &initrd_addr[base + CPIO_HEADER_FILESIZE], 8);
		buffer[8] = '\0';
		file_size = strtoul16(buffer, 8);

		// Files are aligned in 4-byte boundaries after the header. That's why
		// there might be some padding in between the header and the file.
		padding = 4 - ((CPIO_HEADER_SIZE + name_size) & 3);

		// And extract everything from the ELF file for the given task.
		get_task_entry_addr(task_id, &initrd_addr[base + name_size + CPIO_HEADER_SIZE + padding]);

		// Advance the base to the next file.
		base += CPIO_HEADER_SIZE + name_size + padding + file_size;
	}
}
