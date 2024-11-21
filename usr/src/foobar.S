.global _start
.text

_start:
	li a0, 1
	la a1, foobar
	ecall
.Loop:
	j .Loop

.section .rodata
foobar:
	.string "foobar\n"
