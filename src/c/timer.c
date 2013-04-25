#include "def.h"
#include "prot.h"

TIMER_CTL timerctl;

#define PIT_CTRL	0x0043
#define PIT_CNT0	0x0040

#define TIMER_FLAGS_ALLOC	1	// 確保状態
#define TIMER_FLAGS_USING	2	// 動作中

void init_pit() {
	int i;
	TIMER *t;
	io_out8(PIT_CTRL, 0x34);
	io_out8(PIT_CNT0, 0x9c);
	io_out8(PIT_CNT0, 0x2e);
	timerctl.count = 0;
	for(i = 0; i < MAX_TIMERS; i++) {
		timerctl.timers0[i].flags = 0;	// 未使用マーク
	}

	// 番兵の作成
	t = timer_alloc();
	t->timeout = 0xffffffff;
	t->flags = TIMER_FLAGS_USING;
	t->next = 0;

	// 番兵の挿入
	timerctl.t0 = t;
	timerctl.next = 0xffffffff;
}

void hInt20(int *esp) {
	TIMER *timer;

	io_out8(PIC0_OCW2, 0x60);	// IRQ-00受付完了をPICに通知
	timerctl.count++;
	// まだ次の時刻になっていない
	if(timerctl.next > timerctl.count) return;

	timer = timerctl.t0;	// リストの先頭ノードを保存

	for(;;) {
		if(timer->timeout > timerctl.count) break;
		// タイムアウト
		timer->flags = TIMER_FLAGS_ALLOC;
		queue_put(timer->queue, timer->data);
		timer = timer->next;
	}
	timerctl.t0 = timer;
	timerctl.next = timerctl.t0->timeout;
}

TIMER *timer_alloc() {
	int i;
	for(i = 0; i < MAX_TIMERS; i++) {
		if(timerctl.timers0[i].flags == 0) {
			timerctl.timers0[i].flags = TIMER_FLAGS_ALLOC;
			return &timerctl.timers0[i];
		}
	}
	return 0;
}

void timer_free(TIMER *timer) { timer->flags = 0; }

void timer_init(TIMER *timer, QUEUE *queue, int data) {
	timer->queue = queue;
	timer->data = data;
}

void timer_settime(TIMER *timer, dword timeout) {
	int e;
	TIMER *t, *s;

	timer->timeout = timeout + timerctl.count;
	timer->flags = TIMER_FLAGS_USING;	// 使用中マーク
	e = io_load_eflags();
	io_cli();
	t = timerctl.t0;

	// 現在のタイマをリストの先頭に挿入する場合
	if(timer->timeout <= t->timeout) {
		timerctl.t0 = timer;
		timer->next = t;
		timerctl.next = timer->timeout;
		io_store_eflags(e);
		return;
	}

	// 開いた隙間に入れる
	for(;;) {
		s = t;
		t = t->next;
		if(timer->timeout <= t->timeout) {
			s->next = timer;
			timer->next = t;
			io_store_eflags(e);
			return;
		}
	}
}
