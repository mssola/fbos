#include <fbos/image.h>
#include <fbos/mm.h>

.global _start
.global _start_kernel
.section .head.text

// Since this kernel boots in virtualization environments and machines that are
// expecting a Linux kernel, we have to adhere to how bootloaders expect Linux
// kernel images to look like. In particular, the entry of the kernel is
// actually a Boot image header, as defined in:
//
// 		https://docs.kernel.org/arch/riscv/boot-image-header.html
//
// Conveniently, for machines who don't care about this (e.g. QEMU), the first
// 64 bits correspond to two instructions that can be set up, and hence entering
// here will simply jumpt to `_start_kernel`, our real entry.
_start:
	// The first two words give us room for two executable instructions. Linux
	// uses that on EFI support to first allocate a magic value for UEFI and
	// then have a `j _start_kernel` instruction. Otherwise it just allocates
	// the first one for `j _start_kernel` and leaves the second word empty. The
	// latter is what we do here as well.
	j _start_kernel
	.word 0

	// Ensure alignment for the next double word.
	.balign 8

	// Load offset. Note that this matches the PAGE_OFFSET as defined in
	// `include/mm.h`.
	.dword 0x200000

	// Size of the image. This is *mandatory* as per bootloader request.
	.dword _end - _start

	// Flags. As defined by Linux, only one bit matters here, which is related
	// to endianness. Setting 0 means little-endian.
	.dword 0

	// Header version.
	.word RISCV_HEADER_VERSION

	// Reserved fields.
	.word 0
	.dword 0

	// Deprecated image magic.
	.ascii RISCV_IMAGE_MAGIC
	.balign 4

	// Good image magic, in little-endian format.
	.ascii RISCV_IMAGE_MAGIC2

	// Reserved field.
	.word 0

_start_kernel:
	// Mask all interrupts
	csrw sie, zero
	csrw sip, zero

	// Flush the instruction cache
	fence.i

	// Reset all registers except ra, a0, a1.
	li sp, 0
	li gp, 0
	li tp, 0
	li t0, 0
	li t1, 0
	li t2, 0
	li s0, 0
	li s1, 0
	li a2, 0
	li a3, 0
	li a4, 0
	li a5, 0
	li a6, 0
	li a7, 0
	li s2, 0
	li s3, 0
	li s4, 0
	li s5, 0
	li s6, 0
	li s7, 0
	li s8, 0
	li s9, 0
	li s10, 0
	li s11, 0
	li t3, 0
	li t4, 0
	li t5, 0
	li t6, 0
	csrw sscratch, 0

	// Point 'tp' to the init task. The 'tp' register will always point to the
	// current process being executed, and it will be shown on debug when
	// printing out messages.
	la tp, tasks

	// Point 'sp' the our general stack.
	la sp, stack + STACK_SIZE

	// The `start_kernel` function requires an argument to be passed, which is
	// the pointer to the `fdt` blob. The bootloader puts this on the `a1`
	// register, so let's move it now to `a0`.
	mv a0, a1

	// Start the kernel.
	tail start_kernel

	// We really shouldn't reach this point, but just in case, just loop
	// infinitely here.
	j .
