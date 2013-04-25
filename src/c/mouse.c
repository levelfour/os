#include "def.h"
#include "prot.h"

QUEUE *qMouse;

// interrupt from PS/2 mouse
void hInt2c(int *esp) {
	int data;
	io_out8(PIC1_OCW2, 0x64);	// IRQ-12割り込み受付完了の通知
	io_out8(PIC0_OCW2, 0x62);	// IRQ-2割り込み受付完了の通知

	data = io_in8(PORT_KEYDAT);
	queue_put(qMouse, data + INT_MOUSE);
	return;
}

// マウスの有効化
void enable_mouse(QUEUE *queue, MOUSE_DEC *mdec) {
	qMouse = queue;
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
	// 成功すればACK(0xfa)を返却
	mdec->phase = 0;
}

int mouse_decode(MOUSE_DEC *mdec, byte dat) {
	if(mdec->phase == 0) {
		if(dat == 0xfa) mdec->phase = 1;
		return 0;
	}
	
	if(mdec->phase == 1) {
		if((dat & 0xc8) == 0x08) {
			mdec->buf[0] = dat;
			mdec->phase = 2;
		}
		return 0;
	}
	
	if(mdec->phase == 2) {
		mdec->buf[1] = dat;
		mdec->phase = 3;
		return 0;
	}
	
	if(mdec->phase == 3) {
		mdec->buf[2] = dat;
		mdec->phase = 1;
		mdec->button = mdec->buf[0] & 0x07;
		mdec->x = mdec->buf[1];
		mdec->y = mdec->buf[2];
		if((mdec->buf[0] & 0x10) != 0) {
			mdec->x |= 0xffffff00;
		}
		if((mdec->buf[0] & 0x20) != 0) {
			mdec->y |= 0xffffff00;
		}
		mdec->y = - mdec->y;
		return 1;
	}

	return -1;
}
