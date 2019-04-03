
#include <memory.h>
#include <unistd.h>
#include <sys/syscall.h>
#include "priv_mac.h"
#include "sk_tbx.h"
#include <sys/prctl.h>

/**************************************
*
**************************************/
#define MAX_THREAD_COUNT 80

DEF_OS_INFO(thread,MAX_THREAD_COUNT);
INIT_OS_INFO(thread);
GET_FREE_NODE(thread);
SET_FREE_NODE(thread);
SET_NAME_NODE(thread);
TEST_NODE_IS_USE(thread);

static sk_status_code_t sk_check_task_name(const char *ptaskname);

static pid_t com_task_gettid(void);

static sky_os_thread_info_t* get_task_info(const sk_task_id_t * task_id)
{
	int i;
	
	if(task_id == NULL)
		return NULL;
	for(i=0;i<MAX_THREAD_COUNT;i++)
	{
		if(os_thread_info[i].task_sk_id == *task_id)
		{
			return &(os_thread_info[i]);
		}
	}
	return NULL;
}

void sk_task_thread_exit_handler(int sig)
{
	SK_DEBUG(("sk_task_thread_exit_handler $$$$$$$$$$$$$$$$$$$$$$$$$$$"));
	pthread_exit(0);
}



void sk_task_thread_sigaction(void)
{
	static int sigflag=0;
	struct sigaction actions; 

	if(sigflag==0)
	{
		memset(&actions, 0, sizeof(actions));
		sigemptyset(&actions.sa_mask);
		actions.sa_flags = 0;
		actions.sa_handler = sk_task_thread_exit_handler;
		sigaction(SIGUSR1,&actions,NULL);
		sigflag=1;
	}
}

/**********************************************************************************
*linux 系统的线程如果采用OTHER模式，没有优先级可以调整
*因此加入了这个函数通过task的优先级来修改nice的值.
**********************************************************************************/
static void *wrap_task_fun(void* param)
{
	pthread_t self;	 
	int i=0; 		
	S32 pro =0;
	int ret = 0;
	int oldtype = 0;

	pid_t idThread = SK_INVALID_ID;

	self = pthread_self();
	
	while(1)
	{
		LOCK_SOURCE(thread);
		
		for(i=0;i<MAX_THREAD_COUNT;i++)
		{
		
			if(pthread_equal( self, os_thread_info[i].thread_id) && os_thread_info[i].is_use == IS_USE_FLAGE)
			{			
				//ur_task_id = (sk_task_id_t)&(os_thread_info[i]);
				idThread = com_task_gettid();
				os_thread_info[i].task_pid = idThread;
				pro =0;
				if(os_thread_info[i].task_pro != SK_TASK_DEFAULT_PRIORITY)
				{
					/* linux 的nice 值从-20 到20 负值表示占用的cpu的时间片越长. */
					pro = ((S32)(40*(os_thread_info[i].task_pro-SK_TASK_DEFAULT_PRIORITY))/(SK_TASK_PRIORITY_HIGHEST - SK_TASK_PRIORITY_LOWEST))*(-1);	

					ret = setpriority(PRIO_PROCESS, idThread, pro);
					if(ret != 0)
					{
						SK_ERROR(("can not set task name %s pro %ld", os_thread_info[i].str_name, pro));
					}					
				}
				
				SK_DEBUG(("the task_name: %s task_id: %lu task_pro: %lu pro: %ld thread_id: %d", 
														os_thread_info[i].str_name,
														os_thread_info[i].task_pid, 
														os_thread_info[i].task_pro,
														pro, 
														idThread));
				
				UNLOCK_SOURCE(thread);	

				#ifndef SK_ANDROID
				pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldtype);
				pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &oldtype);
				#endif
				
				prctl(PR_SET_NAME, os_thread_info[i].str_name, 0, 0, 0);
				
				if(os_thread_info[i].task_fun != NULL)
				{				
					os_thread_info[i].task_fun(param);
				}
				//return ;
				set_free_thread_info((sky_os_common_info_t*)&os_thread_info[i]);	
				pthread_detach(pthread_self());
				pthread_exit(NULL);
				return NULL;
			}
		}
		
		UNLOCK_SOURCE(thread);
		SK_ERROR(("failed to get cur thread fun pid %d self %ld ",getpid(), self));
		sk_task_delay(10);
	}
	
/*lint -e527*/
	return NULL;
/*lint +e527*/
}

sk_status_code_t sk_task_create(sk_task_id_t * const p_task_id, 
						const char * p_task_name, 
						const sk_task_pfn_t task_pfn, 
						void * p_param,
						void * p_stack, 
						U32 stack_size,
						U32 priority)
{
	static U32 pid = 0;
	sky_os_thread_info_t *pInfo;
	int ret = 0;
	pthread_attr_t thread_attr;
	int prioritys;
	
	//int min_priority,max_priority;	
	//struct sched_param scheduling_value;
	//sk_task_id_t tmp_id;

	static U32 count = 0;
	
	if(p_task_id == NULL)
	{
		SK_ERROR(("task id is null."));
		return SK_ERROR_BAD_PARAMETER;
	}
	
	if(SK_SUCCESS!=sk_check_task_name(p_task_name))
	{
		SK_ERROR(("task name error."));
		return SK_ERROR_BAD_PARAMETER;
	}
	
	*p_task_id = SK_INVALID_ID;	

	prioritys = priority;	
	
	if(priority >SK_TASK_PRIORITY_HIGHEST)
		prioritys = SK_TASK_PRIORITY_HIGHEST;
	else if(priority < SK_TASK_PRIORITY_LOWEST)
		prioritys = SK_TASK_PRIORITY_LOWEST;
	
	if(stack_size < SK_TASK_DEFAULT_STACK_SIZE)
		stack_size = SK_TASK_DEFAULT_STACK_SIZE;
	

	/*  init the thread prarm */
	ret = pthread_attr_init(&thread_attr);
	if(ret != 0)
	{
		SK_ERROR(("failed to init the thread attr."));
		return SK_FAILED;
	}	
#if 0
	ret = pthread_attr_setscope(&thread_attr, PTHREAD_SCOPE_SYSTEM);
	if(ret != 0)
	{
		SK_ERROR(("set scope failed."));
		return SK_FAILED;
	}
#endif

#if 0
	/* 设置线程为分离模式*/
	ret = pthread_attr_setdetachstate ( &thread_attr, PTHREAD_CREATE_DETACHED);
	if(ret != 0)
	{
		SK_ERROR(("set detachstate failed."));
		return SK_FAILED;
	}
#endif
#if 0
	ret = pthread_attr_setschedpolicy(&thread_attr, SCHED_RR);  /*SCHED_FIFO*/
	if (ret != 0) 
	{
		SK_ERROR(("set schedpolicy failed."));
		return SK_FAILED;
	}

	max_priority = sched_get_priority_max(SCHED_RR);
	min_priority = sched_get_priority_min(SCHED_RR);

	printf("max_priority  %d min_priority %d \n",max_priority, min_priority);
	priority = sPriority-20;	
	if(min_priority < priority)
		priority = min_priority;
	else if(max_priority > priority)
		priority = max_priority;

	//priority = (max_priority sPriority
	pthread_attr_getschedparam(&thread_attr, &scheduling_value);
	scheduling_value.sched_priority = priority;
	ret = pthread_attr_setschedparam(&thread_attr, &scheduling_value);
	printf("the ret = %d, error =%d \n", ret, scheduling_value.sched_priority);
	if (ret != 0) 
	{
		//printf("the ret = %d, error =%d \n", ret, scheduling_value.sched_priority);
		SK_ERROR(("set schedparam failed."));
		return SK_FAILED;
	}
#endif

	ret = pthread_attr_setstacksize(&thread_attr,stack_size);
	if (ret != 0) 
	{
		SK_ERROR(("set static size  failed."));
		pthread_attr_destroy(&thread_attr);
		return SK_FAILED;
	}


	pInfo = (sky_os_thread_info_t*)get_free_thread_info();
	if(pInfo == NULL)
	{		
		SK_ERROR(("not free node of thread."));
		pthread_attr_destroy(&thread_attr);
		set_free_thread_info((sky_os_common_info_t*)pInfo);
		return SK_FAILED;
	}
	if(NULL == p_task_name)
	{
		char tmpname [256];
		sprintf(tmpname,"%s-%ld","tasknm",count);
		count++;
		set_name_thread_info((sky_os_common_info_t*)pInfo,tmpname);	
	}
	else
	{
		set_name_thread_info((sky_os_common_info_t*)pInfo,(char *)p_task_name);	
	}
	
	LOCK_SOURCE(thread);
	pInfo->task_fun = task_pfn;
	pInfo->task_pro = prioritys;
	ret = pthread_create( &(pInfo->thread_id),&thread_attr,/*pFunPtr*/wrap_task_fun, p_param );	
	if(ret != 0)
	{		
		SK_ERROR(("failed to create thread. %d", ret));
		pthread_attr_destroy(&thread_attr);
		UNLOCK_SOURCE(thread);
		set_free_thread_info((sky_os_common_info_t*)pInfo);
		return SK_FAILED;
	}

	pid++;
	pInfo->task_sk_id = pid;
	pInfo->task_status = SK_TASK_STATUS_RUNNING;
	//*p_task_id = (sk_task_id_t)(pInfo);
	*p_task_id = (sk_task_id_t)pInfo->task_sk_id;
	
	SK_DEBUG(("create the task: %s priority: %d sk_pid: %ld",
											pInfo->str_name,
											prioritys, 
											*p_task_id));
	
	UNLOCK_SOURCE(thread);
	
	pthread_attr_destroy(&thread_attr);	
	#ifdef SK_ANDROID
	sk_task_thread_sigaction();
	#endif
	return SK_SUCCESS;
}


sk_status_code_t sky_task_Id(char* name, sk_task_id_t *task_id)
{
	int i=0; 	

	if(name == NULL || task_id == NULL)
		return SK_FAILED;
	*task_id = SK_INVALID_ID;
	
	LOCK_SOURCE(thread);
	
	for(i=0;i<MAX_THREAD_COUNT;i++)
	{
	
		if(strcmp(os_thread_info[i].str_name , name) == 0)
		{			
			
			//*task_id = (sk_task_id_t)&(os_thread_info[i]);	
			*task_id = (sk_task_id_t)(os_thread_info[i].task_sk_id);
			//SK_DEBUG(("the task name %s task id is %d ", os_thread_info[i].str_name,os_thread_info[i].task_pid));
			UNLOCK_SOURCE(thread);	
			return SK_SUCCESS;
			
		}
	}	
	UNLOCK_SOURCE(thread);
	SK_ERROR(("not task name %s ", name));
	return SK_FAILED;
}


sk_status_code_t sk_task_cur_id(sk_task_id_t * const p_cur_task_id)	
{
	pthread_t self;
	int i=0;

	if(p_cur_task_id == NULL)
	{
		SK_ERROR(("failed to get cur thread id ."));
		return SK_ERROR_BAD_PARAMETER;
	}
	
	self = pthread_self();
	
	LOCK_SOURCE(thread);
	
	for(i=0;i<MAX_THREAD_COUNT;i++)
	{
	
		if(pthread_equal( self, os_thread_info[i].thread_id) && os_thread_info[i].is_use == IS_USE_FLAGE)
		{
			
			//*p_cur_task_id = (sk_task_id_t)&(os_thread_info[i]);
			*p_cur_task_id = (sk_task_id_t)(os_thread_info[i].task_sk_id);
			UNLOCK_SOURCE(thread);
	
			return SK_SUCCESS;
		}
	}
	
	UNLOCK_SOURCE(thread);
	SK_ERROR(("failed to get cur thread id ."));
	*p_cur_task_id = SK_INVALID_ID;
	return SK_FAILED;

}

sk_status_code_t sk_task_term(void)
{

	sk_task_id_t cur_task_id;
	sky_os_thread_info_t *info = NULL;
	
	if(sk_task_cur_id(&cur_task_id) != SK_SUCCESS)
	{
		SK_ERROR(("not find cur thread id ."));		
		return SK_ERROR_INVALID_ID;
	}	

	LOCK_SOURCE(thread);	
	info =  get_task_info(&cur_task_id);
	if(info == NULL)
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("not find thread id 0x%lx.", cur_task_id));		
		return SK_ERROR_INVALID_ID;
	}
	info->is_use=NO_USE_FLAGE;
	
	SK_DEBUG(("task %s pid is %ld will exit \n", info->str_name, info->task_sk_id));
	
	UNLOCK_SOURCE(thread);	
	pthread_detach(pthread_self());
	pthread_exit(NULL);		
	SK_DEBUG(("task exit ok. \n"));

	return SK_SUCCESS;
}

sk_status_code_t sk_task_kill(const sk_task_id_t  task_id)
{	
	return SK_SUCCESS;
}

sk_status_code_t sk_task_wait(const sk_task_id_t  task_id)
{
	return sk_task_wait_timeout(task_id,SK_TIMEOUT_INFINITY);
}

sk_status_code_t sk_task_wait_timeout(const sk_task_id_t  task_id, const U32 ms)
{
	sky_os_thread_info_t* task_info = NULL;
	//void* res;
	pthread_t pth_t;
	
	LOCK_SOURCE(thread);
	task_info = get_task_info(&task_id);
	
	if(task_info == NULL)
	{
		UNLOCK_SOURCE(thread);
		return SK_ERROR_BAD_PARAMETER;
	}
	if(NO_USE_FLAGE == node_is_use_thread_info((sky_os_common_info_t*)task_info))
	{
		UNLOCK_SOURCE(thread);
		return SK_FAILED;
	}
	pth_t = task_info->thread_id;
	UNLOCK_SOURCE(thread);

#ifndef SK_ANDROID
	if ( 0 != pthread_cancel(pth_t))
	{
		SK_ERROR(("kill task id 0x%lx failed. ", task_id));
        	return SK_FAILED;
	}
#else
	if ( 0 !=pthread_kill(pth_t, SIGUSR1))  //SIGUSR1默认是终止线程
	{
		SK_ERROR(("kill task id 0x%lx failed. ", task_id));
        	return SK_FAILED;
	}
#endif

	#if 0
	if ( 0 != pthread_join(pth_t,res))
	{
		SK_ERROR(("the task join id 0x%x failed ", task_id));
		return SK_FAILED;
	}
	#endif
	
	return SK_SUCCESS;
	
}

sk_status_code_t sk_task_destroy(const sk_task_id_t  task_id)
{
	sky_os_thread_info_t* task_info = NULL;
	pthread_t pth_t;
	void* res = NULL;
	
	LOCK_SOURCE(thread);
	task_info = get_task_info(&task_id);
	
	if(task_info == NULL)
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("the task id is null"));
		return SK_ERROR_BAD_PARAMETER;
	}
	if(NO_USE_FLAGE == node_is_use_thread_info((sky_os_common_info_t*)task_info))
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("the task id is not use"));
		return SK_FAILED;
	}

	task_info->is_use=NO_USE_FLAGE;	
	pth_t = task_info->thread_id;
	
	UNLOCK_SOURCE(thread);


	if ( 0 != pthread_join(pth_t,&res))
	{
		SK_ERROR(("kill task id 0x%lx failed. ", task_id));
        	return SK_FAILED;
	}


	return SK_SUCCESS;
}

sk_status_code_t sk_task_set_priority(const sk_task_id_t  task_id, const U32 priority)
{
	sky_os_thread_info_t* pstuTaskInfo = NULL;
	S32 pro =0;
	int ret = 0;
	U32 prioritys;
	

	LOCK_SOURCE(thread);
	
	pstuTaskInfo = get_task_info(&task_id);
	if(NULL == pstuTaskInfo)
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("get_task_info failed. task_id: %ld", task_id));
		return SK_ERROR_BAD_PARAMETER;
	}

	if(priority == pstuTaskInfo->task_pro)
	{
		SK_ERROR(("set priority is same task_id: %lu,priority:%lu", task_id,priority));
		UNLOCK_SOURCE(thread);
		return SK_SUCCESS;
	}
	else
	{
		prioritys = priority;	
	
		if(priority >SK_TASK_PRIORITY_HIGHEST)
			prioritys = SK_TASK_PRIORITY_HIGHEST;
		else if(priority < SK_TASK_PRIORITY_LOWEST)
			prioritys = SK_TASK_PRIORITY_LOWEST;

		pro = ((S32)(40*(prioritys-SK_TASK_DEFAULT_PRIORITY))/(SK_TASK_PRIORITY_HIGHEST - SK_TASK_PRIORITY_LOWEST))*(-1);	

		ret = setpriority(PRIO_PROCESS, pstuTaskInfo->task_pid, pro);
		if(ret != 0)
		{
			SK_ERROR(("can not set task name %s pro %ld", pstuTaskInfo->str_name, pro));
			UNLOCK_SOURCE(thread);
			return SK_FAILED;
		}
		pstuTaskInfo->task_pro = prioritys;
	}

	UNLOCK_SOURCE(thread);

	return SK_SUCCESS;
}

sk_status_code_t sk_task_get_priority(const sk_task_id_t  task_id, U32 * const  p_priority)
{
	sky_os_thread_info_t* task_info = NULL;
	pthread_t pth_t;	
	
	LOCK_SOURCE(thread);
	task_info = get_task_info(&task_id);
	if(task_info == NULL)
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("get_task_info failed. task_id: %ld", task_id));
		return SK_ERROR_BAD_PARAMETER;
	}

	UNLOCK_SOURCE(thread);

	SK_ERROR(("get_task_info success. task_id: %ld", task_id));
	
	return SK_ERROR_FEATURE_NOT_SUPPORTED;
}

sk_status_code_t sk_task_suspend(const sk_task_id_t  task_id)
{
	sky_os_thread_info_t* task_info = NULL;
	pthread_t pth_t;	
	
	LOCK_SOURCE(thread);
	task_info = get_task_info(&task_id);
	
	if(task_info == NULL)
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("suppend task id failed. "));
		return SK_ERROR_BAD_PARAMETER;
	}
	if(NO_USE_FLAGE == node_is_use_thread_info((sky_os_common_info_t*)task_info))
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("suppend task id 0x%lx failed. ", task_id));
		return SK_FAILED;
	}
	if(task_info->task_status != SK_TASK_STATUS_RUNNING)
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("suppend task id 0x%lx failed. ", task_id));
		return SK_SUCCESS;
	}

	
	pth_t = task_info->thread_id;
	task_info->task_status = SK_TASK_STATUS_SUPENDED;
	UNLOCK_SOURCE(thread);

	if ( 0 != pthread_kill( pth_t , SIGSTOP ) )
    	{
		SK_ERROR(("suppend task id 0x%lx failed. ", task_id));
        	return SK_FAILED;
    	}	

	return SK_SUCCESS;	
}

sk_status_code_t sk_task_resume(const sk_task_id_t  task_id)
{
	sky_os_thread_info_t* task_info = NULL;
	pthread_t pth_t;
	
	LOCK_SOURCE(thread);
	task_info = get_task_info(&task_id);
	
	if(task_info == NULL)
	{
		UNLOCK_SOURCE(thread);
		return SK_ERROR_BAD_PARAMETER;
	}
	if(NO_USE_FLAGE == node_is_use_thread_info((sky_os_common_info_t*)task_info))
	{
		UNLOCK_SOURCE(thread);
		return SK_FAILED;
	}
	if(task_info->task_status != SK_TASK_STATUS_SUPENDED)
	{
		UNLOCK_SOURCE(thread);
		return SK_SUCCESS;
	}

	
	pth_t = task_info->thread_id;
	task_info->task_status = SK_TASK_STATUS_RUNNING;
	UNLOCK_SOURCE(thread);

	if ( 0 != pthread_kill( pth_t , SIGCONT ) )
    	{
		SK_ERROR(("resume task id 0x%lx failed. ", task_id));
        	return SK_FAILED;
    	}	

	return SK_SUCCESS;	
}

sk_status_code_t sk_task_get_status(const sk_task_id_t  task_id,  sk_task_status_t * const p_status)
{
	sky_os_thread_info_t* task_info = NULL;
	//pthread_t pth_t;

	if( p_status == NULL)
		return SK_ERROR_BAD_PARAMETER;
	
	LOCK_SOURCE(thread);
	task_info = get_task_info(&task_id);
	
	if(task_info == NULL)
	{
		UNLOCK_SOURCE(thread);
		return SK_ERROR_BAD_PARAMETER;
	}
	if(NO_USE_FLAGE == node_is_use_thread_info((sky_os_common_info_t*)task_info))
	{
		UNLOCK_SOURCE(thread);
		return SK_FAILED;
	}
	
	*p_status = task_info->task_status;
	UNLOCK_SOURCE(thread);

	return SK_SUCCESS;	
}

sk_status_code_t sk_task_delay(const U32  ms)
{
	#if 0
	usleep(u32Ms*1000);
	#else
	struct   timeval   tv; 
	tv.tv_sec   =   ms/1000;   
       tv.tv_usec   =   (ms%1000)*1000; 
       select(0,   NULL,   NULL,   NULL,   &tv);     
	#endif
	return SK_SUCCESS;
}

void sk_task_dump(void)
{
	int i;
	for(i=0;i<MAX_THREAD_COUNT;i++)	
	{
		if(os_thread_info[i].is_use == IS_USE_FLAGE)
		{
			SK_DEBUG(("task: %s pid: %ld sk_pid: %ld pro: %ld ",
								os_thread_info[i].str_name, 
								os_thread_info[i].task_pid,
								os_thread_info[i].task_sk_id,
								os_thread_info[i].task_pro));
		}
	}
}

#if 0  //增加了kill的实现，等待其他模块上传后，才能使用这部分代码替换上面的实现
sk_status_code_t sk_task_kill(const sk_task_id_t  task_id)
{
	sky_os_thread_info_t* task_info = NULL;
	//void* res;
	pthread_t pth_t;
	
	LOCK_SOURCE(thread);
	task_info = get_task_info(&task_id);
	
	if(task_info == NULL)
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("the task id is null"));
		return SK_ERROR_BAD_PARAMETER;
	}
	if(NO_USE_FLAGE == node_is_use_thread_info((sky_os_common_info_t*)task_info))
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("the task id is not use"));
		return SK_FAILED;
	}
	pth_t = task_info->thread_id;
	UNLOCK_SOURCE(thread);

	if ( 0 != pthread_cancel(pth_t))
	{
		SK_ERROR(("kill task id 0x%lx failed. ", task_id));
        	return SK_FAILED;
	}

	#if 0
	if ( 0 != pthread_join(pth_t,res))
	{
		SK_ERROR(("the task join id 0x%x failed ", task_id));
		return SK_FAILED;
	}
	#endif
	
	return SK_SUCCESS;
}

sk_status_code_t sk_task_wait(const sk_task_id_t  task_id)
{
	return sk_task_wait_timeout(task_id,SK_TIMEOUT_INFINITY);
}

sk_status_code_t sk_task_wait_timeout(const sk_task_id_t  task_id, const U32 ms)
{
	sky_os_thread_info_t* task_info = NULL;
	pthread_t pth_t;
	void* res = NULL;
	
	LOCK_SOURCE(thread);
	task_info = get_task_info(&task_id);
	
	if(task_info == NULL)
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("the task id is null"));
		return SK_ERROR_BAD_PARAMETER;
	}
	if(NO_USE_FLAGE == node_is_use_thread_info((sky_os_common_info_t*)task_info))
	{
		UNLOCK_SOURCE(thread);
		SK_ERROR(("the task id is not use"));
		return SK_FAILED;
	}

	task_info->is_use=NO_USE_FLAGE;	
	pth_t = task_info->thread_id;
	
	UNLOCK_SOURCE(thread);

	if ( 0 != pthread_join(pth_t,&res))
	{
		SK_ERROR(("kill task id 0x%lx failed. ", task_id));
        	return SK_FAILED;
	}
	return SK_SUCCESS;	
}

sk_status_code_t sk_task_destroy(const sk_task_id_t  task_id)
{
	return SK_SUCCESS;
}
#endif

/*检查任务名称，当发现重名task则返回失败*/
static sk_status_code_t sk_check_task_name(const char *ptaskname)
{
	int i;
	
	if(ptaskname == NULL)
	{
		SK_ERROR(("task name is null "));
		return SK_ERROR_BAD_PARAMETER;
	}
	
	if(strlen(ptaskname)>=MAX_OS_NAME_NUM)
	{
		SK_ERROR(("task name size error tasknamesize=%d,MAX_OS_NAME_NUM=%d",strlen(ptaskname),MAX_OS_NAME_NUM));
		return SK_ERROR_BAD_PARAMETER;
	}
	
	for(i=0;i<MAX_THREAD_COUNT;i++)
	{
		//SK_ERROR(("thread_info[%d]_name=%s,ptaskname=%s",i,os_thread_info[i].str_name,ptaskname));
		if((os_thread_info[i].is_use==IS_USE_FLAGE)&&
			(strcmp(os_thread_info[i].str_name,ptaskname)==0))
		{
			SK_ERROR(("task name: %s is duplicate", ptaskname));
			return SK_ERROR_BAD_PARAMETER;
		}
	}
	return SK_SUCCESS;
}

static pid_t com_task_gettid(void)
{
	return syscall(SYS_gettid);
//	return syscall(__NR_gettid);
}

