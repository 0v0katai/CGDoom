.global ecall_no
.global do_ecall

do_ecall:
	la ecall_no, a7
	ld a7, 0(a7)
	ecall
	ret

ecall_no:
	.long 0
