.equ CYLS, 10

.text
.code16
	jmp entry

	.byte 0x90
	.ascii "HELLOIPL"		# ブートセクタの名前
	.word 512				# 1セクタの大きさ
	.byte 1					# クラスタの大きさ
	.word 1					# FATがどこから始まるか
	.byte 2					# FATの個数
	.word 224				# ルートディレクトリのサイズ
	.word 2880				# このドライブの大きさ
	.byte 0xf0				# メディアのタイプ
	.word 9					# FAT領域の長さ
	.word 18				# 1トラックにいくつのセクタがあるか
	.word 2					# ヘッドの数
	.int 0					# 必ず0
	.int 2880				# ドライブのサイズ
	.byte 0, 0, 0x29
	.int 0xffffffff			# ボリュームシリアル番号
	.ascii "HELLO-OS   "	# ディスクの名前
	.ascii "FAT12   "		# フォーマットの名前
.skip 18, 0x00				# 18バイト空ける

# プログラム本体
entry:
	movw $0, %ax			# レジスタ初期化
	movw %ax, %ss
	movw $0x7c00, %sp
	movw %ax, %ds
	movw %ax, %es
	
	movw $0x0820, %ax
	movw %ax, %es
	movb $0x00, %ch			# シリンダ0
	movb $0x00, %dh			# ヘッド0
	movb $0x02, %cl			# セクタ2

readloop:
	movw $0x00, %si			# 失敗回数のカウンタ
retry:
	movb $0x02, %ah			# ディスク読み込み
	movb $0x01, %al			# 1セクタ
	xorw %bx, %bx
	movb $0x00, %dl			# Aドライブ
	int $0x13
	jnc next
	addw $0x01, %si
	cmpw $0x05, %si			# 5回失敗したらエラー
	jae error
	movb $0x00, %ah
	movb $0x00, %dl			# Aドライブ
	int $0x13				# ドライブのリセット
	jmp retry

next:
	movw %es, %ax
	addw $0x0020, %ax		# アドレスを0x0200進める
	movw %ax, %es
	addb $0x01, %cl
	cmpb $18, %cl			# 18セクタまで読み込む
	jbe readloop

	movb $0x01, %cl
	addb $0x01, %dh
	cmpb $0x02, %dh			# 裏ヘッダを読み込む
	jb readloop
	movb $0x00, %dh
	addb $0x01, %ch
	cmpb $CYLS, %ch			# CYLSシリンダまで読み込む
	jb readloop

_load_fin:
	movb $CYLS, (0x0ff0)

	jmp 0xc200

error:
	movw $load_err, %si
	call print
_error_fin:
	hlt
	jmp _error_fin

# %si = string adress
print:
	movb (%si), %al
	addw $1, %si
	cmpb $0, %al
	je _print_fin
	movb $0x0e, %ah			# 一文字表示BIOSコール
	movw $15, %bx			# カラーコード
	int $0x10				# ビデオBIOS呼び出し
	jmp print
_print_fin:
	ret

.data
load_err:	.string "load error\n"
