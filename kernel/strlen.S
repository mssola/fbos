.globl strlen
.type strlen, @function

/*
 * Defined in include/fbos/string.h
 *
 *     size_t strlen(const char *str)
 *
 * Returns (a0): string length.
 * Parameter (a0): string to measure.
 * Clobbers: t0, t1.
 */
strlen:
	mv t1, a0
1:
	lbu t0, 0(t1)
	beqz t0, 2f
	addi t1, t1, 1
	j 1b
2:
	sub a0, t1, a0
	ret
