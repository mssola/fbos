.global _start
.text

_start:
	li a0, 1
	la a1, foo
	ecall
.Loop:
	j .Loop

.section .rodata
foo:
	.string "foo\n"
