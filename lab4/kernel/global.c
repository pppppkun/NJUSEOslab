
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            global.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#define GLOBAL_VARIABLES_HERE

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"

PUBLIC PROCESS proc_table[NR_TASKS + NR_PROCS];
PUBLIC Semaphore sem[SEM_NUM];

PUBLIC TASK task_table[NR_TASKS] = {
	{task_tty, STACK_SIZE_TTY, "tty"},
	{task_clear, STACK_SIZE_CLEAR, "clear"}};

PUBLIC TASK user_proc_table[NR_PROCS] = {
	{ReadA, STACK_SIZE_READA, "ReadA"},
	{ReadB, STACK_SIZE_READB, "ReadB"},
	{ReadC, STACK_SIZE_READC, "ReadC"},
	{WriteD, STACK_SIZE_WRITED, "WriteD"},
	{WriteE, STACK_SIZE_WRITEE, "WriteE"},
	{PrintF, STACK_SIZE_PRINTF, "PrintF"}};

PUBLIC char task_stack[STACK_SIZE_TOTAL];

PUBLIC TTY tty_table[NR_CONSOLES];
PUBLIC CONSOLE console_table[NR_CONSOLES];

PUBLIC irq_handler irq_table[NR_IRQ];

PUBLIC system_call sys_call_table[NR_SYS_CALL] = {sys_get_ticks, sys_printf, sys_sleep, sys_sem};
