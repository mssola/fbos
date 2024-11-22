.global _start
.text

_start:
	li a7, 1
	la a0, buzz
	ecall
.Loop:
	j .Loop

.section .rodata
buzz:
	.string "buzz\n"
