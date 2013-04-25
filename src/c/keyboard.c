#include "def.h"
#include "prot.h"

QUEUE *qKeyboard;

// interrupt from PS/2 keyboard
void hInt21(int *esp) {
	int data;
	io_out8(PIC0_OCW2, 0x61);	// IRQ-01割り込み受付完了の通知

	data = io_in8(PORT_KEYDAT);	// キーボードからデータの取得
	queue_put(qKeyboard, data + INT_KEYBOARD);// FIFOにデータを書き込む

	return;
}

// キーボードコントローラがデータ送信可能になるのを待つ
void wait_KBC_sendready() {
	for(;;) {
		if((io_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) break;
	}
}

// キーボードコントローラの初期化
void init_keyboard(QUEUE *queue) {
	qKeyboard = queue;
	wait_KBC_sendready();
	io_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
	wait_KBC_sendready();
	io_out8(PORT_KEYDAT, KBC_MODE);
}
