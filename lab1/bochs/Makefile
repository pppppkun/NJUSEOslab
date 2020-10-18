run:
	nasm boot.asm -o boot.bin 
	dd if=boot.bin of=os.img bs=512 count=1 conv=notrunc
	bochs -f bochsrc

clean:
	@rm boot.bin