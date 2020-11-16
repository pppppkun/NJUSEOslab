
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                               clear_tty.c
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
                                                    Pkun, 2020
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

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
#define TTY_FIRST (tty_table)
#define TTY_END (tty_table + NR_CONSOLES)
/*======================================================================*
                           task_clear
 *======================================================================*/
PUBLIC void task_clear()
{
    
    while(1){
        milli_delay(200000);
        tty_clear(TTY_FIRST);
    }

}