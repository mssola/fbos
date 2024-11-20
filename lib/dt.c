#include <fbos/dt.h>
#include <fbos/printk.h>
#include <fbos/string.h>

/*
 * Swap bytes in 32-bit value. Taken from glibc.
 */
#define __bswap_constant_32(x)                                                               \
	((((x) & 0xff000000u) >> 24) | (((x) & 0x00ff0000u) >> 8) | (((x) & 0x0000ff00u) << 8) | \
	 (((x) & 0x000000ffu) << 24))

// Magic number in little-endian format identifying a valid FDT structure.
#define FDT_MAGIC_LE 0xedfe0dd0

// Token representing the beginning of node on the structure block in
// little-endian format.
#define FDT_BEGIN_NODE_LE 0x01000000

// Token representing the beginning of a property on the structure block in
// little-endian format.
#define FDT_PROP_LE 0x03000000

// All the relevant information we need from the FDT header.
struct fdt_header {
	uint32_t off_dt_struct;
	uint32_t off_dt_string;
	uint32_t size_dt_struct;
};

/*
 * Find the device tree property by "name" starting at the given index "idx".
 * This function assumes that the property is exactly 8 bytes long (hey, it's
 * not so general purpose after all :D).
 *
 * Returns -1 if the given property could not be found.
 */
__kernel int64_t find_dt_property_from(uint32_t *dtb, struct fdt_header *header, uint32_t idx,
									   const char *const name)
{
	char *base_dt_string = ((char *)dtb) + header->off_dt_string;
	uint32_t len, nameoff;
	int64_t ret;

	while (dtb[idx] == FDT_PROP_LE) {
		len = __bswap_constant_32(dtb[idx + 1]);
		nameoff = __bswap_constant_32(dtb[idx + 2]);

		if (len == 8 && strcmp(&base_dt_string[nameoff], name) == 0) {
			ret = (int64_t)__bswap_constant_32(dtb[idx + 3]) << 32;
			ret += (int64_t)__bswap_constant_32(dtb[idx + 4]);
			return ret;
		}

		// Length of the data + original FDT_PROP_LE + len + nameoff.
		idx += (len / sizeof(uint32_t)) + 3;
	}
	return -1;
}

// Find the "initrd" values from the given DTB blob. Returns an empty
// `initrd_addr` if these values could not be found.
__kernel struct initrd_addr __find_dt_initrd_addr(uint32_t *dtb, struct fdt_header *header)
{
	uint32_t idx;
	struct initrd_addr ret = {
		.start = 0,
		.end = 0,
	};

	// Try to find out the 32-bit offset of the "chosen" property inside of the
	// FDT structure block.
	for (idx = header->off_dt_struct; idx < header->size_dt_struct; idx++) {
		/*
		 * We only care about beginning of nodes, and then that the block is
		 * literally named "chosen". After that, our offset will be that + 3
		 * (skipping FDT_BEGIN_NODE + 2 that takes "chosen" with padding for
		 * alignment).
		 */
		if (dtb[idx] == FDT_BEGIN_NODE_LE) {
			if (strcmp((char *)&dtb[idx + 1], "chosen") == 0) {
				idx += 3;
				break;
			}
		}
	}

	// "chosen" property could not be found. Leave early with an empty result.
	if (idx == header->size_dt_struct || dtb[idx] != FDT_PROP_LE) {
		return ret;
	}

	/*
	 * Now that we have the index of the "chosen" property, fetch the "initrd-*"
	 * values and return that.
	 */

	int64_t i = find_dt_property_from(dtb, header, idx, "linux,initrd-start");
	if (i < 0) {
		return ret;
	}
	ret.start = (uintptr_t)i;

	i = find_dt_property_from(dtb, header, idx, "linux,initrd-end");
	if (i < 0) {
		ret.start = 0;
		return ret;
	}
	ret.end = (uintptr_t)i;

	return ret;
}

__kernel struct initrd_addr find_dt_initrd_addr(uint32_t *dtb)
{
	if (dtb[0] != FDT_MAGIC_LE) {
		die("FDT structure does not have a valid magic identifier\n");
	}

	struct fdt_header header = {
		.off_dt_struct = __bswap_constant_32(dtb[2]) / sizeof(uint32_t),
		.off_dt_string = __bswap_constant_32(dtb[3]),
		.size_dt_struct = __bswap_constant_32(dtb[9]) / sizeof(uint32_t),
	};

	return __find_dt_initrd_addr(dtb, &header);
}
