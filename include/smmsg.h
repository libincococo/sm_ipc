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

#include "sk_os.h"

#define MAX_MESSAGE_QUEUE		20
#define MAX_SM_TEXT_LEN 		32

#define SMM_SYS					0x00
#define SMM_PRIVATE				0x10000000
#define SMM_COMMON				0x20000000
#define SMM_USER				0x80000000
#define SMM_INVALIDMSG			0
#define SMM_CREATE				(SMM_SYS + 1)
#define SMM_CLOSE				(SMM_SYS + 2)
#define SMM_TICK				(SMM_COMMON + 2)

typedef unsigned char			u8;
typedef signed char				s8;
typedef unsigned short int		u16;
typedef signed short int		s16;
typedef unsigned int			u32;
typedef signed int				s32;

typedef u32						HSM;
typedef u32						(*SM_PROC)(HSM, u32, u32, u32);

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

#define COMMON_SMALLWIN_ELEMENTS								\
	struct tag_common_smallwin		*prev;						\
	struct tag_common_smallwin		*next;						\
	u16							sm_type;						\
	u16							sm_id;							\
	char						sm_text[MAX_SM_TEXT_LEN];		\
	SM_PROC						sm_proc;						\

typedef struct tag_common_smallwin {
	COMMON_SMALLWIN_ELEMENTS
} COMMON_SM;

#define smp_common(hsm)			((COMMON_SM*)(hsm))

void    run_smallwin(void);

void 	init_msgq(s32 nqueue);
void 	send_message(HSM hsm, u32 message, u32 param1, u32 param2);
s32 	push_message(HSM hsm, u32 message, u32 param1, u32 param2);
s32 	pop_message(HSM *hsm, u32 *message, u32 *param1, u32 *param2);
s32 	is_message(HSM hsm);
s32 	is_message_param(HSM hsm, u32 message, u32 param1, u32 param2);
s32 	get_nmessage(void);

void 	invalidate_message(s32 message);
void 	erase_message(HSM hsm);

void 	sm_tick_init(void);
s32 	sm_create_tick(HSM hsm, u32 n100msec);
void 	sm_destroy_tick(s32 htick);


#endif


