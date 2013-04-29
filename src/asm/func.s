.code32

.globl memtest_sub
.globl taskswitch4, taskswitch5

.text
memtest_sub:
	pushl %edi
	pushl %esi
	pushl %ebx
	movl $0xaa55aa55, %esi		# pat0 = 0xaa55aa55;
	movl $0x55aa55aa, %edi		# pat1 = 0x55aa55aa;
	movl 16(%esp), %eax			# i = start;
mts_loop:
	movl %eax, %ebx
	addl $0xffc, %ebx			# p = i + 0xffc;
	movl (%ebx), %edx			# old = *p;
	movl %esi, (%ebx)			# *p = pat0;
	xorl $0xffffffff, (%ebx)	# *p ^= 0xffffffff;
	cmpl (%ebx), %edi			# if(*p != pat1) {
	jne mts_fin					# goto mts_ }
	movl %edx, (%ebx)			# *p = old;
	addl $0x1000, %eax			# i += 0x1000;
	cmpl 20(%esp), %eax			# if(i <= end) {
	jbe mts_loop				# goto mts_loop; }
	popl %ebx
	popl %esi
	popl %edi
	ret
mts_fin:
	movl %edx, (%ebx)			# *p = old;
	popl %ebx
	popl %esi
	popl %edi
	ret

taskswitch4:
	ljmpl $4*8, $0x00000000
	ret

taskswitch5:
	ljmpl $5*8, $0x00000000
	ret
