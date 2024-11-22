.global _start
.text

_start:
	li a7, 1
	la a0, fizzbuzz
	li a1, 9
	ecall
.Loop:
	j .Loop

.section .rodata
fizzbuzz:
	.string "fizzbuzz\n"
