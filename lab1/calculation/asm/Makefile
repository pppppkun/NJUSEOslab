run:
	@nasm -f elf -o add.o add.asm 
	@ld -e main add.o -o add
	@./add

debug:
	@nasm -f elf -o add.o add.asm 
	@ld -o add -e main add.o
	@gdb ./add

clean:
	@rm add.o 
	@rm add