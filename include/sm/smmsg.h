/*
 *	SMMSG.H				ver 1.0
 *
 * (c)	Copyright  Federal Technologies Limited	2001 - 2002
 *
 *    	Source file name	: smmsg.h
 *
 *    	AUTHOR (S) NAME	: MDR Jan. 2001
 *
 *    	Original Work		:
 *						  SmallWin Message Definitions and Message APIs
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

#ifndef SMMSG_H
#define SMMSG_H


#include "smtype.h"
#include "sk_os.ho"

#define MAX_MESSAGE_QUEUE		20

typedef struct{
	HSM		hsm;
	u32		message;
	u32		param1;
	u32		param2;
} MESSAGE;

typedef struct{
	sk_sem_id_t	access;
	s16			head;
	s16			tail;
	MESSAGE		*queue;
} MSGQ;

extern void init_msgq(s32 nqueue);
extern s32 push_message(HSM hsm, u32 message, u32 param1, u32 param2);
extern s32 pop_message(HSM *hsm, u32 *message, u32 *param1, u32 *param2);
extern s32 is_message(HSM hsm);
extern s32 is_message_param(HSM hsm, u32 message, u32 param1, u32 param2);
extern s32 get_nmessage(void);
extern void empty_msgq(void);
extern void invalidate_message(s32 message);
extern void erase_message(HSM hsm);
void erase_message_type(HSM hsm, u32 message);

extern void init_message_control_process(void);
extern void set_message_control(
	HSM hsm,
	MESSAGE *pmessage_array,
	s32 message_num,
	s32 first_wait_time,
	s32 wait_time);

#endif


