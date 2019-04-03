/*
 *	 SMSMALLWIN_SCHEDULER.C				ver 1.0
 *
 * (c)	Copyright  Federal Technologies Limited	2001 - 2002
 *
 *    	Source file name	: smsmallwin_scheduler.c
 *
 *    	AUTHOR (S) NAME	: KBA, Jan.2001
 *
 *    	Original Work		:
 *					SmallWin smallwin_scheduler & kernel API.
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
#include <stdio.h>

#define SMALLWIN_NMSGQ				1000

static COMMON_SM	*smallwin_list = NULL;
static sk_sem_id_t	sem_system;
static sk_sem_id_t	sem_event;

static void smallwin_scheduler(void *param);

static void init_global(void)
{
	sk_sem_create(&sem_system,"system", 1);
	sk_sem_create(&sem_event, "event",0);
}

void wait_event(void)
{
	sk_sem_lock(sem_event);
}

void signal_event(void)
{
	sk_sem_unlock(sem_event);
}

void lock_system(void)
{
	sk_sem_lock(sem_system);
}

void unlock_system(void)
{
	sk_sem_unlock(sem_system);
}

static void sm_add_smallwin_list(COMMON_SM *node)
{
	COMMON_SM	*temp = smallwin_list;

	if(temp) {
		while(temp->next)
			temp = temp->next;

		temp->next = node;
		node->prev = temp;
		node->next = NULL;
	}
	else {
		smallwin_list = node;
		node->next = node->prev = NULL;
	}
}

u32 run_sm(HSM hsm, s32 message, s32 param1, s32 param2)
{
	COMMON_SM		*sm = smp_common(hsm);
	s32				return_value=0;

	if(sm->sm_proc)
		return_value = sm->sm_proc(hsm, message, param1, param2);

	return return_value;
}

HSM sm_create_smallwin(
	u16			sm_type,
	u16			sm_id,
	char		*sm_text,
	SM_PROC		sm_proc
	)
{
	COMMON_SM		*sm = NULL;


	lock_system();
	sm = (COMMON_SM*)sk_mem_malloc(sizeof(COMMON_SM));
	if(sm) {
		memset(sm, 0, sizeof(COMMON_SM));
		sm->sm_type = sm_type;
		sm->sm_id = sm_id;

		sm->sm_proc = sm_proc;
		if(sm_text) {
			memcpy(&(sm->sm_text), sm_text, MAX_SM_TEXT_LEN);
			sm->sm_text[MAX_SM_TEXT_LEN - 1] = 0;
		}
		sm_add_smallwin_list(sm);
		push_message((HSM)sm, SMM_CREATE, 0, 0);
	}
	unlock_system();
	return (HSM)sm;
}

void sm_destroy_smallwin(HSM hsm, s32 message_flag)
{
	if(!hsm)
		return;

	lock_system();
    if(smp_common(hsm)->prev)
        smp_common(hsm)->prev->next = smp_common(hsm)->next;
    else
        smallwin_list = smp_common(hsm)->next;
    if(smp_common(hsm)->next)
        smp_common(hsm)->next->prev = smp_common(hsm)->prev;

	erase_message(hsm); 
	sk_mem_free(smp_common(hsm));
	unlock_system();
}



static void smallwin_scheduler(void *param)
{
    HSM		hsm;
    u32		message,
            param1,
            param2;


	while(1)
	{
        if(pop_message(&hsm, &message, &param1, &param2)) {
            if (hsm) {
                run_sm(hsm, message, param1, param2);
            }
        }
		sk_task_delay(1);
	}
}


HSM sm_get_smallwin_list(void)
{
	return ((HSM)smallwin_list);
}

sk_task_id_t SmallwinSchedulerTask = 0;

void run_smallwin(void)
{
	sk_status_code_t recod = SK_FAILED;
	init_msgq(SMALLWIN_NMSGQ);
	init_global();
	sm_tick_init();

	recod = sk_task_create(&SmallwinSchedulerTask,"sm",smallwin_scheduler, NULL,NULL,SK_TASK_DEFAULT_STACK_SIZE,SK_TASK_DEFAULT_PRIORITY);
	if(SmallwinSchedulerTask==0)
	{
		SK_ERROR(("SMALLWIN smallwin_scheduler START FAIL.............\n\r"));
	}
	SK_DEBUG(("SMALLWIN smallwin_scheduler PROCESS STARTED ...........\n"));
}
