.global _start
.text

_start:
	li a7, 1
	la a0, fizz
	ecall
.Loop:
	j .Loop

.section .rodata
fizz:
	.string "fizz\n"
