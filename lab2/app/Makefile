CC = gcc

# CFLAGS = -g
CFLAGS = -m32 -g

CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)

run:
	nasm -f elf32 my_print.asm
	$(CC) $(CFLAGS) -o main $(CFILES) my_print.o
	# gcc -m32 main.c my_print.o -o main
	rm -rf my_print.o
	./main

genFS: $(CFILES)
	$(CC) $(CFLAGS) -o genFS $(CFILES)
	./genFS

debug:
	$(CC) $(CFLAGS) -o genFS $(CFILES)
	gdb genFS

clean:
	@#rm -rf $(OBJS) genFS fs.bin
	rm -rf $(OBJS) genFS genFS.dSYM
