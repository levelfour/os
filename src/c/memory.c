#include "def.h"
#include "prot.h"

dword memtest(dword start, dword end) {
	char flag486 = 0;
	dword eflags, cr0, i;

	eflags = io_load_eflags();
	eflags |= EFLAGS_AC_BIT;
	io_store_eflags(eflags);

	eflags = io_load_eflags();
	// 386ではAC=1にしても自動で0に戻ってしまう
	if((eflags & EFLAGS_AC_BIT) != 0) flag486 = 1;

	eflags &= ~EFLAGS_AC_BIT;
	io_store_eflags(eflags);

	// 486ならキャッシュ無効化
	if(flag486) {
		cr0 = load_cr0();
		cr0 |= CR0_CACHE_DISABLE;
		store_cr0(cr0);
	}

	i = memtest_sub(start, end);

	// キャッシュ有効化
	if(flag486) {
		cr0 = load_cr0();
		cr0 &= ~CR0_CACHE_DISABLE;
		store_cr0(cr0);
	}

	return i;
}

void mem_init(MEMORY_MANAGE *man) {
	man->frees		= 0;	// 空き情報の個数
	man->maxfrees	= 0;	// freeの最大値
	man->lostsize	= 0;	// 解放に失敗した合計サイズ
	man->losts		= 0;	// 解放に失敗した回数
}

dword mem_total(MEMORY_MANAGE *man) {
	dword i, t = 0;
	for(i = 0; i < man->frees; i++) {
		t += man->free[i].size;
	}
	return t;
}

// メモリ確保
dword mem_alloc(MEMORY_MANAGE *man, dword size) {
	dword i, a;
	for(i= 0; i < man->frees; i++) {
		// 十分な広さの空きを発見した場合
		if(man->free[i].size >= size) {
			a = man->free[i].address;
			man->free[i].address += size;
			man->free[i].size -= size;
			// free[i]がなくなったので前に詰める
			if(man->free[i].size == 0) {
				man->frees--;
				for(; i < man->frees; i++) man->free[i] = man->free[i+1];
			}
			return a;
		}
	}
	return 0;	// 空きがない
}

// メモリ解放
int mem_free(MEMORY_MANAGE *man, dword address, dword size) {
	int i, j;

	// 解放するメモリをどこに入れるか決める
	for(i = 0; i < man->frees; i++) {
		if(man->free[i].address > address) break;
	}

	// 前があるとき
	if(i > 0) {
		// 前の領域にまとめられるとき
		if(man->free[i-1].address + man->free[i-1].size == address) {
			man->free[i-1].size += size;
			// 後ろもあるとき
			if(i < man->frees) {
				// 後ろの領域もまとめられるとき
				if(address += size == man->free[i].address) {
					man->free[i-1].size += man->free[i].size;

					// man->free[i]がなくなったので削除
					man->frees--;
					for(; i < man->frees; i++)
						man->free[i] = man->free[i+1];
				}
			}
			return 0;
		}
	}

	// 前にはまとめられなかったとき
	if(i < man->frees) {
		// 後ろがあるとき
		if(address += size == man->free[i].address) {
			man->free[i].address = address;
			man->free[i].size += size;
			return 0;
		}
	}

	// 前とも後ろともまとめられないとき
	if(man->frees < MEMMAN_FREES) {
		// free[j]よりも後ろを、後ろへずらして、隙間を作る
		for(j = man->frees; j > i; j--) 
			man->free[j] = man->free[j-1];
		man->frees++;
		if(man->maxfrees < man->frees) {
			man->maxfrees = man->frees;
		}
		man->free[i].address = address;
		man->free[i].size = size;
		return 0;
	}

	// 記憶領域を越えているとき
	man->losts++;
	man->lostsize += size;
	return -1;	// 失敗
}

dword mem_alloc_4k(MEMORY_MANAGE *man, dword size) {
	dword a;
	size = (size + 0xfff) & 0xfffff000;
	a = mem_alloc(man, size);
	return a;
}

int mem_free_4k(MEMORY_MANAGE *man, dword addr, dword size) {
	int i;
	size = (size + 0xfff) & 0xfffff000;
	i = mem_free(man, addr, size);
	return i;
}
