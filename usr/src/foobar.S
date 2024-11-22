.global _start
.text

_start:
	li a7, 1
	la a0, foobar
	ecall
.Loop:
	j .Loop

.section .rodata
foobar:
	.string "foobar\n"
