.code32
.globl io_hlt, io_cli, io_sti, io_stihlt
.globl io_load_eflags, io_store_eflags
.globl io_in8, io_in16, io_in32
.globl io_out8, io_out16, io_out32
.globl load_gdtr, load_idtr
.globl load_cr0, store_cr0
.globl load_tr

.text
#########################

# HLT
io_hlt:
	hlt
	ret

# CLI
io_cli:
	cli
	ret

# STI
io_sti:
	sti
	ret

# STI -> HLT
io_stihlt:
	sti
	hlt
	ret

# LOAD EFLAGS
io_load_eflags:
	pushf
	popl %eax
	ret

# STORE EFLAGS
io_store_eflags:
	movl 4(%esp), %eax
	pushl %eax
	popf
	ret

# IN ( 8bit )
io_in8:
	movl 4(%esp), %edx	# port
	xorl %eax, %eax
	inb %dx, %al
	ret

# IN ( 16bit )
io_in16:
	movl 4(%esp), %edx
	xorl %eax, %eax
	inw %dx, %ax
	ret

# IN ( 32bit )
io_in32:
	movl 4(%esp), %edx
	inl %dx, %eax
	ret

# OUT ( 8bit )
io_out8:
	movl 4(%esp), %edx
	movb 8(%esp), %al
	outb %al, %dx
	ret

# OUT ( 16bit )
io_out16:
	movl 4(%esp), %edx
	movl 8(%esp), %eax
	outw %ax, %dx
	ret

# OUT ( 32bit )
io_out32:
	movl 4(%esp), %edx
	movl 8(%esp), %eax
	outl %eax, %dx
	ret

# LGDT
load_gdtr:
	movw 4(%esp), %ax
	movw %ax, 6(%esp)
	lgdt 6(%esp)
	ret

# LIDT
load_idtr:
	movw 4(%esp), %ax
	movw %ax, 6(%esp)
	lidt 6(%esp)
	ret

# CR0
load_cr0:
	movl %cr0, %eax
	ret

store_cr0:
	movl 4(%esp), %eax
	movl %eax, %cr0
	ret

# TR
load_tr:
	ltr 4(%esp)
	ret
