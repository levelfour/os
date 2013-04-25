.code32
.extern hInt20, hInt21, hInt2c

.globl inthandler20, inthandler21, inthandler2c

.text
inthandler20:
	pushw %es
	pushw %ds
	pusha
	movl %esp, %eax
	pushl %eax
	movw %ss, %ax
	movw %ax, %ds
	movw %ax, %es
	call hInt20
	popl %eax
	popa
	popw %ds
	popw %es
	iret

inthandler21:
	pushw %es
	pushw %ds
	pusha
	movl %esp, %eax
	pushl %eax
	movw %ss, %ax
	movw %ax, %ds
	movw %ax, %es
	call hInt21
	popl %eax
	popa
	popw %ds
	popw %es
	iret

inthandler2c:
	pushw %es
	pushw %ds
	pusha
	movl %esp, %eax
	pushl %eax
	movw %ss, %ax
	movw %ax, %ds
	movw %ax, %es
	call hInt2c
	popl %eax
	popa
	popw %ds
	popw %es
	iret
