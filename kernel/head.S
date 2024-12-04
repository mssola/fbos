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

	// Load offset. Note that this matches the LOAD_BASE_OFFSET as defined in
	// `include/mm.h`.
	.dword LOAD_BASE_OFFSET

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

	// Run the hart lottery. If this is not the first time that it happens, then
	// stall this hart forever: on this simple kernel we only want one hart
	// available to avoid SMP shenanigans. See explanation on fbos/init.h.
	la a3, hart_lottery
	li a2, 1
	amoadd.w a3, a2, (a3)
	bnez a3, .Lhart_wait

	// Explicitely nullify the 'gp' and 'sscratch' registers, as they are a bit
	// special but we are not using them. For the rest of the registers, we
	// explicitely do not care to reset them.
	li gp, 0
	csrw sscratch, 0

	// Point 'tp' to the init task. The 'tp' register will always point to the
	// current process being executed, and it will be shown on debug when
	// printing out messages.
	la tp, tasks

	// Point 'sp' the our global stack. See fbos/sched.h for more details.
	la sp, stack + STACK_SIZE

	// Start the kernel. Notice that both 'a0' and 'a1' have been left
	// untouched. This is no coincidence as the values as passed from the
	// bootloader for both these two registers will be passed down to the kernel
	// as is. As with the Linux kernel these two registers contain:
	//   - a0: boot hart id.
	//   - a1: pointer to the flattened device tree blob.
	tail start_kernel

	// We really shouldn't reach this point. If so, at least mask again all
	// interrupts, flush the instruction cache and halt the current hart.
	csrw sie, zero
	csrw sip, zero
	fence.i

	// This is where harts come to die :)
.Lhart_wait:
	wfi
	j .Lhart_wait
