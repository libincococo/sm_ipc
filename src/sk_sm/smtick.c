/*
 *	SMTICK.C									ver 1.0
 *
 * (c)	Copyright  Federal Technologies Limited	2001 - 2002
 *
 *    	Source file name	: smtick.c
 *
 *    	AUTHOR (S) NAME	:  KBA, Jan.2000
 *
 *    	Original Work		:
 *						SmllWin timer tick API & process.
 * =======================
 * IMPROVEMENTS THOUGHT  OF
 * =======================
 *				None
 * ==============
 * Modification History
 * ==============
 * Date			Initials			Modification
 * ===================================
 *
*/

#include <stdio.h>
#include "smallwin.h"
#include "smglobal.h"

#include "os.h"

typedef struct tag_tick_message {
	HSM		hsm;
	u32			n100msec;
	u32			count;
	u32			param2;
	s32			sysflag;
} TICK_MSG;

static TICK_MSG		tick_list[SMALLWIN_NTICKS];
static semaphore_t	sem_tick;

//extern void watchdog_reset_timer(void);

static void sm_ticker(void *param)
{
	s32	i;

	while(1) {
		//print("proc the ticker...................\n");
		task_delay(SEC_100M);

#if 0
		j ^= 1;
		if(j)
			send_key_message(SMKY_INVALID, 0);
#endif
		//watchdog_reset_timer();
		semaphore_wait(&sem_tick);
		for(i = 0; i < SMALLWIN_NTICKS; i++)
		{
			if(tick_list[i].hsm)
			{
				if(tick_list[i].count == tick_list[i].n100msec)
				{
					if(is_message_param(tick_list[i].hsm, SMM_TICK, i + 1, tick_list[i].count) == 0)
					{
						if(!sm_is_smallwin(smp_common(tick_list[i].hsm)))
						{
//							print("...sm_tick()...hsm=0x%08x, htick=%d\n",tick_list[i].hsm, i + 1);
							tick_list[i].hsm = 0;
						}
						else
						{
							if(tick_list[i].sysflag)
							{
								if(tick_list[i].param2)
									send_message(tick_list[i].hsm, SMM_SYSTICK, i + 1, tick_list[i].param2);
								else
									send_message(tick_list[i].hsm, SMM_SYSTICK, i + 1, tick_list[i].count);
							}
							else
							{
								if(tick_list[i].param2)
									send_message(tick_list[i].hsm, SMM_TICK, i + 1, tick_list[i].param2);
								else
									send_message(tick_list[i].hsm, SMM_TICK, i + 1, tick_list[i].count);
							}
						}
					}
					tick_list[i].count = 0;
				}
				tick_list[i].count++;
			}
		}
		semaphore_signal(&sem_tick);
	}
}

task_id_t SmallwinTickTask = 0;

void sm_tick_init(void)
{
	Memset(tick_list, 0, sizeof(tick_list));
	semaphore_init_fifo(&sem_tick, 1);

	SmallwinTickTask = task_create(sm_ticker, NULL, NULL, 0x0800, TASK_PRIORITY_LOWEST, "SWTicker");
	if (SmallwinTickTask == 0)
	{
		printf("...Smallwindows Ticket Process Create Error....\n");
	}
}

s32 sm_set_xtick(HSM hsm, u32 n100msec, u32 param2, s32 sysflag)
{
	s32		i;

	//assert(hsm);
	semaphore_wait(&sem_tick);
	for(i = 0; i < SMALLWIN_NTICKS; i++) {
		if(tick_list[i].hsm == 0) {
			tick_list[i].hsm = hsm;
			tick_list[i].n100msec = n100msec;
			tick_list[i].count = 0;
			tick_list[i].param2 = param2;
			tick_list[i].sysflag = sysflag;
			break;
		}
	}
	semaphore_signal(&sem_tick);

	if(i != SMALLWIN_NTICKS) {
		return (i + 1);
	}
	else
		//assert(0);

	return 0;
}

s32 sm_create_tick(HSM hsm, u32 n100msec)
{
	//assert(hsm);
	return sm_set_xtick(hsm, n100msec, 0, 0);
}

s32 sm_create_systick(HSM hsm, u32 n100msec)
{
	//assert(hsm);
	return sm_set_xtick(hsm, n100msec, 0, 1);
}

void sm_destroy_tick(s32 htick)
{
	//assert(htick != -1);
	semaphore_wait(&sem_tick);
	if((htick > 0) && (htick <= SMALLWIN_NTICKS))
		tick_list[htick - 1].hsm = 0;
	else{
		char aksjdflja_buff[100];
		Sprintf(aksjdflja_buff,"htick=%d\n",htick);
	}
	semaphore_signal(&sem_tick);
}
