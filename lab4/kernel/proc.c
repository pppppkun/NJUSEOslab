
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
			if(p->block==2){
				continue;
			}
			if(p->block == 1){
				p->ticks++;
				if(p->ticks==0){
					p->block = 0;
					p->ticks=p->priority;
				}
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
				if(p->block==1) continue;
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
	return;
}

PUBLIC void sys_sleep(int t){
	p_proc_ready->block = 1;
	p_proc_ready->ticks = -(t*HZ/1000);
	schedule();
}

PUBLIC int sys_sem(int value, int select){
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
PUBLIC int sys_sem_init(int value){
	int i = 0;
	for(i = 0;i<SEM_NUM;i++){
		if(sem[i].state==0) break;
	}
	if(i==SEM_NUM) return -1;
	sem[i].state=1;
	sem[i].value=value;
	return i;
}
PUBLIC int sys_sem_post(int index){
	//TODO
	sem[index].value++;
	if(sem[index].value<=0){
		PROCESS* n = sem[index].pcb[sem[index].left++];
		n->block=0;
		n->ticks=n->priority;
	}
	return 0;
}
// value = index
PUBLIC int sys_sem_wait(int index){
	sem[index].value--;
	if(sem[index].value<0){
		p_proc_ready->block=2;
		p_proc_ready->ticks=-1;
		sem[index].pcb[sem[index].right++] = p_proc_ready;
		schedule();
	}
	return 0;
}