#ifndef __TYPE_H__
#define __TYPE_H__

// type
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;

// ブート情報
typedef struct __BOOTINFO__ {
	char cyls, leds, vmode, reserve;
	short scrnx, scrny;
	char *vram;
} __attribute__((__packed__)) BOOTINFO;

// セグメントデスクリプタ
typedef struct __SEGMENT_DESCRIPTOR__ {
	short limit_low, base_low;
	char base_mid, access_right;
	char limit_high, base_high;
} __attribute__((__packed__)) SEGMENT_DESCRIPTOR;

// ゲートデスクリプタ
typedef struct __GATE_DESCRIPTOR__ {
	short offset_low, selector;
	char dw_count, access_right;
	short offset_high;
} __attribute__((__packed__)) GATE_DESCRIPTOR;

// FIFOバッファ
typedef struct __FIFO32__ {
	int *buf;
	int next_w, next_r, size, free, flags;
} QUEUE;

// マウス解読用バッファ
typedef struct __MOUSE_DEC__ {
	byte buf[3], phase;
	int x, y, button;
} MOUSE_DEC;

// 空き情報管理
typedef struct __FREE_INFO__ {
	dword address, size;
} FREEINFO;

// メモリマネージャの最大記憶容量
#define MEMMAN_FREES		4090
// メモリマネージャ構造体
typedef struct __MEMMAN__ {
	int frees, maxfrees, lostsize, losts;
	FREEINFO free[MEMMAN_FREES];
} MEMORY_MANAGE;

// 下敷き構造体
struct __SHEET_CTL__;
typedef struct {
	byte *buf;
	int bxsize, bysize, vx0, vy0, col_inv, height, flags;
	struct __SHEET_CTL__ *ctl;
} SHEET;

// 下敷きの枚数の最大値
#define MAX_SHEETS	256
// 下敷き管理構造体
typedef struct __SHEET_CTL__ {
	byte *vram, *map;
	int xsize, ysize, top;
	SHEET *sheets[MAX_SHEETS];
	SHEET sheets0[MAX_SHEETS];
} SHEET_CTL;

// タイマ構造体
typedef struct __TIMER__ {
	struct __TIMER__ *next;
	dword timeout, flags;
	QUEUE *queue;
	int data;
} TIMER;

// タイマ管理構造体
#define MAX_TIMERS	500
typedef struct {
	dword count, next;
	TIMER *t0;
	TIMER timers0[MAX_TIMERS];
} TIMER_CTL;

#endif // __TYPE_H__
