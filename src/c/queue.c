#include "def.h"
#include "prot.h"

// QUEUEバッファの初期化
void queue_init(QUEUE *queue, int size, int *buf) {
	queue->size = size;
	queue->buf = buf;
	queue->free = size;	// 空き
	queue->flags = 0;
	queue->next_w = 0;	// 書き込み位置
	queue->next_r = 0;	// 読み込み位置
}

// QUEUEバッファへ書き込み
int queue_put(QUEUE *queue, int data) {
	// 空きがなくてあふれた
	if(queue->free == 0) {
		queue->flags |= FLAGS_OVERRUN;
		return -1;
	}

	queue->buf[queue->next_w] = data;
	queue->next_w++;
	if(queue->next_w == queue->size) queue->next_w = 0;
	queue->free--;

	return 0;
}

// QUEUEバッファから読み込む
int queue_get(QUEUE *queue) {
	int data;
	// バッファが空のとき
	if(queue->free == queue->size) {
		return -1;
	}

	data = queue->buf[queue->next_r];
	queue->next_r++;
	if(queue->next_r == queue->size) queue->next_r = 0;
	queue->free++;
	return data;
}

// どのくらいデータが溜まっているか返す
int queue_status(QUEUE *queue) {
	return queue->size - queue->free;
}
