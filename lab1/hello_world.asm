SECTION .data

msg: db "Hello World!", 0x0a
len: equ $-msg

SECTION .text
global _main

_main:
    mov eax,4
    mov ebx,1
    mov ecx,msg
    mov edx,len
    int 80h

    mov eax,1
    mov ebx,0
    int 80h