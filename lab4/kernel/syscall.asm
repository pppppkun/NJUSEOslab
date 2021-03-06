
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                               syscall.asm
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
;                                                     Forrest Yu, 2005
; ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

%include "sconst.inc"

_NR_get_ticks       equ 0 ; 要跟 global.c 中 sys_call_table 的定义相对应！
sys_call_printf		equ 1
sys_call_sleep		equ 2
sys_call_sem		equ 3
sys_call_color_printf equ 4
INT_VECTOR_SYS_CALL equ 0x90

; 导出符号
global	get_ticks
global 	printf
global 	sleep
global 	sem_init
global 	sem_p
global 	sem_v
global 	color_printf

bits 32
[section .text]

; ====================================================================
;                              get_ticks
; ====================================================================
get_ticks:
	mov	eax, _NR_get_ticks
	int	INT_VECTOR_SYS_CALL
	ret

printf:
	mov eax, sys_call_printf
	mov ebx, [esp+4]
	int INT_VECTOR_SYS_CALL
	ret 
sleep:
	mov eax, sys_call_sleep
	mov ebx, [esp+4]
	int INT_VECTOR_SYS_CALL
	ret
sem_init:
	mov eax, sys_call_sem
	mov ebx, [esp+4]
	mov ecx, 0
	int INT_VECTOR_SYS_CALL
	ret
sem_p:
	mov eax, sys_call_sem
	mov ebx, [esp+4]
	mov ecx, 2
	int INT_VECTOR_SYS_CALL
	ret
sem_v:
	mov eax, sys_call_sem
	mov ebx, [esp+4]
	mov ecx, 1
	int INT_VECTOR_SYS_CALL
	ret
color_printf:
	mov eax, sys_call_color_printf
	mov ebx, [esp+4]
	mov ecx, [esp+8]
	int INT_VECTOR_SYS_CALL
	ret