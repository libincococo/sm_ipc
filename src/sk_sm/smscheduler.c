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

#include "smallwin.h"
#include "smglobal.h"
#include "smgrp.h"

static COMMON_SM	*smallwin_list = NULL;


static void smallwin_scheduler(void *param);

//extern void sm_usif_start(U8 type);
extern void sm_tick_init(void);

void sm_add_smallwin_list(COMMON_SM *node)
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

	/*********************************************************************
	 *	ͨ��sizeof��struct������������� Ӧ���ʹ��ڻ�ؼ���������ڴ�ռ䡣
	 *	�����ڴ���亯��malloc()Ϊ������ڴ�ռ�
	 *********************************************************************/
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
		push_message((HSM)sm, SMM_CREATE, param1, param2);
	}
	unlock_system();
	return (HSM)sm;
}

static void sm_destroy_main_smallwin(HSM hsm, s32 message_flag)
{
	if(!hsm)
		return;
	if(smp_common(hsm)->prev)

	erase_message(hsm); 
	sk_mem_free(smp_common(hsm));
}

static void sm_destroy_child_smallwin(HSM hsm)
{
	COMMON_SM *sm = smp_common(hsm);

	if(sm->prev)
		sm->prev->next = sm->next;

	if(sm->next)
		sm->next->prev = sm->prev;

	sk_mem_free(smp_common(hsm));
}

static HSM sm_destroy_smallwin(HSM hsm, s32 message_flag)
{
	COMMON_SM *next;

	lock_system();
	next = smp_common(hsm)->next;
	if(smp_common(hsm)->sm_type & SMT_CHILD_BIT)
		sm_destroy_child_smallwin(hsm);
	else
		sm_destroy_main_smallwin(hsm, message_flag);
	unlock_system();
	return ((HSM)next);
}


static void smallwin_scheduler(void *param)
{
	while(1)
	{
		sm_process_message(0);
		sk_task_delay(1);
	}
}

void sm_process_message(HSM hsuspend_smallwin)
{
	HSM		hsm;
	u32		message,
			param1,
			param2;


	if(pop_message(&hsm, &message, &param1, &param2)) {
		if(hsm) {
			if(hsuspend_smallwin != hsm) {
				if(sm_is_smallwin(smp_common(hsm))) {
					switch(message) {
						case SMM_SYSPAINT:
						case SMM_PAINT:
							if(!smmq_paint(hsm))
								break;
						case SMM_KEY:
							if(smmq_nokey(hsm) && (message == SMM_KEY))
							{
								break;
							}
						default:
							//assert(fvalid_pointer((void *)hsm,sizeof(HSM)));
							if(hsm)
								if(run_sm(hsm, message, param1, param2) == 0) {
									if(smp_common(hsm)->sm_type == SMT_POPUP) {
										sm_destroy_popup((POPUP*)(popup_list));
										popup_list = NULL;
									}
									else if(smp_common(hsm)->sm_type == SMT_DIALOG) {
										sm_destroy_smallwin(hsm, 0);
									}
									else
										sm_destroy_smallwin(hsm, 1);
									erase_message(hsm);
								}
					}
				}
			}
			else switch(message) {
				case SMM_TICK:
					break;
				default:
					sm_express_message(hsm, message, param1, param2);
			}
		}
	}
}


void sm_express_message(HSM hsm, u32 message, u32 param1, u32 param2)
{

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
	init_message_control_process();
	init_global();
	sm_tick_init();

	recod = sk_task_create(&SmallwinSchedulerTask,"sm",smallwin_scheduler, NULL,NULL,SK_TASK_DEFAULT_STACK_SIZE,SK_TASK_DEFAULT_PRIORITY);
	if(SmallwinSchedulerTask==0)
	{
		SK_ERROR("SMALLWIN smallwin_scheduler START FAIL.............\n\r");
	}
	SK_DEBUG("SMALLWIN smallwin_scheduler PROCESS STARTED ...........\n");
}
