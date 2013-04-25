.equ BOTPAK,	0x00280000
.equ DSKCAC,	0x00100000
.equ DSKCAC0,	0x00008000

.equ VBEMODE,	0x101		# 640 * 480 * 8bit color

.equ CYLS,		0x0ff0
.equ LEDS,		0x0ff1
.equ VMODE,		0x0ff2
.equ SCRNX,		0x0ff4
.equ SCRNY,		0x0ff6
.equ VRAM,		0x0ff8

.text
.code16
screen_mode:
	# VBEの存在を確認
	movw $0x9000, %ax
	movw %ax, %es
	movw $0x00, %di
	movw $0x4f00, %ax
	int $0x10
	cmpw $0x004f, %ax
	jne scrn320

	# VBEのバージョンを確認
	movw %es:4(%di), %ax
	cmpw $0x0200, %ax
	jb scrn320

	# 画面モード情報の取得
	movw $VBEMODE, %cx
	movw $0x4f01, %ax
	int $0x10

	# 画面情報の確認
	cmpb $0x08, %es:0x19(%di)
	jne scrn320
	cmpb $0x04, %es:0x1b(%di)
	jne scrn320
	movw %es:0x00(%di), %ax
	andw $0x0080, %ax
	jz scrn320

	# 画面モードの切り替え
	movw $VBEMODE+0x4000, %bx
	movw $0x4f02, %ax
	int $0x10

	movb $0x08, (VMODE)
	movw %es:0x12(%di), %ax
	movw %ax, (SCRNX)
	movw %es:0x14(%di), %ax
	movw %ax, (SCRNY)
	movl %es:0x28(%edi), %eax
	movl %eax, (VRAM)

	jmp head

scrn320:
	movb $0x13, %al
	movb $0x00, %ah
	int $0x10

	movb $0x08, (VMODE)
	movw $320, (SCRNX)
	movw $200, (SCRNY)
	movl $0x000a0000, (VRAM)

head:
	movb $0x02, %ah
	int $0x16
	movb %al, (LEDS)

# 32ビットプロテクトモードへ移行
	# PICへの割り込み禁止
	movb $0xff, %al
	outb %al, $0x21
	nop
	outb %al, $0xa1
	cli							# CPUレベルでも割り込み禁止

	# A20信号線をON
	call waitkbdout
	movb $0xd1, %al
	outb %al, $0x64
	call waitkbdout
	movb $0xdf, %al
	outb %al, $0x60				# enable A20
	call waitkbdout

.arch i486						# 32bitネイティブコード
	lgdtl (GDTR0)				# 暫定GDTを設定
	movl %cr0, %eax
	andl $0x7fffffff, %eax		# bit31を0に(ページング禁止)
	orl $0x00000001, %eax		# bit0を1に(プロテクトモード移行)
	movl %eax, %cr0
	jmp pipelineflash
pipelineflash:
	movw $1*8, %ax				# 読み書き可能セグメント32bit
	movw %ax, %ds
	movw %ax, %es
	movw %ax, %fs
	movw %ax, %gs
	movw %ax, %ss

# bootpackの転送
	movl $bootpack, %esi
	movl $BOTPAK, %edi
	movl $512*1024/4, %ecx
	call memcpy

# ブートセクタの転送
	movl $0x7c00, %esi
	movl $DSKCAC, %edi
	movl $512/4, %ecx
	call memcpy

# 残り
	movl $DSKCAC0+512, %esi
	movl $DSKCAC+512, %edi
	movl $0x00, %ecx
	movb (CYLS), %cl
	imull $512*18*2/4, %ecx
	subl $512/4, %ecx
	call memcpy

# start bootpack
	movl $BOTPAK, %ebx
	movl $0x11a8, %ecx
	addl $3, %ecx
	shrl $2, %ecx
	jz skip
	movl $0x10c8, %esi
	addl %ebx, %esi
	movl $0x00310000, %edi
	call memcpy
skip:
	movl $0x00310000, %esp
	ljmpl $2*8, $0x00000000

###########################
# function

waitkbdout:
	inb $0x64, %al
	andb $0x02, %al
	inb $0x60, %al
	jnz waitkbdout
	ret

memcpy:
	movl (%esi), %eax
	addl $4, %esi
	movl %eax, (%edi)
	addl $4, %edi
	subl $1, %ecx
	jnz memcpy
	ret

##########################
# GDT
.align 8
GDT0:
	.skip 8, 0x00							# ヌルセレクタ
	.word 0xffff, 0x0000, 0x9200, 0x00cf	# 読み書き可能セグメント32bit
	.word 0xffff, 0x0000, 0x9a28, 0x0047	# 実行可能セグメント32bit

	.word 0x0000
GDTR0:
	.word 8*3-1
	.int GDT0

.align 8
bootpack:
