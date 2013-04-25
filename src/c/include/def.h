#ifndef __DEF_H__
#define __DEF_H__

#include "types.h"

// 基本定数
#define NULL		0x00

// カラーナンバー
#define COLORN		16
#define COL_000000	0
#define COL_FF0000	1
#define COL_00FF00	2
#define COL_FFFF00	3
#define COL_0000FF	4
#define COL_FF00FF	5
#define COL_00FFFF	6
#define COL_FFFFFF	7
#define COL_C6C6C6	8
#define COL_840000	9
#define COL_008400	10
#define COL_848400	11
#define COL_000084	12
#define COL_840084	13
#define COL_008484	14
#define COL_848484	15

// アドレス
#define ADR_IDT			0x0026f800
#define LIMIT_IDT		0x000007ff
#define ADR_GDT			0x00270000
#define LIMIT_GDT		0x0000ffff
#define ADR_BOTPAK		0x00280000
#define LIMIT_BOTPAK	0x0007ffff
#define ADR_MEMMAN		0x003c0000

// 属性
#define AR_DATA32_RW	0x4092
#define AR_CODE32_ER	0x409a
#define AR_INTGATE32	0x008e
#define AR_TSS32		0x0089

// ポート番号
#define PIC0_ICW1		0x0020
#define PIC0_OCW2		0x0020
#define PIC0_IMR		0x0021
#define PIC0_ICW2		0x0021
#define PIC0_ICW3		0x0021
#define PIC0_ICW4		0x0021
#define PIC1_ICW1		0x00a0
#define PIC1_OCW2		0x00a0
#define PIC1_IMR		0x00a1
#define PIC1_ICW2		0x00a1
#define PIC1_ICW3		0x00a1
#define PIC1_ICW4		0x00a1

#define PORT_KEYDAT		0x0060
#define PORT_KEYSTA		0x0064
#define PORT_KEYCMD		0x0064

// データ定数
#define KEYSTA_SEND_NOTREADY	0x02
#define KEYCMD_WRITE_MODE		0x60
#define KBC_MODE				0x47
#define KEYCMD_SENDTO_MOUSE		0xd4
#define MOUSECMD_ENABLE			0xf4

// レジスタ用マスク
#define EFLAGS_AC_BIT		0x00040000
#define CR0_CACHE_DISABLE	0x60000000

// フラグ定数
#define FLAGS_OVERRUN		0x0001

// 下敷き用定数
#define SHEET_USE	1

// 割り込み識別値
#define INT_KEYBOARD		0x100
#define INT_MOUSE			0x200

// 色パレット
extern const byte table_rgb[16 * 3];
// キー情報
extern const char keytable[0x54];
// カーソルデータ
extern const char _cursor[16][16];
// 半角英数字(ASCII)
extern char hankaku[4096];
// ブート情報
extern BOOTINFO *binfo;
// タイマ管理構造体
extern TIMER_CTL timerctl;

#endif // __DEF_H__
