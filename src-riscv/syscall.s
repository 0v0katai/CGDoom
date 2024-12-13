.macro DEF_SYSCALL NAME, CODE
.global \NAME
\NAME:
	li a7, \CODE
	ecall
	ret
.endmacro

DEF_SYSCALL(Bdisp_PutDisp_DD, 1001)
DEF_SYSCALL(CGD_ScanKeyboard, 1002)
DEF_SYSCALL(Getkey,           1003)
