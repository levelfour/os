#include "prot.h"
#include "def.h"

void set_palete(int, int, const byte *);
void putfont(byte *, int, int, int, char, char *);

void init_palete() {
	set_palete(0, 15, table_rgb);
}

void boxfill(byte *vram, int xs, byte c, int x0, int y0, int x1, int y1) {
	int x, y;
	for( y = y0; y <= y1; y++ ) {
		for( x = x0; x <= x1; x++ ) vram[y * xs + x] = c;
	}
}

void init_screen(byte *vram, const int xsize, const int ysize) {
	boxfill(vram, xsize, COL_008484, 0, 0, xsize - 1, ysize - 29);
	boxfill(vram, xsize, COL_C6C6C6, 0, ysize - 28, xsize - 1, ysize - 28);
	boxfill(vram, xsize, COL_FFFFFF, 0, ysize - 27, xsize - 1, ysize - 27);
	boxfill(vram, xsize, COL_C6C6C6, 0, ysize - 26, xsize - 1, ysize - 1);

	boxfill(vram, xsize, COL_FFFFFF, 3, ysize - 24, 59, ysize - 24);
	boxfill(vram, xsize, COL_FFFFFF, 2, ysize - 24, 2, ysize - 4);
	boxfill(vram, xsize, COL_848484, 3, ysize - 4, 59, ysize - 4);
	boxfill(vram, xsize, COL_848484, 59, ysize - 23, 59, ysize - 5);
	boxfill(vram, xsize, COL_000000, 2, ysize - 3, 59, ysize - 3);
	boxfill(vram, xsize, COL_000000, 60, ysize - 24, 60, ysize - 3);

	boxfill(vram, xsize, COL_848484, xsize - 47, ysize - 24, xsize - 4, ysize - 24);
	boxfill(vram, xsize, COL_848484, xsize - 47, ysize - 23, xsize - 47, ysize - 4);
	boxfill(vram, xsize, COL_FFFFFF, xsize - 47, ysize - 3, xsize - 4, ysize - 3);
	boxfill(vram, xsize, COL_FFFFFF, xsize - 3, ysize - 24, xsize - 3, ysize - 3);
}

void vprint(byte *vram, byte *s, int xs, int x, int y, char c) {
	for(; *s != 0x00; s++ ) {
		putfont(vram, xs, x, y, c, hankaku + *s * 16);
		x += 8;
	}
	return;
}

void init_mouse_cursor(char *mouse, char bc) {
	int x, y;
	for( y = 0; y < 16; y++ )
	for( x = 0; x < 16; x++ ) {
		switch(_cursor[y][x]) {
		case '*':
			mouse[y*16+x] = COL_000000; break;
		case 'O':
			mouse[y*16+x] = COL_FFFFFF; break;
		case '.':
			mouse[y*16+x] = bc;			break;
		}
	}
}

void putblock(byte *vram, int vxsize, int pxsize, int pysize, int x0, int y0, const char *buf, int bxsize) {
	int x, y;
	for( y = 0; y < pysize; y++ )
	for( x = 0; x < pxsize; x++ ) {
		vram[(y0+y) * vxsize + (x0+x)] = buf[y * bxsize + x];
	}
}

void set_palete(int start, int end, const byte *rgb) {
	int i, eflags;
	eflags = io_load_eflags();
	io_cli();
	io_out8(0x03c8, start);
	for( i = start; i <= end; i++ ) {
		io_out8(0x03c9, rgb[0] / 4);
		io_out8(0x03c9, rgb[1] / 4);
		io_out8(0x03c9, rgb[2] / 4);
		rgb += 3;
	}
	io_store_eflags(eflags);
}

void putfont(byte *vram, int xsize, int x, int y, char c, char *font) {
	int i;
	char *p, d;
	for( i = 0; i < 16; i++ ) {
		p = vram + (y + i) * xsize + x;
		d = font[i];
		if((d & 0x80) != 0) { p[0] = c; }
		if((d & 0x40) != 0) { p[1] = c; }
		if((d & 0x20) != 0) { p[2] = c; }
		if((d & 0x10) != 0) { p[3] = c; }
		if((d & 0x08) != 0) { p[4] = c; }
		if((d & 0x04) != 0) { p[5] = c; }
		if((d & 0x02) != 0) { p[6] = c; }
		if((d & 0x01) != 0) { p[7] = c; }
	}
}
