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
 * The size of the property is to be provided by `prop_size`.
 *
 * Returns -1 if the given property could not be found.
 */
__kernel int64_t find_dt_property_from(uint32_t *dtb, struct fdt_header *header, uint32_t idx,
									   const char *const name, size_t prop_size)
{
	char *base_dt_string = ((char *)dtb) + header->off_dt_string;
	uint32_t len, nameoff;
	int64_t ret;

	while (dtb[idx] == FDT_PROP_LE) {
		len = __bswap_constant_32(dtb[idx + 1]);
		nameoff = __bswap_constant_32(dtb[idx + 2]);

		if (len == prop_size && strcmp(&base_dt_string[nameoff], name) == 0) {
			if (len == sizeof(uint32_t)) {
				return (int64_t)__bswap_constant_32(dtb[idx + 3]);
			}
			ret = (int64_t)__bswap_constant_32(dtb[idx + 3]) << 32;
			ret += (int64_t)__bswap_constant_32(dtb[idx + 4]);
			return ret;
		}

		// Length of the data + original FDT_PROP_LE + len + nameoff.
		idx += (len / sizeof(uint32_t)) + 3;
	}
	return -1;
}

/*
 * Returns the index of the node identified by 'name' into the 'dtb' blob. This
 * index will already account for the padding.
 *
 * Returns -1 if the node could not be found.
 */
__kernel int32_t find_dt_node(uint32_t *dtb, struct fdt_header *header, const char *const name)
{
	uint32_t idx;

	// Try to find out the 32-bit offset of the "chosen" property inside of the
	// FDT structure block.
	for (idx = header->off_dt_struct; idx < header->size_dt_struct; idx++) {
		/*
		 * We only care about beginning of nodes, and then that the block has
		 * the interesting 'name'. After that, our offset will be that + 3
		 * (skipping FDT_BEGIN_NODE + 2 that takes "chosen/cpus" with padding
		 * for alignment).
		 *
		 * NOTE: for future extension, the +2 stems from "chosen/cpus". If there
		 * is another node name to be found, we are cooked.
		 */
		if (dtb[idx] == FDT_BEGIN_NODE_LE) {
			if (strcmp((char *)&dtb[idx + 1], name) == 0) {
				idx += 3;
				break;
			}
		}
	}

	// "chosen" property could not be found. Leave early with an empty result.
	if (idx == header->size_dt_struct || dtb[idx] != FDT_PROP_LE) {
		return -1;
	}
	return (int32_t)idx;
}

// Returns the index in the 'dtb' of the first proper node.
__kernel uint32_t first_dt_node(uint32_t *dtb, struct fdt_header *header)
{
	uint32_t idx;

	for (idx = header->off_dt_struct; idx < header->size_dt_struct; idx++) {
		if (dtb[idx] == FDT_BEGIN_NODE_LE) {
			return idx;
		}
	}

	return 0;
}

// Set the 'model' buffer from 'info' if available on the 'dtb' blob.
__kernel void set_dt_model(uint32_t *dtb, struct dt_info *info, struct fdt_header *header)
{
	uint32_t len, nameoff;
	uint32_t idx = first_dt_node(dtb, header);
	char *base_dt_string = ((char *)dtb) + header->off_dt_string;

	while (idx < header->size_dt_struct) {
		len = __bswap_constant_32(dtb[idx + 1]);
		nameoff = __bswap_constant_32(dtb[idx + 2]);
		idx += 3;

		if (strcmp(&base_dt_string[nameoff], "model") == 0) {
			// In the (very unlikely) case that the model string is huge, don't
			// even try.
			if (len >= DT_MODEL_MAX) {
				printk("WARNING: The model string is too large!\n");
				break;
			}

			// And copy the string. Since we know the length of it, we can
			// simply use 'memcpy' which is already available on kernel code as
			// well.
			memcpy(info->model, (char *)&dtb[idx], len);
			info->model[len] = '\0';
			break;
		}

		// Skip until the beginning of the next node.
		for (; dtb[idx] != FDT_PROP_LE && idx < header->size_dt_struct; idx++)
			;
	}
}

// Set the 'cpu_freq' field of 'info' if available on the 'dtb' blob.
__kernel void set_cpu_freq(uint32_t *dtb, struct dt_info *info, struct fdt_header *header)
{
	int32_t idx = find_dt_node(dtb, header, "cpus");
	if (idx < 0) {
		return;
	}

	int64_t i =
		find_dt_property_from(dtb, header, (uint32_t)idx, "timebase-frequency", sizeof(uint32_t));
	if (i > 0) {
		info->cpu_freq = (uint64_t)i;
	}
}

// Set the "initrd" values from the given 'dtb' blob into 'info'.
__kernel void set_initrd_addr(uint32_t *dtb, struct dt_info *info, struct fdt_header *header)
{
	int32_t idx = find_dt_node(dtb, header, "chosen");
	if (idx < 0) {
		return;
	}

	/*
	 * Now that we have the index of the "chosen" property, fetch the "initrd-*"
	 * values and return that.
	 */

	int64_t i =
		find_dt_property_from(dtb, header, (uint32_t)idx, "linux,initrd-start", sizeof(uint64_t));
	if (i < 0) {
		return;
	}
	info->initrd_start = (uintptr_t)i;

	i = find_dt_property_from(dtb, header, (uint32_t)idx, "linux,initrd-end", sizeof(uint64_t));
	if (i < 0) {
		info->initrd_start = 0;
		return;
	}
	info->initrd_end = (uintptr_t)i;
}

__kernel void get_dt_info(uint32_t *dtb, struct dt_info *info)
{
	if (dtb[0] != FDT_MAGIC_LE) {
		die("FDT structure does not have a valid magic identifier\n");
	}

	struct fdt_header header = {
		.off_dt_struct = __bswap_constant_32(dtb[2]) / sizeof(uint32_t),
		.off_dt_string = __bswap_constant_32(dtb[3]),
		.size_dt_struct = __bswap_constant_32(dtb[9]) / sizeof(uint32_t),
	};

	set_dt_model(dtb, info, &header);

	set_initrd_addr(dtb, info, &header);
	if (!info->initrd_start || !info->initrd_end) {
		die("Could not fetch the addresses for the initramfs\n");
	}

	set_cpu_freq(dtb, info, &header);
	if (!info->cpu_freq) {
		printk("WARNING: could not figure out the CPU frequency. "
			   "Defaulting to 10000000 even if this might be bad\n");
		info->cpu_freq = DEFAULT_CPU_FREQ;
	}
}
