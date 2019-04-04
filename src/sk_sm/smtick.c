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


#include "smmsg.h"
#include "sk_tbx.h"

#define SMALLWIN_NTICKS				32

typedef struct tag_tick_message {
	HSM			hsm;
	u32			n100msec;
	u32			count;
	u32			param2;
	s32			sysflag;
} TICK_MSG;

static TICK_MSG		tick_list[SMALLWIN_NTICKS];
static sk_sem_id_t	sem_tick_id;
static sk_task_id_t	SmallwinTickTask = 0;
static u32 			time_tick = 100;

static void* sm_ticker(void *param)
{
	s32	i;

	while(1) {

		sk_task_delay(time_tick);

		sk_sem_lock(sem_tick_id);
		for(i = 0; i < SMALLWIN_NTICKS; i++)
		{
			if(tick_list[i].hsm)
			{
				if(tick_list[i].count*time_tick >= tick_list[i].n100msec)
				{
					if(is_message_param(tick_list[i].hsm, SMM_TICK, i + 1, tick_list[i].count) == 0)
					{

						if(tick_list[i].param2)
							send_message(tick_list[i].hsm, SMM_TICK, i + 1, tick_list[i].param2);
						else
							send_message(tick_list[i].hsm, SMM_TICK, i + 1, tick_list[i].count);

					}
					tick_list[i].count = 0;
				}
				tick_list[i].count++;
			}
		}
		sk_sem_unlock(sem_tick_id);
	}
}



void sm_tick_init(void)
{

	sk_status_code_t recode = SK_SUCCESS;
	memset(tick_list, 0, sizeof(tick_list));
	sk_sem_create(&sem_tick_id,"sm_tick", 1);


	recode = sk_task_create(&SmallwinTickTask,"sm_tick",sm_ticker,NULL,NULL,SK_TASK_DEFAULT_STACK_SIZE,SK_TASK_PRIORITY_LOWEST);
	if (recode != SK_SUCCESS)
	{
		SK_ERROR(("...Smallwindows Ticket Process Create Error....\n"));
	}
}

s32 sm_set_xtick(HSM hsm, u32 n100msec, u32 param2, s32 sysflag)
{
	s32		i;

	//assert(hsm);
	sk_sem_lock(sem_tick_id);
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
	sk_sem_unlock(sem_tick_id);

	if(i != SMALLWIN_NTICKS) {
		return (i + 1);
	}

	return 0;
}

s32 sm_create_tick(HSM hsm, u32 ms){

	return sm_set_xtick(hsm, ms, 0, 0);
}

s32 sm_create_systick(HSM hsm, u32 n100msec)
{
	return sm_set_xtick(hsm, n100msec, 0, 1);
}

void sm_destroy_tick(s32 htick)
{
	sk_sem_lock(sem_tick_id);
	if((htick > 0) && (htick <= SMALLWIN_NTICKS))
		tick_list[htick - 1].hsm = 0;
	sk_sem_lock(sem_tick_id);
}
