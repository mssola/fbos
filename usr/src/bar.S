.global _start
.text

_start:
	li a0, 1
	la a1, bar
	ecall
.Loop:
	j .Loop

.section .rodata
bar:
	.string "bar\n"
