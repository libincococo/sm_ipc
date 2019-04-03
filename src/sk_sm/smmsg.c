/*
 *	 SMMSG.C							ver 1.0
 *
 * (c)	Copyright  Federal Technologies Limited	2001 - 2002
 *
 *    	Source file name	: smmsg.c
 *
 *    	AUTHOR (S) NAME	: KBA, Feb.2001
 *
 *    	Original Work		:
 *						SmallWin Message engine & API.
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
#include "smstruct.h"
#include "smmacro.h"
#include "smglobal.h"
#include "smmsg.h"
#include "sk_os.h"

static MSGQ		message_queue;
static s32		queue_size;

void init_msgq(s32 nqueue)
{
	queue_size = nqueue + 1;

	message_queue.head = message_queue.tail = 0;
	message_queue.queue = (MESSAGE*)sk_mem_malloc(queue_size * sizeof(MESSAGE));
	sk_sem_create(&(message_queue.access), "msgq",1);
}

s32 push_message(HSM hsm, u32 message, u32 param1, u32 param2)
{
	s32		return_value = 1,
			def;

	if(hsm == 0) {
		return 0;
	}
	sk_sem_lock(message_queue.access);

	def = message_queue.head - message_queue.tail;
	if ((def != 1) && (def != -(queue_size - 1))) {
		message_queue.queue[message_queue.tail].hsm = hsm;
		message_queue.queue[message_queue.tail].message = message;
		message_queue.queue[message_queue.tail].param1 = param1;
		message_queue.queue[message_queue.tail].param2 = param2;
		signal_event();
		message_queue.tail++;
		if(message_queue.tail == queue_size)
			message_queue.tail = 0;
	}
	else
	{
		return_value = 0;
		SK_DEBUG("......Message push ..... Error \n");
		SK_DEBUG("......message...h=%d, t=%d, qsize=%d ",message_queue.head, message_queue.tail, queue_size);
	}
	sk_sem_unlock(message_queue.access);
	return return_value;
}

s32 pop_message(HSM *hsm, u32 *message, u32 *param1, u32 *param2)
{
	s32	return_value;

	SMMSG_ASSERT(hsm);
	SMMSG_ASSERT(message);
	SMMSG_ASSERT(param1);
	SMMSG_ASSERT(param2);
	if(hsm == 0) {
		return 0;
	}
	wait_event();
	sk_sem_lock(message_queue.access);

	if(message_queue.head != message_queue.tail) {
		*hsm = message_queue.queue[message_queue.head].hsm;
		*message = message_queue.queue[message_queue.head].message;
		*param1 = message_queue.queue[message_queue.head].param1;
		*param2 = message_queue.queue[message_queue.head].param2;

		message_queue.head++;
		if(message_queue.head == queue_size)
			message_queue.head = 0;
		return_value = 1;
	}
	else {
		return_value = 0;
		signal_event();
	}
	sk_sem_unlock(message_queue.access);
	return return_value;
}

s32 is_message(HSM hsm)
{
	s32		return_value = 0,
			i;

	sk_sem_lock(message_queue.access);

	i = message_queue.head;

	while(i != message_queue.tail) {
		if(message_queue.queue[i++].hsm == hsm) {
			return_value = 1;
			break;
		}
		if(i == queue_size)
			i = 0;
	}
	sk_sem_unlock(message_queue.access);
	return return_value;
}

s32 is_message_param(HSM hsm, u32 message, u32 param1, u32 param2)
{
	s32		return_value = 0,
			i;

	sk_sem_lock(message_queue.access);

	i = message_queue.head;

	while(i != message_queue.tail) {
		if(message_queue.queue[i].hsm == hsm)
		if(message_queue.queue[i].message == message)
		if(message_queue.queue[i].param1 == param1)
		if(message_queue.queue[i].param2 == param2) {
			return_value = 1;
			break;
		}
		i++;
		if(i == queue_size)
			i = 0;
	}
	sk_sem_unlock(message_queue.access);
	return return_value;
}

s32 get_nmessage(void)
{
	s32		return_value;

	if(message_queue.head == message_queue.tail)
		return_value = 0;
	else if(message_queue.head < message_queue.tail)
		return_value = message_queue.tail - message_queue.head;
	else
		return_value = (queue_size - message_queue.head) + message_queue.tail;

	return return_value;
}

void invalidate_message(s32 message)
{
	s32		i;

	sk_sem_lock(message_queue.access);
	for(i = 0; i < queue_size; i++) {
		if(message_queue.queue[i].message == message) {
			message_queue.queue[i].message = SMM_INVALIDMSG;
			message_queue.queue[i].param1 = message_queue.queue[i].param2 = 0;
		}
	}
	sk_sem_unlock(message_queue.access);
}

void erase_message(HSM hsm)
{
	s32		i;

	//////////assert_3(hsm);
	sk_sem_lock(message_queue.access);
	for(i = 0; i < queue_size; i++) {
		if(message_queue.queue[i].hsm == hsm) {
			message_queue.queue[i].hsm = 0;
			message_queue.queue[i].message = SMM_INVALIDMSG;
			message_queue.queue[i].param1 = message_queue.queue[i].param2 = 0;
		}
	}
	sk_sem_unlock(message_queue.access);
}

// add by libin 20060107
void erase_message_type(HSM hsm, u32 message)
{
	s32		i;

	//////////assert_3(hsm);
	sk_sem_lock(message_queue.access);
	for(i = 0; i < queue_size; i++) {
		if((message_queue.queue[i].hsm == hsm)&&(message_queue.queue[i].message == message)) {
			message_queue.queue[i].hsm = 0;
			message_queue.queue[i].message = SMM_INVALIDMSG;
			message_queue.queue[i].param1 = message_queue.queue[i].param2 = 0;
		}
	}
	sk_sem_unlock(message_queue.access);
}
