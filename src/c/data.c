#include "def.h"

const char keytable[0x54] = {
	0,  0,  '1','2','3','4','5','6','7','8','9','0','-','^',0,  0  ,
	'Q','W','E','R','T','Y','U','I','O','P','@','[',0  ,0  ,'A','S',
	'D','F','G','H','J','K','L',';',':',0  ,0  ,']','Z','X','C','V', 
	'B','N','M',',','.','/',0  ,'*',0  ,' ',0  ,0  ,0  ,0  ,0  ,0  ,
	0  ,0  ,0  ,0  ,0  ,0  ,0  ,'7','8','9','-','4','5','6','+','1',
	'2','3','0','.'
};

const byte table_rgb[COLORN * 3] = {
	0x00, 0x00, 0x00,	// 0: black
	0xff, 0x00, 0x00,	// 1: red
	0x00, 0xff, 0x00,	// 2: lime
	0xff, 0xff, 0x00,	// 3: yellow
	0x00, 0x00, 0xff,	// 4: blue
	0xff, 0x00, 0xff,	// 5: purple
	0x00, 0xff, 0xff,	// 6: cyan
	0xff, 0xff, 0xff,	// 7: white
	0xc6, 0xc6, 0xc6,	// 8: gray
	0x84, 0x00, 0x00, 	// 9: dark red
	0x00, 0x84, 0x00,	// 10: green
	0x84, 0x84, 0x00,	// 11: dark yellow
	0x00, 0x00, 0x84,	// 12: dark blue
	0x84, 0x00, 0x84,	// 13: dark purple
	0x00, 0x84, 0x84,	// 14: dark cyan
	0x84, 0x84, 0x84	// 15: dark gray
};

const char _cursor[16][16] = {
	"**************..",
	"*OOOOOOOOOOO*...",
	"*OOOOOOOOOO*....",
	"*OOOOOOOOO*.....",
	"*OOOOOOOO*......",
	"*OOOOOOO*.......",
	"*OOOOOOO*.......",
	"*OOOOOOOO*......",
	"*OOOO**OOO*.....",
	"*OOO*..*OOO*....",
	"*OO*....*OOO*...",
	"*O*......*OOO*..",
	"**........*OOO*.",
	"*..........*OOO*",
	"............*OO*",
	".............***",
};
