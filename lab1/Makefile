run:
	nasm boot.asm -o boot.bin 
	dd if=boot.bin of=os.img bs=512 count=1 conv=notrunc
	bochs -f bochsrc
add:
	@nasm -f elf -o add.o add.asm 
	@ld -e main add.o -o add

debug:
	@nasm -f elf -o add.o add.asm 
	@ld -o add -e main add.o
	@gdb ./add

clean:
	@rm boot.bin
	@rm add.o
	@rm add
cleanA:
	@rm add.o 
	@rm add