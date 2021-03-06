
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                            proto.h
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Forrest Yu, 2005
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/* klib.asm */
PUBLIC void	out_byte(u16 port, u8 value);
PUBLIC u8	in_byte(u16 port);
PUBLIC void	disp_str(char * info);
PUBLIC void	disp_color_str(char * info, int color);

/* protect.c */
PUBLIC void	init_prot();
PUBLIC u32	seg2phys(u16 seg);

/* klib.c */
PUBLIC void	delay(int time);

/* kernel.asm */
void restart();

/* main.c */
void ReadA();
void ReadB();
void ReadC();
void WriteD();
void WriteE();
void PrintF();

/* i8259.c */
PUBLIC void put_irq_handler(int irq, irq_handler handler);
PUBLIC void spurious_irq(int irq);

/* clock.c */
PUBLIC void clock_handler(int irq);
PUBLIC void init_clock();

/* keyboard.c */
PUBLIC void init_keyboard();

/* tty.c */
PUBLIC void task_tty();
PUBLIC void in_process(TTY* p_tty, u32 key);
PUBLIC void tty_clear(TTY *p_tty);

/* console.c */
PUBLIC void out_char(CONSOLE* p_con, char ch);
PUBLIC void scroll_screen(CONSOLE* p_con, int direction);
PUBLIC void console_clear(TTY *p_tty);

/* clear_tty.c */
PUBLIC void task_clear();

/* 以下是系统调用相关 */

/* proc.c */
PUBLIC  int     sys_get_ticks();        /* sys_call */
PUBLIC  void    sys_printf(char * str);
PUBLIC  void    sys_sleep(int t);
PUBLIC  int     sys_sem(int index, int select);
PUBLIC  int     sys_sme_init(int value);   
PUBLIC  int     sys_sem_post(int index);
PUBLIC  int     sys_sem_wait(int index);
PUBLIC  void    sys_color_printf(char *str, int color);

/* syscall.asm */
PUBLIC  void    sys_call();             /* int_handler */
PUBLIC  int     get_ticks();
PUBLIC  void    printf(char * str);
PUBLIC  void    sleep(int t);
PUBLIC  int     sem_init(int value);
PUBLIC  int     sem_p(int index);
PUBLIC  int     sem_v(int index);
PUBLIC  int     color_printf(char * str, int color);