QEMU=qemu-system-i386

OSNAME=os
BOOTOBJS=$(OBJ)/boot.o $(OBJ)/graphic.o $(OBJ)/dsctbl.o \
$(OBJ)/int.o $(OBJ)/lib.o $(OBJ)/queue.o $(OBJ)/memory.o $(OBJ)/sheet.o\
$(OBJ)/keyboard.o $(OBJ)/mouse.o $(OBJ)/timer.o \
$(OBJ)/hankaku.o $(OBJ)/data.o $(OBJ)/func.o $(OBJ)/gasint.o\
$(OBJ)/mnemonic.o

ASRC=./src/asm
CSRC=./src/c
FSRC=./src/font
OBJ=./obj
LS=./ls
TOOL=./tool
INCLUDE=./src/c/include

IMG=$(OSNAME).img
OSSYS=$(OBJ)/$(OSNAME).sys
IPL=$(OBJ)/ipl.bin

BINOPT=-nostdlib -Wl,--oformat=binary
QEMUOPT=-m 32 -localtime -vga std -fda

$(IMG) : $(OSSYS) $(IPL)
	mformat -f 1440 -C -B $(IPL) -i $(IMG) ::
	mcopy $(OSSYS) -i $(IMG) ::

$(OSSYS) : $(BOOTOBJS) $(CSRC)/hankaku.c $(ASRC)/head.s 
	gcc $(ASRC)/head.s -nostdlib -T$(LS)/head.ls -o $(OBJ)/head.bin
	ld -o $(OBJ)/boot.bin --script=$(LS)/boot.ls $(BOOTOBJS)
	cat $(OBJ)/head.bin $(OBJ)/boot.bin > $(OSSYS)

$(IPL) : $(ASRC)/ipl.s
	gcc $(ASRC)/ipl.s -nostdlib -T$(LS)/ipl.ls -o $(IPL)

$(CSRC)/hankaku.c : $(FSRC)/hankaku.txt
	$(TOOL)/mkfont $(FSRC)/hankaku.txt -o $(CSRC)/hankaku.c

# General Rules

$(ASRC)/%.s : $(CSRC)/%.c
	gcc -S $(CSRC)/$*.c -I$(INCLUDE) $(BINOPT) -o $(ASRC)/$*.s

$(OBJ)/%.o : $(ASRC)/%.s
	as $(ASRC)/$*.s -o $(OBJ)/$*.o

# Command

run		: $(IMG)
	$(QEMU) $(QEMUOPT) $(IMG)
debug	: $(IMG)
	$(QEMU) -s -S $(QEMUOPT) $(IMG) -redir tcp:5555:127.0.0.1:1234 &
img		:;	make $(IMG)
clean	:;	rm $(OBJ)/*
