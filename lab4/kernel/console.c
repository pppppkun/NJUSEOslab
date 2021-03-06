
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			      console.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
						    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*
	回车键: 把光标移到第一列
	换行键: 把光标前进到下一行
*/

#include "type.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "keyboard.h"
#include "proto.h"
PRIVATE void set_cursor(unsigned int position);
PRIVATE void set_video_start_addr(u32 addr);
PRIVATE void flush(CONSOLE *p_con);
PRIVATE void make_str(char ch);
PRIVATE void find(CONSOLE *p_con);
PRIVATE void restore(CONSOLE *p_con);
PRIVATE void find_space(CONSOLE *p_con);
PRIVATE void find_tab(CONSOLE *p_con);
int tabs[1000];
int index;
char str[1000];
int str_index;
unsigned int enter[1000];
int enter_index;
/*======================================================================*
			   init_screen
 *======================================================================*/
PUBLIC void init_screen(TTY *p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;
	index = 0;
	enter_index = 0;
	int v_mem_size = V_MEM_SIZE >> 1; /* 显存总大小 (in WORD) */
	int con_v_mem_size = v_mem_size / NR_CONSOLES;
	p_tty->p_console->original_addr = nr_tty * con_v_mem_size;
	p_tty->p_console->v_mem_limit = con_v_mem_size;
	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;

	/* 默认光标位置在最开始处 */
	p_tty->p_console->cursor = p_tty->p_console->original_addr;

	if (nr_tty == 0)
	{
		/* 第一个控制台沿用原来的光标位置 */
		p_tty->p_console->cursor = disp_pos / 2;
		disp_pos = 0;
	}
	else
	{
		out_char(p_tty->p_console, nr_tty + '0');
		out_char(p_tty->p_console, '#');
	}

	set_cursor(p_tty->p_console->cursor);
}

PUBLIC void console_clear(TTY *p_tty)
{
	int nr_tty = p_tty - tty_table;
	p_tty->p_console = console_table + nr_tty;
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_tty->p_console->cursor * 2);
	int now = p_tty->p_console->current_start_addr;

	index = 0;
	int v_mem_size = V_MEM_SIZE >> 1; /* 显存总大小 (in WORD) */
	int con_v_mem_size = v_mem_size / NR_CONSOLES;
	p_tty->p_console->original_addr = nr_tty * con_v_mem_size;
	p_tty->p_console->v_mem_limit = con_v_mem_size;
	p_tty->p_console->current_start_addr = p_tty->p_console->original_addr;

	/* 默认光标位置在最开始处 */
	p_tty->p_console->cursor = p_tty->p_console->original_addr;

	if (nr_tty == 0)
	{
		/* 第一个控制台沿用原来的光标位置 */
		p_tty->p_console->cursor = disp_pos / 2;
		disp_pos = 0;
	}

	u8 *np_vmem = (u8 *)(V_MEM_BASE + p_tty->p_console->cursor * 2);
	u8 *last_vmem=(u8 *)((p_tty->p_console->original_addr + p_tty->p_console->v_mem_limit - SCREEN_WIDTH)*2+V_MEM_BASE);
	while (last_vmem != np_vmem)
	{
		*(--last_vmem) = DEFAULT_CHAR_COLOR;
		*(--last_vmem) = ' ';
	}

	set_cursor(p_tty->p_console->cursor);
}

/*======================================================================*
			   is_current_console
*======================================================================*/
PUBLIC int is_current_console(CONSOLE *p_con)
{
	return (p_con == &console_table[nr_current_console]);
}

/*======================================================================*
			   out_char
 *======================================================================*/
PUBLIC void out_char(CONSOLE *p_con, char ch)
{
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2);
	if (lock == 1 && ch != '\e' && ch != '\n')
	{
		make_str(ch);
	}
	if (lock == 1 && ch == '\n')
	{
		find(p_con);
		return;
	}
	if (lock == 1 && ch == '\e')
	{
		restore(p_con);
		lock = 0;
		return;
	}
	switch (ch)
	{
	case '\n':
		if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - SCREEN_WIDTH)
		{
			enter[enter_index++] = p_con->cursor;
			p_con->cursor = p_con->original_addr + SCREEN_WIDTH * ((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH + 1);
		}
		else
		{
			p_con->cursor = p_con->original_addr;
			while (p_con->current_start_addr > p_con->original_addr)
			{
				p_con->current_start_addr -= SCREEN_WIDTH;
			}
		}
		break;
	case '\b':
		if (p_con->cursor > p_con->original_addr)
		{
			if (index == 0 || p_con->cursor - 1 != tabs[index - 1])
			{
				if (p_con->cursor == p_con->original_addr + SCREEN_WIDTH * ((p_con->cursor - p_con->original_addr) / SCREEN_WIDTH))
				{
					if(p_con->cursor != p_con->original_addr)
						p_con->cursor = enter[--enter_index];
				}
				else
				{
					p_con->cursor--;
					*(p_vmem - 2) = ' ';
					*(p_vmem - 1) = DEFAULT_CHAR_COLOR;
				}
			}
			else
			{
				index -= 1;
				p_con->cursor -= 4;
			}
		}
		break;
	case '\t':
		if (p_con->cursor < p_con->original_addr + p_con->v_mem_limit - 1)
		{
			tabs[index++] = p_con->cursor + 3;
			p_con->cursor += 4;
			if(lock==1){
				*p_vmem++=' ';
				*p_vmem++=0x2c;
				*p_vmem++=' ';
				*p_vmem++=0x2c;
				*p_vmem++=' ';
				*p_vmem++=0x2c;
				*p_vmem++=' ';
				*p_vmem++=0x2c;
				index-=1;
			}
		}
		break;
	case '\e':
		lock = 1;
		str_index = 0;
		break;
	default:
		if (p_con->cursor <
			p_con->original_addr + p_con->v_mem_limit - 1)
		{
			*p_vmem++ = ch;
			if (lock != 1)
				*p_vmem++ = DEFAULT_CHAR_COLOR;
			else{
				if(*(p_vmem-1)==' '){
					*p_vmem++ = 0x2c;
				}else{
					*p_vmem++ = 0x0c;
				}
			}
			p_con->cursor++;
		}
		break;
	}

	while (p_con->cursor >= p_con->current_start_addr + SCREEN_SIZE)
	{
		scroll_screen(p_con, SCR_DN);
	}

	flush(p_con);
}

/*======================================================================*
                           flush
*======================================================================*/
PRIVATE void flush(CONSOLE *p_con)
{
	set_cursor(p_con->cursor);
	set_video_start_addr(p_con->current_start_addr);
}

/*======================================================================*
			    set_cursor
 *======================================================================*/
PRIVATE void set_cursor(unsigned int position)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, CURSOR_H);
	out_byte(CRTC_DATA_REG, (position >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, CURSOR_L);
	out_byte(CRTC_DATA_REG, position & 0xFF);
	enable_int();
}

/*======================================================================*
			  set_video_start_addr
 *======================================================================*/
PRIVATE void set_video_start_addr(u32 addr)
{
	disable_int();
	out_byte(CRTC_ADDR_REG, START_ADDR_H);
	out_byte(CRTC_DATA_REG, (addr >> 8) & 0xFF);
	out_byte(CRTC_ADDR_REG, START_ADDR_L);
	out_byte(CRTC_DATA_REG, addr & 0xFF);
	enable_int();
}

/*======================================================================*
			   select_console
 *======================================================================*/
PUBLIC void select_console(int nr_console) /* 0 ~ (NR_CONSOLES - 1) */
{
	if ((nr_console < 0) || (nr_console >= NR_CONSOLES))
	{
		return;
	}

	nr_current_console = nr_console;

	set_cursor(console_table[nr_console].cursor);
	set_video_start_addr(console_table[nr_console].current_start_addr);
}

/*======================================================================*
			   scroll_screen
 *----------------------------------------------------------------------*
 滚屏.
 *----------------------------------------------------------------------*
 direction:
	SCR_UP	: 向上滚屏
	SCR_DN	: 向下滚屏
	其它	: 不做处理
 *======================================================================*/
PUBLIC void scroll_screen(CONSOLE *p_con, int direction)
{
	if (direction == SCR_UP)
	{
		if (p_con->current_start_addr > p_con->original_addr)
		{
			p_con->current_start_addr -= SCREEN_WIDTH;
		}
	}
	else if (direction == SCR_DN)
	{
		if (p_con->current_start_addr + SCREEN_SIZE <
			p_con->original_addr + p_con->v_mem_limit)
		{
			p_con->current_start_addr += SCREEN_WIDTH;
		}
	}
	else
	{
	}

	set_video_start_addr(p_con->current_start_addr);
	set_cursor(p_con->cursor);
}
PRIVATE void make_str(char ch)
{
	if(ch=='\b'){
		str_index--;
		return;
	}
	str[str_index++] = ch;
}
PRIVATE void find(CONSOLE *p_con)
{
	if(str_index == 0) return;
	if (str_index == 1 && str[str_index - 1] == ' ')
	{
		find_space(p_con);
		return;
	}
	if (str_index == 1 && str[str_index-1]== '\t'){
		find_tab(p_con);
		return;
	}
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2)-str_index*2-2;
	u8 *np_vmem = (u8 *)(V_MEM_BASE + p_con->original_addr * 2);
	while (p_vmem >= np_vmem)
	{
		int flag = 1;
		for (int i = 0; i < str_index; i++)
		{
			if(p_vmem>=(np_vmem + 2 * i)){
				
			}
			else{
				flag = -1;
				break;
			}
			if (flag == 0)
				break;
			if (*(np_vmem + 2 * i) != str[i])
				flag = 0;
		}
		if (flag == 0)
		{
			np_vmem++;
		}
		else if(flag==1)
		{
			for (int i = 0; i < str_index; i++)
			{
				np_vmem++;
				if(*(np_vmem-1)==' '){
					*np_vmem++=0x2c;
				}
				else *np_vmem++ = 0x0c;
			}
		}else if(flag == -1){
			return;
		}
	}
}
PRIVATE void find_space(CONSOLE *p_con)
{
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2)-str_index*2-2;
	u8 *np_vmem = (u8 *)(V_MEM_BASE + p_con->original_addr * 2);
	//u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2);
	while (p_vmem >= np_vmem)
	{
		if (*np_vmem == ' ')
		{
			int flag = 1;
			for (int i = 0; i < index; i++)
			{
				if (np_vmem == (u8 *)((tabs[i] - 3) * 2 + V_MEM_BASE))
				{
					flag = 0;
					break;
				}
			}
			if (flag == 1)
			{
				np_vmem++;
				*np_vmem++ = 0x40;
			}
			if (flag == 0)
			{
				//disp_str("NMSL");
				np_vmem++;
				np_vmem++;
				np_vmem++;
				np_vmem++;
				np_vmem++;
				np_vmem++;
				np_vmem++;
				np_vmem++;
			}
		}
		else
			np_vmem++;
	}
}
PRIVATE void find_tab(CONSOLE *p_con)
{
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2)-str_index*2-2;
	u8 *np_vmem = (u8 *)(V_MEM_BASE + p_con->original_addr * 2);
	//u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2);
	while (p_vmem >= np_vmem)
	{
		if (*np_vmem == ' ')
		{
			int flag = 1;
			for (int i = 0; i < index; i++)
			{
				if (np_vmem == (u8 *)((tabs[i] - 3) * 2 + V_MEM_BASE))
				{
					flag = 0;
					break;
				}
			}
			if (flag == 1)
			{
				np_vmem++;
				np_vmem++;
			}
			if (flag == 0)
			{
				//disp_str("NMSL");
				np_vmem++;
				*np_vmem++=0x2c;
				np_vmem++;
				*np_vmem++=0x2c;
				np_vmem++;
				*np_vmem++=0x2c;
				np_vmem++;
				*np_vmem++=0x2c;
			}
		}
		else
			np_vmem++;
	}
}
PRIVATE void restore(CONSOLE *p_con)
{
	u8 *p_vmem = (u8 *)(V_MEM_BASE + p_con->cursor * 2);
	u8 *np_vmem = (u8 *)(V_MEM_BASE + p_con->original_addr * 2);
	if (str_index == 1 && str[str_index-1]== '\t'){
		str_index=4;
	}
	while (p_vmem >= np_vmem)
	{
		*(--p_vmem) = DEFAULT_CHAR_COLOR;
		if (str_index > 0)
		{
			*(--p_vmem) = ' ';
			p_con->cursor--;
			str_index--;
		}
		else
			--p_vmem;
	}
	set_cursor(p_con->cursor);
}
