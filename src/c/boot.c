#include "def.h"
#include "prot.h"

#define ADR_BOOTINFO		0x0ff0

#define WIN_XS				160
#define WIN_YS				52

// ブート情報アドレス
BOOTINFO *binfo = (BOOTINFO *)ADR_BOOTINFO;
// 割り込み用バッファ
QUEUE qInt;

void make_window(byte *, int, int, char *);
void make_textbox(SHEET *, int, int, int, int, int);
void putfont_sheet(SHEET *, int, int, int, int, char *, int);

typedef struct __TSS32__ {
	int backlink, esp0, ss0, esp1, ss1, esp2, ss2, cr3;
	int eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
	int es, cs, ss, ds, fs, gs;
	int ldtr, iomap;
} TSS32;

void task_b_main() {
	while(1) { io_hlt(); }
}


void Main() {
	char s[30];
	int int_buf[128];
	int i;
	int mx = (binfo->scrnx-16)/2, my = (binfo->scrny-28-16)/2;
	int cursor_x, cursor_c;
	int task_b_esp;	// stack address of task b
	MOUSE_DEC mdec;
	dword memtotal;
	MEMORY_MANAGE *memman = (MEMORY_MANAGE *)ADR_MEMMAN;
	SHEET_CTL *sheetctl;
	SHEET *sheet_back, *sheet_mouse, *sheet_win;
	byte *buf_back, *buf_mouse, *buf_win;
	TIMER *timer1, *timer2, *timer3;
	TSS32 tss_a, tss_b;

	init_gdtidt();	// GDT/IDTの初期化
	init_pic();		// PICの初期化
	io_sti();		// 割り込み許可

	// 割り込み用QUEUEの初期化
	queue_init(&qInt, 128, int_buf);

	init_pit();		// PITの初期化
	init_keyboard(&qInt);
	enable_mouse(&qInt, &mdec);

	io_out8(PIC0_IMR, 0xf8);	// PITとPIC1とキーボードを許可(0xf9)
	io_out8(PIC1_IMR, 0xef);	// マウスを許可

	// タイマ設定
	timer1 = timer_alloc();
	timer_init(timer1, &qInt, 10);
	timer_settime(timer1, 1000);
	timer2 = timer_alloc();
	timer_init(timer2, &qInt, 3);
	timer_settime(timer2, 300);
	timer3 = timer_alloc();
	timer_init(timer3, &qInt, 1);
	timer_settime(timer3, 50);

	// メモリ管理
	memtotal = memtest(0x00400000, 0xbfffffff);
	mem_init(memman);
	mem_free(memman, 0x00001000, 0x0009e000);
	mem_free(memman, 0x00400000, memtotal- 0x00400000);

	init_palete();	// パレットの初期化

	// 下敷き管理
	sheetctl = sheetctl_init(memman, binfo->vram, binfo->scrnx, binfo->scrny);
	if(!sheetctl) return;
	sheet_back	= sheet_alloc(sheetctl);
	sheet_mouse	= sheet_alloc(sheetctl);
	sheet_win	= sheet_alloc(sheetctl);
	buf_back	= (byte*)mem_alloc_4k(memman, binfo->scrnx * binfo->scrny);
	buf_win		= (byte*)mem_alloc_4k(memman, WIN_XS * WIN_YS);
	sheet_setbuf(sheet_back, buf_back, binfo->scrnx, binfo->scrny, -1);
	sheet_setbuf(sheet_mouse, buf_mouse, 16, 16, 99);
	sheet_setbuf(sheet_win, buf_win, WIN_XS, WIN_YS, -1);

	init_screen(buf_back, binfo->scrnx, binfo->scrny);
	init_mouse_cursor(buf_mouse, 99);

	make_window(buf_win, WIN_XS, WIN_YS, "window");
	make_textbox(sheet_win, 8, 28, 144, 16, COL_FFFFFF);
	cursor_x = 8;
	cursor_c = COL_FFFFFF;

	sheet_slide(sheet_back, 0, 0);
	sheet_slide(sheet_mouse, mx, my);
	sheet_slide(sheet_win, 80, 72);	
	sheet_updown(sheet_back, 0);
	sheet_updown(sheet_win, 1);
	sheet_updown(sheet_mouse, 2);

	lsprintf(s, "memory %dMB   free : %dKB", memtotal / (1024*1024),
				mem_total(memman) / 1024);
	putfont_sheet(sheet_back, 0, 32, COL_FFFFFF, COL_008484, s, 40);

	int t = (int)binfo->vram;
	lsprintf(s, "VRAM = 0x%X", t);
	putfont_sheet(sheet_back, 0, 0, COL_FFFFFF, COL_008484, s, 40);

	// init tss
	tss_a.ldtr = 0;
	tss_a.iomap - 0x40000000;
	tss_b.ldtr = 0;
	tss_b.iomap = 0x40000000;

	// set tss to segment descriptor
	SEGMENT_DESCRIPTOR *gdt = (SEGMENT_DESCRIPTOR*)ADR_GDT;
	set_segmdesc(gdt+3, 103, (int)&tss_a, AR_TSS32);
	set_segmdesc(gdt+4, 103, (int)&tss_b, AR_TSS32);

	// load tss
	load_tr(3*8);
	task_b_esp = mem_alloc_4k(memman, 64*1024) + 64*1024;
	tss_b.eip = (int)&task_b_main;
	tss_b.eflags = 0x00000202; // IF = 1;
	tss_b.eax = 0;
	tss_b.ecx = 0;
	tss_b.edx = 0;
	tss_b.ebx = 0;
	tss_b.esp = task_b_esp;
	tss_b.ebp = 0;
	tss_b.esi = 0;
	tss_b.edi = 0;
	tss_b.es = 1*8;
	tss_b.cs = 2*8;
	tss_b.ss = 1*8;
	tss_b.ds = 1*8;
	tss_b.fs = 1*8;
	tss_b.gs = 1*8;



	while(1) {
		io_cli();
		// 割り込みバッファからデータを受け取っていない
		if(queue_status(&qInt) == 0) {
			io_stihlt();
		} else {
			i = queue_get(&qInt);
			io_sti();
			// キーボードからデータを受信
			if(INT_KEYBOARD <= i && i <= INT_KEYBOARD + 255) {
				lsprintf(s, "%X", i - INT_KEYBOARD);
				putfont_sheet(sheet_back, 0, 16, COL_FFFFFF, COL_008484, s, 2);
				if(i < INT_KEYBOARD + 0x54) {
					if(keytable[i - INT_KEYBOARD] != 0 && cursor_x < 144) {
						s[0] = keytable[i - INT_KEYBOARD];
						s[1] = 0;
						putfont_sheet(sheet_win, cursor_x, 28, COL_000000, COL_FFFFFF, s, 1);
						cursor_x += 8;
					}
				}
				
				// バックスペース
				if(i == INT_KEYBOARD + 0x0e && cursor_x > 8) {
					putfont_sheet(sheet_win, cursor_x, 28, COL_000000, COL_FFFFFF, " ", 1);
					cursor_x -= 8;
				}

				// カーソルの表示
				boxfill(sheet_win->buf, sheet_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sheet_win, cursor_x, 28, cursor_x + 8, 44);

			// マウスかデータを受信
			} else if(INT_MOUSE <= i && i <= INT_MOUSE + 255) {
				// マウスデータが3バイト揃う
				if(mouse_decode(&mdec, i - INT_MOUSE)) {
					// データの表示
					lsprintf(s, "[lcr] x=%d y=%d", mdec.x, mdec.y);
					if((mdec.button & 0x01) != 0) s[1] = 'L';
					if((mdec.button & 0x02) != 0) s[3] = 'R';
					if((mdec.button & 0x03) != 0) s[2] = 'C';
					putfont_sheet(sheet_back, 32, 16, COL_FFFFFF, COL_008484, s, 30);

					// マウスの移動
					mx += mdec.x; my += mdec.y;
					if(mx < 0) mx = 0;
					if(my < 0) my = 0;
					if(mx > binfo->scrnx - 1) mx = binfo->scrnx - 1;
					if(my > binfo->scrny - 1) my = binfo->scrny - 1;
					sheet_slide(sheet_mouse, mx, my);

					// 左ボタンを押していたら、sheet_winを動かす
					if((mdec.button & 0x01) != 0)
						sheet_slide(sheet_win, mx - 80, my - 8);
				}
			} else if(i == 10) {
				putfont_sheet(sheet_back, 0, 64, COL_FFFFFF, COL_008484, "10[sec]", 7);
				taskswitch4();
			} else if(i == 3) {
				putfont_sheet(sheet_back, 0, 80, COL_FFFFFF, COL_008484, "3[sec]", 6);
			} else if(i <= 1) {
				if(i != 0) {
					timer_init(timer3, &qInt, 0);
					cursor_c = COL_000000;
				} else {
					timer_init(timer3, &qInt, 1);
					cursor_c = COL_FFFFFF;
				}
				timer_settime(timer3, 50);
				boxfill(sheet_win->buf, sheet_win->bxsize, cursor_c, cursor_x, 28, cursor_x + 7, 43);
				sheet_refresh(sheet_win, cursor_x, 28, cursor_x + 8, 44);
			}
		}
	}
}

void make_window(byte * buf, int xsize, int ysize, char *title) {
	static char closebtn[14][16] = {
		"OOOOOOOOOOOOOOO@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQQQ@@QQQQQ$@",
		"OQQQQQ@@@@QQQQ$@",
		"OQQQQ@@QQ@@QQQ$@",
		"OQQQ@@QQQQ@@QQ$@",
		"OQQQQQQQQQQQQQ$@",
		"OQQQQQQQQQQQQQ$@",
		"O$$$$$$$$$$$$$$@",
		"@@@@@@@@@@@@@@@@"
	};
	int x, y;
	char c;
	boxfill(buf, xsize, COL_C6C6C6, 0,		0,		xsize-1,0		);
	boxfill(buf, xsize, COL_FFFFFF, 1,		1,		xsize-2,1		);
	boxfill(buf, xsize, COL_C6C6C6, 0,		0,		0,		ysize-1	);
	boxfill(buf, xsize, COL_FFFFFF, 1,		1,		1,		ysize-2	);
	boxfill(buf, xsize, COL_848484, xsize-2,1,		xsize-2,ysize-2	);
	boxfill(buf, xsize, COL_000000, xsize-1,0,		xsize-1,ysize-1	);
	boxfill(buf, xsize, COL_C6C6C6, 2,		2,		xsize-3,ysize-3	);
	boxfill(buf, xsize, COL_000084, 3,		3,		xsize-4,20		);
	boxfill(buf, xsize, COL_848484, 1,		ysize-2,xsize-1,ysize-2	);
	boxfill(buf, xsize, COL_000000, 0,		ysize-1,xsize-1,ysize-1	);

	vprint(buf, title, xsize, 24, 4, COL_FFFFFF);

	for(y = 0; y < 14; y++)
	for(x = 0; x < 16; x++) {
		c = closebtn[y][x];
		switch(c) {
			case '@':
				c = COL_000000; break;
			case '$':
				c = COL_848484; break;
			case 'Q':
				c = COL_C6C6C6; break;
			default:
				c = COL_FFFFFF;
		}
		buf[(5+y)*xsize + (xsize-21+x)] = c;
	}
}

void make_textbox(SHEET *sheet, int x0, int y0, int sx, int sy, int c) {
	int x1 = x0 + sx, y1 = y0 + sy;
	boxfill(sheet->buf,sheet->bxsize,COL_848484,x0-2,y0-3,x1+1,y0-3);
	boxfill(sheet->buf,sheet->bxsize,COL_848484,x0-3,y0-3,x0-3,y1+1);
	boxfill(sheet->buf,sheet->bxsize,COL_FFFFFF,x0-3,y1+2,x1+1,y1+2);
	boxfill(sheet->buf,sheet->bxsize,COL_FFFFFF,x1+2,y0-3,x1+2,y1+2);
	boxfill(sheet->buf,sheet->bxsize,COL_000000,x0-1,y0-2,x1+0,y0-2);
	boxfill(sheet->buf,sheet->bxsize,COL_000000,x0-2,y0-2,x0-2,y1+0);
	boxfill(sheet->buf,sheet->bxsize,COL_C6C6C6,x0-2,y1+1,x1+0,y1+1);
	boxfill(sheet->buf,sheet->bxsize,COL_C6C6C6,x1+1,y0-2,x1+1,y1+1);
	boxfill(sheet->buf,sheet->bxsize,c         ,x0-1,y0-1,x1+0,y1+0);
}

void putfont_sheet(SHEET * sheet, int x, int y, int c, int b, char *s, int l) {
	boxfill(sheet->buf, sheet->bxsize, b, x, y, x + l * 8 -1, y + 15);
	vprint(sheet->buf, s, sheet->bxsize, x, y, c);
	sheet_refresh(sheet, x, y, x + l * 8, y + 16);
}
