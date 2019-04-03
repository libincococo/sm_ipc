/*!
*@file sk_os.h	
*@brief sk_os api
* Version:	 0.0.1
* Description:  This file contains function declaration and some macro definition of OS abstract layer os 
*				for Skyworth .
* Copyright (c) 2009, SkyworthDTV
* All rights reserved.				
*@author  libin
*@date 2009-04-09 
*/

#ifndef SK_OS_H
#define SK_OS_H

#include "sk_std_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef SK_OS_LINUX
#define MAX_OS_NAME_NUM  60
#else
#define MAX_OS_NAME_NUM  30
#endif

/*!�߳����ȼ�*/
#define SK_TASK_DEFAULT_PRIORITY   (64)

#define SK_TASK_PRIORITY_LOWEST  	(SK_TASK_DEFAULT_PRIORITY-24) 	/*!<��͵����ȼ�*/
#define SK_TASK_PRIORITY_LOW    	(SK_TASK_DEFAULT_PRIORITY-12)	/*!<�ϵ͵����ȼ�*/
#define SK_TASK_PRIORITY_NORMAL 	(SK_TASK_DEFAULT_PRIORITY)		/*!<�͵����ȼ�*/
#define SK_TASK_PRIORITY_HIGH  		(SK_TASK_DEFAULT_PRIORITY+12)	/*!<�ߵ����ȼ�*/
#define SK_TASK_PRIORITY_HIGHEST  	(SK_TASK_DEFAULT_PRIORITY+24)	/*!<��ߵ����ȼ�*/


#ifdef SK_ANDROID  //android Ҳ����linux os����ô������android�ܶ�ط� ֻ����SK_ANDROID��δ����SK_OS_LINUX freeman
	#define SK_TASK_DEFAULT_STACK_SIZE	(64*1024)  /*androidջ��Ҫ���� ��С����Щ�쳣*/
#else
	#ifdef SK_OS_LINUX
	    #if defined (SK_CHIP_mstar7c51k) ||  defined(SK_CHIP_mstar7c51g) || defined(SK_OS_MSTAR_LINUX) || defined (SK_OS_ALI_LINUX)
			#define SK_TASK_DEFAULT_STACK_SIZE (128*1024)/*mstar linuxƽ̨ ��̬���task�Ĵ�����С�Ķ�ջ����С��128K*/
		#else
			#define SK_TASK_DEFAULT_STACK_SIZE	(24*1024)  /*��ֹջ̫С��ջ�������������������ð�ҳ������������ */
	    #endif
	#else
	/*!the define the task default stack size  */
		#define SK_TASK_DEFAULT_STACK_SIZE (18*1024)  /*!��linux�����ֵ����С��*/
	#endif
#endif

/*!��Ϣ����*/
#define SK_QUEUE_MSG_SIZE		4
typedef U32 sk_queue_msg_t[SK_QUEUE_MSG_SIZE];

typedef sk_id_t sk_task_id_t;
typedef sk_id_t sk_sem_id_t;
typedef sk_id_t sk_mutex_id_t;
typedef sk_id_t sk_tick_id_t;
typedef sk_id_t sk_evt_id_t;
typedef sk_id_t sk_queue_id_t;

/*!����ĳ�ʱʱ�䶨��*/
#define  SK_TIMEOUT_INFINITY 		(0xffffffff) 	/*!<���޵ȴ� */
#define  SK_TIMEOUT_IMMEDIATE 		(0x00)			 /*!<������ʱ */

#ifdef SKY_BIG_ENDIAN
#define CONSTANT_SWAB16(x) \
	((U16)( \
		(((U16)(x) & (U16)0x00ffU) << 8) | \
		(((U16)(x) & (U16)0xff00U) >> 8) ))
#define CONSTANT_SWAB32(x) \
	((U32)( \
		(((U32)(x) & (U32)0x000000ffUL) << 24) | \
		(((U32)(x) & (U32)0x0000ff00UL) <<  8) | \
		(((U32)(x) & (U32)0x00ff0000UL) >>  8) | \
		(((U32)(x) & (U32)0xff000000UL) >> 24) ))
#define CONSTANT_SWAB64(x) \
	((U64)( \
		(U64)(((U64)(x) & (U64)0x00000000000000ffULL) << 56) | \
		(U64)(((U64)(x) & (U64)0x000000000000ff00ULL) << 40) | \
		(U64)(((U64)(x) & (U64)0x0000000000ff0000ULL) << 24) | \
		(U64)(((U64)(x) & (U64)0x00000000ff000000ULL) <<  8) | \
	        (U64)(((U64)(x) & (U64)0x000000ff00000000ULL) >>  8) | \
		(U64)(((U64)(x) & (U64)0x0000ff0000000000ULL) >> 24) | \
		(U64)(((U64)(x) & (U64)0x00ff000000000000ULL) >> 40) | \
		(U64)(((U64)(x) & (U64)0xff00000000000000ULL) >> 56) ))
#else

#define CONSTANT_SWAB16(x)	(x)
#define CONSTANT_SWAB32(x)	(x)
#define CONSTANT_SWAB64(x)	(x)

#endif


 /*!����block�ķ���eit�ڴ������*/
typedef enum sk_block_mem_type_e
{
	SK_BLOCK_MEM_DATA_TYPE,
	SK_BLOCK_MEM_EIT_TYPE,
	SK_BLOCK_MEM_CA_TYPE,
	SK_BLOCK_MEM_BROWSET_TYPE,
	SK_BLOCK_MEM_AV_TYPE,
	#ifdef SK_API_MEMPOOL_SUPPORT
		SK_BLOCK_MEM_API_TYPE,
	#endif
	SK_NB_OF_BLOCK_MEM_TYPE
}sk_block_mem_type_t;

 /*!����block���������*/
typedef enum sk_mem_cmd_e
{
	SK_BLOCK_MEM_CMD_CREATE,
	SK_BLOCK_MEM_CMD_DESTROY,
	SK_BLOCK_MEM_CMD_CLEAN,
	SK_BLOCK_MEM_CMD_STATUS,
	SK_BLOCK_MEM_CMD_SIZE_STARTADDR,
	SK_BLOCK_MEM_CMD_TEST,
	SK_NB_OF_BLOCK_MEM_CMD
}sk_mem_cmd_t;

/*!
* task����ָ�����Ͷ���
*@param p_param�̺߳���������/�������
*@return void
*/
typedef  void* (*sk_task_pfn_t)(void * p_param);

/*!
* tick����ָ�����Ͷ���
*@param p_param����������/�������
*@param sk_tick_id_t tick id
*@return void
*/
typedef void (*sk_tick_pfn_t)(const sk_tick_id_t, void * p_param);

/*!
* �߳�״̬
*/
typedef enum sk_task_status_e
{
	SK_TASK_STATUS_READY, 		/*!<����*/
	SK_TASK_STATUS_SLEEP, 		/*!<����*/
	SK_TASK_STATUS_RUNNING, 		/*!<��������*/
	SK_TASK_STATUS_SUPENDED,		/*!<����*/
	SK_TASK_STATUS_ERROR 		/*!<����״̬*/
} sk_task_status_t;

/*!
* tick ����
*/
typedef  enum sk_tick_type_e
{
	SK_TICK_TYPE_PERIO 	= 0,	/*!<call periodic*/
	SK_TICK_TYPE_SINGLE		/*!<call only once*/
}sk_tick_type_t;

/*!
* tick ״̬
*/
typedef  enum sk_tick_status_e
{
	SK_TICK_STOP_STATUS 	= 0,		/*!<stoped*/
	SK_TICK_RUN_STATUS ,  			/*!<running	*/
	SK_TICK_NOT_EXIST				/*!<not exist*/
}sk_tick_status_t;


/*!
*@brief ��ȡ�汾��Ϣ
*@return sk_revision_t �汾��Ϣ
*/
sk_revision_t  sk_os_get_revision(void);

/*!
*@brief osģ���ʼ��������
*��������������os����������ǰ����(get revision ����)
*@param	 ��
*@return sk_status_code_t
*/
sk_status_code_t  sk_os_init(void);

/*!
*@brief os start, ��Щ
*OS���У��������ʵ��,��ֱ�ӷ��سɹ�
*@param	 ��
*@return sk_status_code_t
*/
sk_status_code_t  sk_os_start(void);

/*!
*@brief os start, ��Щ
*OSֹͣ���������ʵ��,��ֱ�ӷ��سɹ�
*@param	 ��
*@return sk_status_code_t
*/
sk_status_code_t  sk_os_stop(void);

/*!
*@brief ɱ��һ���߳�
*@return sk_status_code_t
*/
sk_status_code_t sk_task_kill(const sk_task_id_t  task_id);

/*!
*@brief �ȴ��߳̽���
*@param ms �ȴ�ʱ��(ms)
*@return sk_status_code_t
*/
sk_status_code_t sk_task_wait_timeout(const sk_task_id_t  task_id, const U32 ms);
sk_status_code_t sk_task_wait(const sk_task_id_t  task_id);

/*!
*@brief osģ����ֹ������
*��ֹ�Ժ�ģ�������������ܱ�����(get revision & sk_os_init����)
*@param	 ��
*@return sk_status_code_t
*/
sk_status_code_t  sk_os_term(void);

/*!
*@brief �����߳�
*@retval  p_task_id �����̵߳�ID���㣬���ʧ�ܷ���SK_INVALID_ID
*@param p_task_name ��ʾ�������̵߳����ƣ�������Ψһ��
*@param task_pfn�߳����еĺ�����ָ�롣
*@param p_param�̺߳���������/�������
*@param p_stack�߳�ջ��ָ�룬 ���ΪNULL�����ɸ������Զ�����
*@param stack_sizeջ�Ĵ�С
*@param priority���ȼ�
*@return sk_status_code_t
*/
sk_status_code_t sk_task_create(sk_task_id_t * const p_task_id, 
						const char * p_task_name, 
						const sk_task_pfn_t task_pfn, 
						void * p_param,
						void * p_stack, 
						U32 stack_size,
						U32 priority);

/*!
*@brief ��ȡ��ǰ�߳�ID
*@retval  p_cur_task_id �����߳�ID
*@return sk_status_code_t
*/
sk_status_code_t sk_task_cur_id(sk_task_id_t * const p_cur_task_id);

/*!
*@brief ��ֹ���߳�
*@return sk_status_code_t
*/
sk_status_code_t sk_task_term(void);

/*!
*@brief ��ֹ�߳�
*@param  task_id Ҫ��ֹ���߳�ID, 
*@return sk_status_code_t
*/
sk_status_code_t sk_task_destroy(const sk_task_id_t  task_id);

/*!
*@brief �����߳����ȼ�
*@param task_id �߳�ID
*@param priority ���ȼ�
*@return sk_status_code_t
*/
sk_status_code_t sk_task_set_priority(const sk_task_id_t  task_id, const U32 priority);

/*!
*@brief �����߳����ȼ�
*@param task_id �߳�ID
*@retval p_priority �������ȼ�
*@return sk_status_code_t
*/
sk_status_code_t sk_task_get_priority(const sk_task_id_t  task_id, U32 * const  p_priority);

/*!
*@brief �����߳�
*@param task_id �߳�ID
*@return sk_status_code_t
*/
sk_status_code_t sk_task_suspend(const sk_task_id_t  task_id);

/*!
*@brief �ָ�һ��������߳�,
*@param task_id �߳�ID
*@return sk_status_code_t
*/
sk_status_code_t sk_task_resume(const sk_task_id_t  task_id);

/*!
*@brief �����߳�״̬
*@param task_id �߳�ID
*@retval p_status �����߳�״̬
*@return sk_status_code_t
*/
sk_status_code_t sk_task_get_status(const sk_task_id_t  task_id,  sk_task_status_t * const p_status);

/*!
*@brief �߳�����
*@param ms ����ʱ��(ms)
*@return sk_status_code_t
*/
sk_status_code_t sk_task_delay(const U32  ms);

/*!
*@brief �߳�����
*@param ms ����ʱ��(us)
*@return sk_status_code_t
*/
sk_status_code_t sk_task_delay_us(const U32 u32us);

/*!
*@brief �ڴ����ģ��ĳ�ʼ��
*@param	 ��
*@return sk_status_code_t
*/
sk_status_code_t sk_mem_init(void);

/*!
*@brief ��̬����size��С���ڴ�
*@param	 size ��С(��λBYTE)
*@return �ɹ������ڴ�ĵ�ַ,ʧ�ܷ���NULL
*/
void * sk_mem_malloc(U32 size);

/*!
*@brief ���·���p_mem �� size ��С���ڴ�
*@param	 size ��С(��λBYTE)
*@return �ɹ������ڴ�ĵ�ַ,ʧ�ܷ���NULL
*/
void * sk_mem_realloc(void* p_mem, U32 size);

/*!
*@brief ��̬����������ڴ档�μ���׼C����
*@param	 num ���������
*@param	 size ���ݵĴ�С
*@return �ɹ������ڴ�ĵ�ַ,ʧ�ܷ���NULL
*/
void * sk_mem_calloc(const U32 num, const  U32 size);

/*!
*@brief �ͷŶ�̬������ڴ�
*@param	 p_memҪ�ͷŵ��ڴ��ָ��
*@return ��
*/
void sk_mem_free(void * p_mem);

/*!
*@brief ��ȡram����Ϣ
*@retval p_ram_size ram size
*@retval p_ram_size ram width
*@return sk_status_code_t
*/
sk_status_code_t sk_mem_get_ram_info(U32 * const  p_ram_size, U32 * const p_ram_width);

/*!
*@brief ��̬����size��С���ڴ�
* ����block_type�Ķ������alloc_size��С���ڴ�顣
* �����ĳһƽ̨�϶Ը����͵��ڴ�����û�������Ҫ���û���ڴ��ķ��䣬
* �����ʹ��sk_mem_malloc����������͵��ڴ档�ڴ���ʼ����sk_mem_init����ɣ�
* �ú�������block_type����Ӧ���ڴ���з��䣬��ʵ����Ҫ������ƽ̨��Ҫ��
*@param	 alloc_size ��С(��λBYTE)
*@param	 block_type ����
*@return �ɹ������ڴ�ĵ�ַ,ʧ�ܷ���NULL
*@see block_type
*/
void* sk_mem_block_calloc(const sk_block_mem_type_t block_type, const U32 alloc_size);

/*!
*@brief ����block_type�Ķ����ͷ���sk_mem_block_calloc������ڴ�
*@param	 p_memҪ�ͷŵ��ڴ��ָ��
*@return sk_status_code_t
*/
sk_status_code_t sk_mem_block_free(const sk_block_mem_type_t block_type, void * p_mem);

/*!
*@brief �ڴ�block����
*@param	 cmd  ��������,@see sk_mem_cmd_t
*@param	 block_type block����,@see sk_block_mem_type_t
*@param fparam  @see sk_mem_cmd_t sk_block_mem_type_t
*@param sparam  @see sk_mem_cmd_t sk_block_mem_type_t
*@return sk_status_code_t
*/
sk_status_code_t sk_mem_cmd(sk_mem_cmd_t cmd, sk_block_mem_type_t block_type, U32 fparam, U32 sparam);

/***********************************************************/
/** sk_mem_set_data_pool_size
* @param   nSize   size
*
* @return SK_SUCCESS means set success, SK_FAILED means failed
* @brief -����data mem pool��С���˽ӿ���Ӧ�õ��ã��������������Ҫ���ô�С��
*		����data mem pool��СΪnSize + eit mem pool��С
*		�˽ӿ�Ӧ�ñ�����sky_SYSTEM_Create(Ҳ����sk_mem_init)֮ǰ����
*		���Ӧ��δ���ô˽ӿڣ���data mem pool��С����ԭ����ʽ����
* @author tiger      @date 2014/05/05      create 
************************************************************/
sk_status_code_t sk_mem_set_data_pool_size(int nSize);

/*!
*@brief �����ź���
*@retval  p_sem_id �����ź���ID���㣬���ʧ�ܷ���SK_INVALID_ID
*@param p_sem_name ��ʾ�������ź��������ƣ�������Ψһ��
*@param init_count ��ʼ��ʱ�ź��������󱻸���ĳ�ֵ
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_create(sk_sem_id_t *const  p_sem_id, const char * p_sem_name,const  U32 init_count);

/*!
*@brief �����ź���
*@param  sem_idҪ���ٵ��ź���ID
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_destroy(const sk_sem_id_t sem_id);

/*!
*@brief �ȴ��ź���,û�г�ʱ,���޵ȴ�
*@param  sem_idҪ�ȴ����ź���ID
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_lock(const sk_sem_id_t sem_id);

/*!
*@brief �ȴ��ź���,
*@param  sem_idҪ�ȴ����ź���ID
*@param  timeout_ms��ʱʱ��(MS),֧�����޳�ʱ��������ʱ��������ʱ��,
*@return sk_status_code_t �����ʱ�򷵻�SK_ERROR_TIMEOUT
*/
sk_status_code_t  sk_sem_lock_timeout(const sk_sem_id_t sem_id, const U32 timeout_ms);

/*!
*@brief �ͷ��ź���
*@param  sem_idҪ�ͷŵ��ź���ID
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_unlock(const sk_sem_id_t sem_id);

/*!
*@brief semaphore_value
*@param  sem_id �ź���ID
*@retval p_count semaphore_value
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_count(const sk_sem_id_t sem_id, S32 * p_count);

/*!
*@brief ����������
*@retval  p_mutex_id �����ź���ID���㣬���ʧ�ܷ���SK_INVALID_ID
*@return sk_status_code_t
*/
sk_status_code_t sk_mutex_create(sk_mutex_id_t *const p_mutex_id);

/*!
*@brief ���ٻ�����
*@param  mutex_idҪ���ٵĻ�����ID
*@return sk_status_code_t
*/
sk_status_code_t sk_mutex_destroy(const sk_mutex_id_t mutex_id);

/*!
*@brief �õ�������סһ����������
* ��û���˳��û�����֮ǰ���������̲߳��ܵõ��û�������
*@param  mutex_idҪ�ȴ��Ļ�����ID
*@param  timeout_ms��ʱʱ��(MS),֧�����޳�ʱ��������ʱ��������ʱ��,
*@return sk_status_code_t �����ʱ�򷵻�SK_ERROR_TIMEOUT
*/
sk_status_code_t sk_mutex_lock(const sk_mutex_id_t mutex_id, U32 timeout_ms);

/*!
*@brief �˳�һ��������
*@param  sem_idҪ�ͷŵĻ�����ID
*@return sk_status_code_t
*/
sk_status_code_t sk_mutex_unlock(const sk_mutex_id_t mutex_id);

/*!
*@brief ��ȡϵͳtick
*@retval  p_sys_tick ����ϵͳ��tick
*@return sk_status_code_t
*/
sk_status_code_t sk_clock_get_sys_tick(U32* const p_sys_tick);

/*!
*@brief ��ȡϵͳʱ��
*@retval  p_sys_time_ms ����ϵͳ��ʱ��(MS)
*@return sk_status_code_t
*/
sk_status_code_t sk_clock_get_sys_time(U32* const p_sys_time_ms);

/*!
*@brief ��������ʱ��Ĳ�ֵ
*@retval  start_time ��ʼʱ��
*@retval  end_time ����ʱ��
*@return ��ֵ
*/
U32 sk_clock_sys_time_minus(const U32 start_time, const U32 end_time);

/*!
*@brief ��������ʱ��ļ�ֵ
*@retval  start_time ��ʼʱ��
*@retval  end_time ����ʱ��
*@return ��ֵ
*/
U32 sk_clock_sys_time_plus(const U32 start_time, const U32 end_time);

/*!
*@brief ����1 s
*@param  void
*@return 1000
*/

U32 sk_clock_sys_time_persec(void);

/*!
*@brief ����ϵͳʱ��
*@param  new_sys_ms ϵͳʱ��(MS)
*@return sk_status_code_t
*/
sk_status_code_t sk_clock_set_sys_time(const U32  new_sys_time_ms);

/*!
*@brief ����ϵͳʱ��Ƶ��
*@retval  p_sys_freg ����ϵͳʱ��Ƶ��(Hz)
*@return sk_status_code_t
*/
sk_status_code_t sk_clock_get_sys_freg(U32* const p_sys_freg);

/*!
*@brief tick ��ʼ��,����������tick��������ǰ������
*@param  ��
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_init(void);

/*!
*@brief tick ��ֹ,��ֹ�����init����������������û������
*@param  ��
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_term(void);

/*!
*@brief ����һ��tickʵ��
*@retval p_tick_id ���ش�����tick id
*@param tick_pfn �ص�����@see sk_tick_pfn_t
*@param p_param ����tick_pfn�Ĳ���@see sk_tick_pfn_t
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_create(sk_tick_id_t * const  p_tick_id, const sk_tick_pfn_t  tick_pfn, void * p_param);

/*!
*@brief ����һ��tickʵ��
*@param tick_id Ҫ���ٵ�tick ʵ��id
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_destroy(const sk_tick_id_t tick_id);

/*!
*@brief ����һ��tick
*@param tick_id 
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_start(const sk_tick_id_t  tick_id);

/*!
*@brief ֹͣһ��tick
*@param tick_id 
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_stop(const sk_tick_id_t  tick_id);


/*!
*@brief ����tick����
*@param tick_id ʵ��id
*@param tick_type ����  @see sk_tick_type_t
*@param tick_time_ms ���ø�tick�Ļص�������tick��ʱ��(MS). ��tick���������ֵʱ�����ûص�����
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_set(const sk_tick_id_t  tick_id, 
							const sk_tick_type_t tick_type,
							const U32 tick_time_ms);

/*!
*@brief ��ȡtick������Ϣ
*@param tick_id ʵ��id
*@retval p_tick_type ����tick ����  @see sk_tick_type_t
*@retval p_tick_time_ms ����tick�Ķ�ʱʱ����� ���ø�tick�Ļص�������tick��ʱ��(MS). ��tick���������ֵʱ�����ûص�����
*@retval p_tick_status ����tick ״̬  @see sk_tick_status_t
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_get_info(const sk_tick_id_t  tick_id, 
								  sk_tick_type_t * const  p_tick_type,
								  U32 * const p_tick_time_ms, 
								  sk_tick_status_t * const p_tick_status);

/*!
*@brief ����һ��event
*@retval p_event_id ���ش�����event id
*@param init_is_set ����ʱ�ĳ�ʼ��״̬,�Ƿ����¼��ź�
*@return sk_status_code_t
*/
sk_status_code_t sk_event_create(sk_evt_id_t * const p_event_id, const U32 init_is_set);

/*!
*@brief ����һ��event
*@param event_id Ҫ���ٵ�event id
*@return sk_status_code_t
*/
sk_status_code_t sk_event_destroy(const sk_evt_id_t  event_id);

/*!
*@brief �ȴ�һ��ͬ���¼�(event), ֱ�����¼����źŻ��߳�ʱ
*@param event_id event id
*@param is_not_auto_reset �ȵ����Ƿ��Զ���λ(ע:���Լ����߳�ͬʱ�ȴ�һ���¼�)
*@param timeout_ms ��ʱʱ��(MS), ֧�����޳�ʱ��������ʱ��������ĵȴ�ʱ��
*@return sk_status_code_t �����ʱ�򷵻�SK_ERROR_TIMEOUT
*/
sk_status_code_t sk_event_wait(const sk_evt_id_t  event_id, const U32 is_not_auto_reset, const U32 timeout_ms);

/*!
*@brief �����¼�Ϊ���ź�״̬
*ע:���Լ����߳�ͬʱ�ȴ�һ���¼�
*@param event_id event id
*@return sk_status_code_t
*/
sk_status_code_t sk_event_set(const sk_evt_id_t  event_id);

/*!
*@brief �����¼�Ϊ���ź�״̬
*ע:��λ��Ϊ���ź�״̬
*@param event_id event id
*@return sk_status_code_t
*/
sk_status_code_t sk_event_reset(const sk_evt_id_t  event_id);

/*!
*@brief ����һ����Ϣ����
*@retval p_queue_id���ش������¼�id
*@param p_queue_name ���е����֣�������Ψһ��
*@param max_msg_count ������������Ϣ����
*@return sk_status_code_t
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_create(sk_queue_id_t * const p_queue_id, const char * p_queue_name, const U32 max_msg_count);

/*!
*@brief ����һ����Ϣ����Ϣ����
*@param queue_id ��Ϣ����id
*@param msgҪ���͵���Ϣ
*@param timeout_ms ��ʱʱ��(MS), ֧�����޳�ʱ��������ʱ��������ĵȴ�ʱ��
*@return sk_status_code_t �����ʱ�򷵻�SK_ERROR_TIMEOUT
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_send(const sk_queue_id_t queue_id, const sk_queue_msg_t msg, const U32 timeout_ms);

/*!
*@brief ����Ϣ�����н���һ����Ϣ
*@param queue_id ��Ϣ����id
*@param msgҪ���ܵ���Ϣ
*@param timeout_ms ��ʱʱ��(MS), ֧�����޳�ʱ��������ʱ��������ĵȴ�ʱ��
*@return sk_status_code_t �����ʱ�򷵻�SK_ERROR_TIMEOUT
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_receive(const sk_queue_id_t queue_id,const  sk_queue_msg_t msg, const  U32 timeout_ms);

/*!
*@brief ����һ����Ϣ����
*@param queue_id Ҫ���ٵ���Ϣ����id
*@return sk_status_code_t
*/
sk_status_code_t  sk_queue_destroy(const sk_queue_id_t queue_id);

/*!
*@brief ����һ����Ϣ���У����Զ�����Ϣ�Ĵ�С
*@retval p_queue_id���ش������¼�id
*@param p_queue_name ���е����֣�������Ψһ��
*@param max_msg_count ������������Ϣ����
*@param msg_size ��Ϣ�Ĵ�С
*@return sk_status_code_t
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_create_ex(sk_queue_id_t * const p_queue_id, const char * p_queue_name, const U32 max_msg_count, const U32 msg_size);

/*!
*@brief ����һ����Ϣ����Ϣ����
*@param queue_id ��Ϣ����id
*@param msgҪ���͵���Ϣ
*@param msg Ҫ���͵���Ϣ�ĳ��ȡ�������������ĳ��ȣ���ô�᷵�ش���
*@param timeout_ms ��ʱʱ��(MS), ֧�����޳�ʱ��������ʱ��������ĵȴ�ʱ��
*@return sk_status_code_t �����ʱ�򷵻�SK_ERROR_TIMEOUT
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_send_ex(const sk_queue_id_t queue_id, void* msg, const U32 msg_size,const U32 timeout_ms);
/*!
*@brief ����һ����Ϣ����Ϣ����
*@param queue_id ��Ϣ����id
*@param msg Ҫ���յ���Ϣ��
*@param msg Ҫ���յ���Ϣ�ĳ��ȡ�������������ĳ��ȣ���ô�᷵�ش���
*@param timeout_ms ��ʱʱ��(MS), ֧�����޳�ʱ��������ʱ��������ĵȴ�ʱ��
*@return sk_status_code_t �����ʱ�򷵻�SK_ERROR_TIMEOUT
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_receive_ex(const sk_queue_id_t queue_id,const  void* msg, const U32 msg_size, const  U32 timeout_ms);

/*!
*@brief ����һ����Ϣ����
*@param queue_id Ҫ���ٵ���Ϣ����id
*@return sk_status_code_t
*/
sk_status_code_t  sk_queue_destroy_ex(const sk_queue_id_t queue_id);






#ifdef __cplusplus
}
#endif

#endif /*SK_OS_H*/

/*eof*/



