#include "def.h"
#include "prot.h"

SHEET_CTL *sheetctl_init(MEMORY_MANAGE* man, byte *vram,  int xsize, int ysize) {
	SHEET_CTL *ctl;
	int i;
	ctl = (SHEET_CTL*)mem_alloc_4k(man, sizeof(SHEET_CTL));
	if(ctl == 0) return 0;

	ctl->map = (byte*)mem_alloc_4k(man, xsize * ysize);
	if(ctl->map == 0) {
		mem_free_4k(man, (int)ctl, sizeof(SHEET_CTL));
		return 0;
	}

	ctl->vram = vram;
	ctl->xsize = xsize;
	ctl->ysize = ysize;
	ctl->top = -1;	// シートは1枚もない
	for(i = 0; i < MAX_SHEETS; i++) {
		ctl->sheets0[i].flags = 0;	// 未使用マーク
		ctl->sheets0[i].ctl = ctl;	// 所属を記録
	}

	return ctl;
}

SHEET *sheet_alloc(SHEET_CTL *ctl) {
	SHEET *sheet;
	int i;
	for(i = 0; i < MAX_SHEETS; i++) {
		if(ctl->sheets0[i].flags == 0) {
			sheet = &ctl->sheets0[i];
			sheet->flags = SHEET_USE;
			sheet->height = -1;	// 非表示中
			return sheet;
		}
	}
	return 0;	// すべてのシートが使用中だった
}

void sheet_setbuf(SHEET *sheet, byte *buf, int xsize, int ysize, int col_inv) {
	sheet->buf		= buf;
	sheet->bxsize	= xsize;
	sheet->bysize	= ysize;
	sheet->col_inv	= col_inv;
}

void sheet_updown(SHEET *sheet, int height) {
	SHEET_CTL *ctl = sheet->ctl;
	int h, old = sheet->height;	// 設定前の高さを記憶する

	// 指定が低すぎや高すぎだったら修正する
	if(height > ctl->top + 1)	height = ctl->top + 1;
	if(height < -1)				height = -1;
	sheet->height = height;	// 高さを設定

	// 以下は主にsheets[]の並べ替え
	if(old > height) {	// 以前よりも低くなる
		if(height >= 0) {
			// 間のものを引き上げる
			for(h = old; h > height; h--) {
				ctl->sheets[h] = ctl->sheets[h-1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sheet;
			sheet_refreshmap(ctl, sheet->vx0, sheet->vy0, sheet->vx0 + sheet->bxsize, sheet->vy0 + sheet->bysize, height + 1);
			sheet_refresh_sub(ctl, sheet->vx0, sheet->vy0, sheet->vx0 + sheet->bxsize, sheet->vy0 + sheet->bysize, height + 1, old);
	} else {	// 非表示化
			if(ctl->top > old) {
				// 上になっているものをおろす
				for(h = old; h < ctl->top; h++) {
					ctl->sheets[h] = ctl->sheets[h+1];
					ctl->sheets[h]->height = h;
				}
			}
			ctl->top--;	// 表示中の下敷きが1つ減るので一番上の高さが減る
			sheet_refreshmap(ctl, sheet->vx0, sheet->vy0, sheet->vx0 + sheet->bxsize, sheet->vy0 + sheet->bysize, 0);
			sheet_refresh_sub(ctl, sheet->vx0, sheet->vy0, sheet->vx0 + sheet->bxsize, sheet->vy0 + sheet->bysize, 0, old - 1);
	}
	} else if(old < height) {	// 以前よりも高くなる
		if(old >= 0) {
			// 間のものを押し下げる
			for(h = old; h < height; h++) {
				ctl->sheets[h] = ctl->sheets[h-1];
				ctl->sheets[h]->height = h;
			}
			ctl->sheets[height] = sheet;
		} else {	// 表示状態へ
			// 上になるものを持ち上げる
			for(h = ctl->top; h >= height; h--) {
				ctl->sheets[h+1] = ctl->sheets[h];
				ctl->sheets[h+1]->height = h + 1;
			}
			ctl->sheets[height] = sheet;
			ctl->top++;	// 表示中の下敷きが1つ増えるので一番上の高さが増える
		}
		sheet_refreshmap(ctl, sheet->vx0, sheet->vy0, sheet->vx0 + sheet->bxsize, sheet->vy0 + sheet->bysize, height);
		sheet_refresh_sub(ctl, sheet->vx0, sheet->vy0, sheet->vx0 + sheet->bxsize, sheet->vy0 + sheet->bysize, height, height);
	}
	return;
}

void sheet_refresh_sub(SHEET_CTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0, int h1) {
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	byte *buf, c, *vram = ctl->vram, *map = ctl->map, sid;
	SHEET *sheet;

	if(vx0 < 0) vx0 = 0;
	if(vy0 < 0) vy0 = 0;
	if(vx1 > ctl->xsize) vx1 = ctl->xsize;
	if(vy1 > ctl->ysize) vy1 = ctl->ysize;

	for(h = h0; h <= h1; h++) {
		sheet = ctl->sheets[h];
		buf = sheet->buf;
		sid = sheet - ctl->sheets0;

		bx0 = vx0 - sheet->vx0;
		by0 = vy0 - sheet->vy0;
		bx1 = vx1 - sheet->vx0;
		by1 = vy1 - sheet->vy0;

		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sheet->bxsize) bx1 = sheet->bxsize;
		if(by1 > sheet->bysize) by1 = sheet->bysize;

		for(by = by0; by < by1; by++) {
			vy = sheet->vy0 + by;
			for(bx = bx0; bx < bx1; bx++) {
				vx = sheet->vx0 + bx;
				if(map[vy*ctl->xsize + vx] == sid)
					vram[vy*ctl->xsize+vx] = buf[by*sheet->bxsize+bx];
			}
		}
	}
}

void sheet_refresh(SHEET *sheet, int bx0, int by0, int bx1, int by1) {
	if(sheet->height >= 0) {
		sheet_refresh_sub(sheet->ctl, sheet->vx0 + bx0, sheet->vy0 + by0, sheet->vx0 + bx1, sheet->vy0 + by1, sheet->height, sheet->height);
	}
}

void sheet_refreshmap(SHEET_CTL *ctl, int vx0, int vy0, int vx1, int vy1, int h0) {
	int h, bx, by, vx, vy, bx0, by0, bx1, by1;
	byte *buf, sid, *map = ctl->map;
	SHEET *sheet;
	if(vx0 < 0) vx0 = 0;
	if(vy0 < 0) vy0 = 0;
	if(vx1 > ctl->xsize) vx1 = ctl->xsize;
	if(vy1 > ctl->ysize) vy1 = ctl->ysize;
	for(h = h0; h <= ctl->top; h++) {
		sheet	= ctl->sheets[h];
		sid		= sheet - ctl->sheets0;	// 番地を引き算して下敷き番号とする
		buf		= sheet->buf;
		bx0		= vx0 - sheet->vx0;
		by0		= vy0 - sheet->vy0;
		bx1		= vx1 - sheet->vx0;
		by1		= vy1 - sheet->vy0;
		if(bx0 < 0) bx0 = 0;
		if(by0 < 0) by0 = 0;
		if(bx1 > sheet->bxsize) bx1 = sheet->bxsize;
		if(by1 > sheet->bysize) by1 = sheet->bysize;
		for(by = by0; by < by1; by++) {
			vy = sheet->vy0 + by;
			for(bx = bx0; bx < bx1; bx++) {
				vx = sheet->vx0 + bx;
				if(buf[by*sheet->bxsize+bx] != sheet->col_inv)
					map[vy*ctl->xsize+vx] = sid;
			}
		}
	}
}

void sheet_slide(SHEET *sheet, int vx0, int vy0) {
	SHEET_CTL *ctl = sheet->ctl;
	int old_vx0 = sheet->vx0, old_vy0 = sheet->vy0;
	sheet->vx0 = vx0;
	sheet->vy0 = vy0;
	if(sheet->height >= 0) { // 表示中なら
		sheet_refreshmap(ctl, old_vx0, old_vy0, old_vx0 + sheet->bxsize, old_vy0 + sheet->bysize, 0);
		sheet_refreshmap(ctl, vx0, vy0, vx0 + sheet->bxsize, vy0 + sheet->bysize, sheet->height);
		sheet_refresh_sub(ctl, old_vx0, old_vy0, old_vx0 + sheet->bxsize, old_vy0 + sheet->bysize, 0, sheet->height - 1);
		sheet_refresh_sub(ctl, vx0, vy0, vx0 + sheet->bxsize, vy0 + sheet->bysize, sheet->height, sheet->height);
}
}

void sheet_free(SHEET *sheet) {
	if(sheet->height >= 0) {	// 表示中なら
		sheet_updown(sheet, -1);	// 非表示にする
	}
	sheet->flags = 0;	// 未使用マーク
}
