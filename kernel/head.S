#include <fbos/mm.h>

.global _start
.section .head.text

_start:
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

	// Point tp and sp to the init task.
	la tp, init_task
	la sp, init_task + THREAD_SIZE

	// The `start_kernel` function requires an argument to be passed, which is
	// the pointer to the `fdt` blob. The bootloader puts this on the `a1`
	// register, so let's move it now to `a0`.
	mv a0, a1

	// Start the kernel.
	tail start_kernel
