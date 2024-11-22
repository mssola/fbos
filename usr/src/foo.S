.global _start
.text

_start:
	li a7, 1
	la a0, foo
	ecall
.Loop:
	j .Loop

.section .rodata
foo:
	.string "foo\n"
