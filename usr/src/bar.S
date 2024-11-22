.global _start
.text

_start:
	li a7, 1
	la a0, bar
	ecall
.Loop:
	j .Loop

.section .rodata
bar:
	.string "bar\n"
