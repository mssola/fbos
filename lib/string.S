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
	lbu t1, 0(a1)
	addi a0, a0, 1
	addi a1, a1, 1
	bne t0, t1, 2f
	bnez t0, 1b
	li a0, 0
	ret
2:
	sub a0, t0, t1
	ret

/*
 * Defined in include/fbos/string.h
 *
 *     int memcmp(const void *ptr1, const void *ptr2, size_t n)
 *
 * Returns (a0): comparison result as in stdlib.
 * Parameters (a0, a1): strings to compare.
 * Clobbers: t0, t1.
 */
.global memcmp
.type memcmp, @function
memcmp:
1:
	lbu t0, 0(a0)
	lbu t1, 0(a1)
	bne t0, t1, 2f
	addi a0, a0, 1
	addi a1, a1, 1
	addi a2, a2, -1
	bnez a2, 1b
	li a0, 0
	ret
2:
	sub a0, t0, t1
	ret

/*
 * Defined in include/fbos/string.h
 *
 *     void memcpy(const void *dest, const void *src, size_t count)
 *
 * Parameter a0: destination string.
 * Parameter a1: source string.
 * Parameter a2: number of bytes to copy.
 * Clobbers: t0.
 */
.global memcpy
.type memcpy, @function
memcpy:
1:
	beqz a2, 3f
2:
	lbu t0, 0(a1)
	sb t0, 0(a0)
	addi a0, a0, 1
	addi a1, a1, 1
	addi a2, a2, -1
	bnez a2, 2b
3:
	ret
