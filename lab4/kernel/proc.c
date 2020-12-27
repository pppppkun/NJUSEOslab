
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               proc.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "tty.h"
#include "console.h"
#include "string.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
PRIVATE void adjust(int pid);
PRIVATE void clear();
/*======================================================================*
                              schedule
 *======================================================================*/
PUBLIC void schedule()
{
	PROCESS *p;
	int greatest_ticks = 0;
	while (!greatest_ticks)
	{
		for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
		{
			if (p->block == 2)
			{
				block_time++;
				if (block_time > 8000)
				{
					//adjust(p->pid);
				}
				continue;
			}
			if (p->block == 1)
			{
				// p->ticks++;
				// if (p->ticks == 0)
				// {
				// 	p->block = 0;
				// 	p->ticks = p->priority;
				// }
				continue;
			}
			if (p->ticks > greatest_ticks)
			{
				greatest_ticks = p->ticks;
				p_proc_ready = p;
			}
		}

		if (!greatest_ticks)
		{
			for (p = proc_table; p < proc_table + NR_TASKS + NR_PROCS; p++)
			{
				if (p->block == 1)
					continue;
				p->ticks = p->priority;
			}
		}
	}
}

/*======================================================================*
                           sys_get_ticks
 *======================================================================*/
PUBLIC int sys_get_ticks()
{
	return ticks;
}

PUBLIC void sys_printf(char *str)
{
	disp_str(str);
	/* 通过打印空格的方式清空屏幕的前五行，并把 disp_pos 清零 */
	if (disp_pos > 80 * 10 * 2)
	{
		clear();
	}
	return;
}

PUBLIC void sys_color_printf(char *str)
{
	disp_color_str(str, RED);
	if (disp_pos > 80 * 24 * 2)
	{
		clear();
	}
	return;
}

PUBLIC void sys_sleep(int t)
{
	p_proc_ready->block = 1;
	p_proc_ready->ticks = -t;
	schedule();
}

PUBLIC int sys_sem(int value, int select)
{
	switch (select)
	{
	case SEM_INIT:
		return sys_sem_init(value);
	case SEM_POST:
		return sys_sem_post(value);
	case SEM_WAIT:
		return sys_sem_wait(value);
	default:
		break;
	}
}
// value = num
PUBLIC int sys_sem_init(int value)
{
	int i = 0;
	for (i = 0; i < SEM_NUM; i++)
	{
		if (sem[i].state == 0)
			break;
	}
	if (i == SEM_NUM)
		return -1;
	sem[i].state = 1;
	sem[i].value = value;
	return i;
}
PUBLIC int sys_sem_post(int index)
{
	//TODO
	sem[index].value++;
	if (sem[index].value <= 0)
	{
		PROCESS *n = sem[index].pcb[sem[index].left++];
		n->block = 0;
		n->ticks = n->priority;
	}
	return 0;
}
// value = index
PUBLIC int sys_sem_wait(int index)
{
	sem[index].value--;
	if (sem[index].value < 0)
	{
		p_proc_ready->block = 2;
		p_proc_ready->ticks = -1;
		sem[index].pcb[sem[index].right++] = p_proc_ready;
		schedule();
	}
	return 0;
}

PRIVATE void clear()
{
	char space[2] = " ";
	disp_pos = 0;
	for (int i = 0; i < 80 * 25; i++)
	{
		disp_str(space);
	}
	disp_pos = 0;
}

PRIVATE void adjust(int pid)
{
	block_time = 0;
	TASK *p_task = task_table;
	PROCESS *p_proc = proc_table;
	char *p_task_stack = task_stack + STACK_SIZE_TOTAL;
	u16 selector_ldt = SELECTOR_LDT_FIRST;
	int i;
	u8 privilege;
	u8 rpl;
	int eflags;
	for (i = 0; i < NR_TASKS + NR_PROCS; i++)
	{
		if (i < NR_TASKS)
		{ /* 任务 */
			p_task = task_table + i;
			privilege = PRIVILEGE_TASK;
			rpl = RPL_TASK;
			eflags = 0x1202; /* IF=1, IOPL=1, bit 2 is always 1 */
		}
		else
		{ /* 用户进程 */
			p_task = user_proc_table + (i - NR_TASKS);
			privilege = PRIVILEGE_USER;
			rpl = RPL_USER;
			eflags = 0x202; /* IF=1, bit 2 is always 1 */
		}

		strcpy(p_proc->p_name, p_task->name); // name of the process
		p_proc->pid = i;					  // pid
		p_proc->block = 0;
		p_proc->ldt_sel = selector_ldt;

		memcpy(&p_proc->ldts[0], &gdt[SELECTOR_KERNEL_CS >> 3],
			   sizeof(DESCRIPTOR));
		p_proc->ldts[0].attr1 = DA_C | privilege << 5;
		memcpy(&p_proc->ldts[1], &gdt[SELECTOR_KERNEL_DS >> 3],
			   sizeof(DESCRIPTOR));
		p_proc->ldts[1].attr1 = DA_DRW | privilege << 5;
		p_proc->regs.cs = (0 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ds = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.es = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.fs = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.ss = (8 & SA_RPL_MASK & SA_TI_MASK) | SA_TIL | rpl;
		p_proc->regs.gs = (SELECTOR_KERNEL_GS & SA_RPL_MASK) | rpl;

		p_proc->regs.eip = (u32)p_task->initial_eip;
		p_proc->regs.esp = (u32)p_task_stack;
		p_proc->regs.eflags = eflags;

		p_task_stack -= p_task->stacksize;
		p_proc++;
		p_task++;
		selector_ldt += 1 << 3;
	}

	proc_table[0].ticks = proc_table[0].priority = 15;
	//proc_table[1].ticks = proc_table[1].priority = 5;
	proc_table[2].ticks = proc_table[2].priority = get_ticks()%8+1; //read
	proc_table[3].ticks = proc_table[3].priority = get_ticks()%8+1; //read
	proc_table[4].ticks = proc_table[4].priority = get_ticks()%8+1; //read
	proc_table[5].ticks = proc_table[5].priority = get_ticks()%8+1; //write
	proc_table[6].ticks = proc_table[6].priority = get_ticks()%8+1; //write
	proc_table[7].ticks = proc_table[7].priority = 1; //print
	proc_table[pid].ticks = proc_table[pid].priority = 9;

	for (int i = 0; i < SEM_NUM; i++)
	{
		sem[i].left = sem[i].right = sem[i].state = sem[i].value = 0;
	}

	k_reenter = 0;
	ticks = 0;

	p_proc_ready = &proc_table[pid];

	WriteMutex = sem_init(1);
	CountMutex = sem_init(1);
	readers = 0;
	clear();
	init_clock();
	init_keyboard();

	restart();
}