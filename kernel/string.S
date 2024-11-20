/*
 * Defined in include/fbos/string.h
 *
 *     size_t strlen(const char *str)
 *
 * Returns (a0): string length.
 * Parameter (a0): string to measure.
 * Clobbers: t0, t1.
 */
.globl strlen
.type strlen, @function
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

/*
 * Defined in include/fbos/string.h
 *
 *     int strcmp(const char *s1, const char *s2)
 *
 * Returns (a0): comparison result as in stdlib.
 * Parameter (a0, a1): strings to compare.
 * Clobbers: t0, t1.
 */
.globl strcmp
.type strcmp, @function
strcmp:
1:
	lbu t0, 0(a0)
	lbu	t1, 0(a1)
	bne	t0, t1, 2f
	addi a0, a0, 1
	addi a1, a1, 1
	bnez t0, 1b
	li a0, 0
	ret
2:
	sub	a0, t0, t1
	ret
