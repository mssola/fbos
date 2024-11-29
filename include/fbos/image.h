#ifndef __FBOS_IMAGE_H
#define __FBOS_IMAGE_H

/*
 * General definitions for building up an image that is suitable on real
 * hardware that expect a Linux kernel header.
 */

// Deprecated image magic.
#define RISCV_IMAGE_MAGIC "RISCV\0\0\0"

// Image magic that the bootloader expects. It's placed at the same position as
// ARM64, which is why the previous 'RISCV_IMAGE_MAGIC' has been deprecated in
// favour of this one.
#define RISCV_IMAGE_MAGIC2 "RSC\x05"

// Image header version as defined by Linux in the format of
// u32: MSB| u16: major | u16: minor |LSB
#define RISCV_HEADER_VERSION_MAJOR 0
#define RISCV_HEADER_VERSION_MINOR 2
#define RISCV_HEADER_VERSION (RISCV_HEADER_VERSION_MAJOR << 16 | RISCV_HEADER_VERSION_MINOR)

#endif /* __FBOS_IMAGE */
