#ifndef _PRIV_MAC_H_
#define _PRIV_MAC_H_

#include "sk_os.h"
#include "sk_tbx.h"

#include <pthread.h>
#include <semaphore.h>

#include <errno.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sched.h>
#include <limits.h>
#include <signal.h>

//#define MAX_OS_NAME_NUM  15

#define IS_USE_FLAGE      1
#define NO_USE_FLAGE	0



typedef struct TAG_SKYAPI_EVENT_T 
{
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	U8 state;
} skyapi_event_t;

typedef struct MsgQ_T 
{
	U32 magicnum;
	U32 mq_maxmsg;
	U32 mq_msgsize;
	U32 refcnt;

	pthread_mutex_t mutex;
	pthread_cond_t read_cond;
	pthread_cond_t write_cond;

	U32 read, write;
	U8 msgbuffer[0];
}MsgQ;



#define COMMON_OS_STR					\
	U8 is_use;   							\
	char str_name[MAX_OS_NAME_NUM];

typedef struct TAG_SKY_OS_COMMON_INFO
{
	COMMON_OS_STR
}sky_os_common_info_t;

typedef struct TAG_SKY_OS_THREAD_INFO
{
	COMMON_OS_STR
	pthread_t thread_id;
	sk_task_pfn_t    task_fun;
	U32 task_pid;
	U32 task_pro;
	U32 task_sk_id;
	sk_task_status_t task_status;
}sky_os_thread_info_t;

typedef struct TAG_SKY_OS_SEM_INFO
{
	COMMON_OS_STR
	sem_t sem_id;
}sky_os_sem_info_t;

typedef struct TAG_SKY_OS_EVENT_INFO
{
	COMMON_OS_STR
	skyapi_event_t event_id;
}sky_os_event_info_t;

typedef struct TAG_SKY_OS_QUE_INFO
{
	COMMON_OS_STR
	//mqd_t que_id;
	MsgQ que_id;
}sky_os_que_info_t;

/*********************************************************
* ����
*********************************************************/
#define DEF_OS_INFO(source,num)  							\
	static int os_##source##_info_num=num;					\
	static sem_t sem_##source##_id; 							\
	static sky_os_##source##_info_t os_##source##_info[num];

#define INIT_OS_INFO(source)						\
void init_os_##source##_info()							\
{ 													\
	int i;												\
	sem_init(&sem_##source##_id, 0, 1);   				\
	for(i=0;i<os_##source##_info_num;i++)				\
	{												\
		os_##source##_info[i].is_use = NO_USE_FLAGE;	\
	}												\
}

#define GET_FREE_NODE(source)										\
static sky_os_common_info_t* get_free_##source##_info()					\
{																	\
	int i;																\
	sem_wait(&sem_##source##_id);									\
	for(i=0;i<os_##source##_info_num;i++)								\
	{																\
		if(NO_USE_FLAGE == os_##source##_info[i].is_use)				\
		{															\
			os_##source##_info[i].is_use = IS_USE_FLAGE;				\
			sem_post(&sem_##source##_id);							\
			return (sky_os_common_info_t*)&(os_##source##_info[i]);		\
		}															\
	}																\
	sem_post(&sem_##source##_id);									\
	return (sky_os_common_info_t*)NULL;								\
}

#define SET_FREE_NODE(source)								\
static void set_free_##source##_info(sky_os_common_info_t* pInfo)	\
{															\
	if(pInfo == NULL)											\
	{														\
		return ;												\
	}														\
	if(pInfo->is_use == NO_USE_FLAGE)							\
		return ;												\
	sem_wait(&sem_##source##_id);							\
	pInfo->is_use=NO_USE_FLAGE;								\
	sem_post(&sem_##source##_id);							\
}

#define SET_NAME_NODE(source)											\
static sk_status_code_t set_name_##source##_info(sky_os_common_info_t* pInfo, char* name)	\
{																		\
	if(pInfo == NULL)											\
	{														\
		return SK_FAILED;								\
	}														\
	if(pInfo->is_use == NO_USE_FLAGE)							\
		return SK_FAILED	;								\
	sem_wait(&sem_##source##_id);							\
	memset(pInfo->str_name,0,MAX_OS_NAME_NUM);				\
	strncpy(pInfo->str_name, name, MAX_OS_NAME_NUM-1);		\
	sem_post(&sem_##source##_id);							\
	return SK_SUCCESS;									\
}

#define GET_NAME_NODE(source)													\
static sk_status_code_t get_name_##source##_info(sky_os_common_info_t* pInfo, char* name)		\
{																				\
	if(pInfo == NULL)											\
	{														\
		return SK_FAILED;								\
	}														\
	if(pInfo->is_use == NO_USE_FLAGE)							\
		return SK_FAILED	;								\
	sem_wait(&sem_##source##_id);							\
	strncpy(name, pInfo->str_name,MAX_OS_NAME_NUM);			\
	sem_post(&sem_##source##_id);							\
	return SK_SUCCESS;									\
}

#define TEST_NODE_IS_USE(source)							\
static U8 node_is_use_##source##_info(sky_os_common_info_t* pInfo)				\
{															\
	if(pInfo == NULL)											\
	{														\
		return NO_USE_FLAGE;								\
	}														\
	return pInfo->is_use;										\
}

#define LOCK_SOURCE(source)	       sem_wait(&sem_##source##_id)

#define UNLOCK_SOURCE(source)	sem_post(&sem_##source##_id)

void maketimespec(struct timespec * pSpec, U32 u32TimeOutMs);

#endif	

