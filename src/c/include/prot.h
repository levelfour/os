#ifndef __PROT_H__
#define __PROT_H__

#include "types.h"

// boot.c
// Main関数からOSを実行
void Main();

// graphic.c
// グラフィック関連の関数群
void draw_mouse(int, int);
void init_palete();
void boxfill(byte *, int, byte, int, int, int, int);
void init_screen(byte *, const int, const int);
void vprint(byte *, byte *, int, int, int, char);
void init_mouse_cursor(char *, char);
void putblock(byte *, int, int, int, int, int, const char *, int);
//void set_palete(int, int, unsigned char *);
//void putfont(byte *, int, int, int, char, char *);

// dsctbl.c
// GDT,IDT等のデスクリプタテーブル関連
void init_gdtidt();
//void set_segmdesc(SEGMENT_DESCRIPTOR *, dword int, int, int);
//void set_gatedesc(GATE_DESCRIPTOR *, int, int, int);

// int.c
// 割り込み関連の関数群
void init_pic();

// lib.c
// 標準ライブラリ的関数群
int lsprintf(char *str, const char *format, ...);
void strcls(char *);
//void int2hex(char *, int, int);
//void int2dec(char *, int);

// fifo.c
// QUEUEバッファ利用関数群
void queue_init(QUEUE *, int, int *);
int queue_put(QUEUE *, int);
int queue_get(QUEUE *);
int queue_status(QUEUE *);

// memory.c
// メモリ管理関数群
dword memtest(dword, dword);
void mem_init(MEMORY_MANAGE *);
dword mem_total(MEMORY_MANAGE *);
dword mem_alloc(MEMORY_MANAGE *, dword);
int mem_free(MEMORY_MANAGE *, dword, dword);
dword mem_alloc_4k(MEMORY_MANAGE *, dword);
int mem_free_4k(MEMORY_MANAGE *, dword, dword);

// sheet.c
// 下敷き管理関数群
SHEET_CTL	*sheetctl_init(MEMORY_MANAGE *, byte *, int, int);
SHEET	*sheet_alloc(SHEET_CTL *);
void	sheet_setbuf(SHEET *, byte *, int, int, int);
void	sheet_updown(SHEET *, int);
void	sheet_refresh(SHEET *, int, int, int, int);
void	sheet_refresh_sub(SHEET_CTL *, int, int, int, int, int, int);
void	sheet_refreshmap(SHEET_CTL *, int, int, int, int, int);
void	sheet_slide(SHEET *, int, int);
void	sheet_free(SHEET *);

// keyboard.c
// キーボード関連関数群
void hInt21(int *);
void wait_KBC_sendready();
void init_keyboard(QUEUE *);

// mouse.c
// マウス関連関数群
void hInt2c(int *);
void enable_mouse(QUEUE *, MOUSE_DEC *);
int mouse_decode(MOUSE_DEC *, byte);

// timer.c
// タイマ関連関数群
void init_pit();
void hInt20(int *);
TIMER	*timer_alloc();
void	timer_free(TIMER *);
void	timer_init(TIMER *timer, QUEUE *, int);
void	timer_settime(TIMER *, dword);

// func.s 
// GASで記述された関数
dword memtest_sub(dword, dword);
void taskswitch4();
void taskswitch5();

// mnemonic.s
// GASで記述されたニーモニック関連関数
extern inline void	io_hlt();
extern inline void	io_cli();
extern inline void	io_sti();
extern inline void	io_stihlt();
extern inline int	io_in8();
extern inline int	io_in16();
extern inline int	io_in32();
extern inline void	io_out8();
extern inline void	io_out16();
extern inline void	io_out32();
extern inline int	io_load_eflags();
extern inline void	io_store_eflags(int eflags);
extern inline int	io_load_cr0();
extern inline void	store_cr0(int cr0);
extern inline void load_tr(int tr);

// int.s
// GASで記述された割り込み関連関数群
void inthandler20();
void inthandler21();
void inthandler2c();

#endif // __PROT_H__
