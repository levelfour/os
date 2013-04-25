#include "def.h"
#include "prot.h"

void set_segmdesc(SEGMENT_DESCRIPTOR *, dword, int, int);
void set_gatedesc(GATE_DESCRIPTOR *, int, int, int);

void init_gdtidt() {
	SEGMENT_DESCRIPTOR	*gdt = (SEGMENT_DESCRIPTOR *)ADR_GDT;
	GATE_DESCRIPTOR	  	*idt = (GATE_DESCRIPTOR *)ADR_IDT;
	int i;

	// GDTの初期化
	for( i = 0; i <= LIMIT_GDT / 8; i++ ) {
		set_segmdesc(gdt + i, 0, 0, 0);
	}
	set_segmdesc(gdt + 1, 0xffffffff, 0x00000000, AR_DATA32_RW);
	set_segmdesc(gdt + 2, LIMIT_BOTPAK, ADR_BOTPAK, AR_CODE32_ER);
	set_segmdesc(gdt + 3, 0xffffffff, 0x00000000, AR_CODE32_ER);
	load_gdtr(LIMIT_GDT, ADR_GDT);

	// IDTの初期化
	for( i = 0; i <= LIMIT_IDT / 8; i++ ) {
		set_gatedesc(idt + i, 0, 0, 0);
	}
	load_idtr(LIMIT_IDT, ADR_IDT);

	// IDTの設定
	set_gatedesc(idt + 0x20, (int)inthandler20, 3 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x21, (int)inthandler21, 3 * 8, AR_INTGATE32);
	set_gatedesc(idt + 0x2c, (int)inthandler2c, 3 * 8, AR_INTGATE32);
}

void set_segmdesc(SEGMENT_DESCRIPTOR *sd,
	dword limit, int base, int ar) {
	if( limit > 0xfffff ) {
		ar |= 0x8000;	// G_bit = 1
		limit /= 0x1000;
	}
	sd->limit_low		= limit & 0xffff;
	sd->base_low		= base & 0xffff;
	sd->base_mid		= (base >> 16) & 0xff;
	sd->access_right	= ar & 0xff;
	sd->limit_high		= ((limit >> 16) & 0x0f) | ((ar >> 8) & 0xf0);
	sd->base_high		= (base >> 24) & 0xff;
}

void set_gatedesc(GATE_DESCRIPTOR *gd,
	int offset, int selector, int ar) {
	gd->offset_low		= offset & 0xffff;
	gd->selector		= selector;
	gd->dw_count		= (ar >> 8) & 0xff;
	gd->access_right	= ar & 0xff;
	gd->offset_high		= (offset >> 16) & 0xffff;
}
