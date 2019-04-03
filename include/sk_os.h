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

/*!线程优先级*/
#define SK_TASK_DEFAULT_PRIORITY   (64)

#define SK_TASK_PRIORITY_LOWEST  	(SK_TASK_DEFAULT_PRIORITY-24) 	/*!<最低的优先级*/
#define SK_TASK_PRIORITY_LOW    	(SK_TASK_DEFAULT_PRIORITY-12)	/*!<较低的优先级*/
#define SK_TASK_PRIORITY_NORMAL 	(SK_TASK_DEFAULT_PRIORITY)		/*!<低的优先级*/
#define SK_TASK_PRIORITY_HIGH  		(SK_TASK_DEFAULT_PRIORITY+12)	/*!<高的优先级*/
#define SK_TASK_PRIORITY_HIGHEST  	(SK_TASK_DEFAULT_PRIORITY+24)	/*!<最高的优先级*/


#ifdef SK_ANDROID  //android 也属于linux os，这么定义是android很多地方 只定义SK_ANDROID，未定义SK_OS_LINUX freeman
	#define SK_TASK_DEFAULT_STACK_SIZE	(64*1024)  /*android栈需要调大 ，小了有些异常*/
#else
	#ifdef SK_OS_LINUX
	    #if defined (SK_CHIP_mstar7c51k) ||  defined(SK_CHIP_mstar7c51g) || defined(SK_OS_MSTAR_LINUX) || defined (SK_OS_ALI_LINUX)
			#define SK_TASK_DEFAULT_STACK_SIZE (128*1024)/*mstar linux平台 动态库的task的创建最小的堆栈不得小于128K*/
		#else
			#define SK_TASK_DEFAULT_STACK_SIZE	(24*1024)  /*防止栈太小，栈溢出导致死机，分配最好按页的整数倍分配 */
	    #endif
	#else
	/*!the define the task default stack size  */
		#define SK_TASK_DEFAULT_STACK_SIZE (18*1024)  /*!在linux中这个值是最小的*/
	#endif
#endif

/*!消息队列*/
#define SK_QUEUE_MSG_SIZE		4
typedef U32 sk_queue_msg_t[SK_QUEUE_MSG_SIZE];

typedef sk_id_t sk_task_id_t;
typedef sk_id_t sk_sem_id_t;
typedef sk_id_t sk_mutex_id_t;
typedef sk_id_t sk_tick_id_t;
typedef sk_id_t sk_evt_id_t;
typedef sk_id_t sk_queue_id_t;

/*!特殊的超时时间定义*/
#define  SK_TIMEOUT_INFINITY 		(0xffffffff) 	/*!<无限等待 */
#define  SK_TIMEOUT_IMMEDIATE 		(0x00)			 /*!<立即超时 */

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


 /*!定义block的分配eit内存的类型*/
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

 /*!定义block命令的类型*/
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
* task函数指针类型定义
*@param p_param线程函数的输入/输入参数
*@return void
*/
typedef  void* (*sk_task_pfn_t)(void * p_param);

/*!
* tick函数指针类型定义
*@param p_param函数的输入/输入参数
*@param sk_tick_id_t tick id
*@return void
*/
typedef void (*sk_tick_pfn_t)(const sk_tick_id_t, void * p_param);

/*!
* 线程状态
*/
typedef enum sk_task_status_e
{
	SK_TASK_STATUS_READY, 		/*!<就绪*/
	SK_TASK_STATUS_SLEEP, 		/*!<休眠*/
	SK_TASK_STATUS_RUNNING, 		/*!<正在运行*/
	SK_TASK_STATUS_SUPENDED,		/*!<挂起*/
	SK_TASK_STATUS_ERROR 		/*!<错误状态*/
} sk_task_status_t;

/*!
* tick 类型
*/
typedef  enum sk_tick_type_e
{
	SK_TICK_TYPE_PERIO 	= 0,	/*!<call periodic*/
	SK_TICK_TYPE_SINGLE		/*!<call only once*/
}sk_tick_type_t;

/*!
* tick 状态
*/
typedef  enum sk_tick_status_e
{
	SK_TICK_STOP_STATUS 	= 0,		/*!<stoped*/
	SK_TICK_RUN_STATUS ,  			/*!<running	*/
	SK_TICK_NOT_EXIST				/*!<not exist*/
}sk_tick_status_t;


/*!
*@brief 获取版本信息
*@return sk_revision_t 版本信息
*/
sk_revision_t  sk_os_get_revision(void);

/*!
*@brief os模块初始化函数。
*必须在所有其他os函数被调用前调用(get revision 例外)
*@param	 无
*@return sk_status_code_t
*/
sk_status_code_t  sk_os_init(void);

/*!
*@brief os start, 有些
*OS运行，如果无需实现,则直接返回成功
*@param	 无
*@return sk_status_code_t
*/
sk_status_code_t  sk_os_start(void);

/*!
*@brief os start, 有些
*OS停止，如果无需实现,则直接返回成功
*@param	 无
*@return sk_status_code_t
*/
sk_status_code_t  sk_os_stop(void);

/*!
*@brief 杀死一个线程
*@return sk_status_code_t
*/
sk_status_code_t sk_task_kill(const sk_task_id_t  task_id);

/*!
*@brief 等待线程结束
*@param ms 等待时间(ms)
*@return sk_status_code_t
*/
sk_status_code_t sk_task_wait_timeout(const sk_task_id_t  task_id, const U32 ms);
sk_status_code_t sk_task_wait(const sk_task_id_t  task_id);

/*!
*@brief os模块终止函数。
*终止以后本模块其他函数不能被调用(get revision & sk_os_init例外)
*@param	 无
*@return sk_status_code_t
*/
sk_status_code_t  sk_os_term(void);

/*!
*@brief 创建线程
*@retval  p_task_id 创建线程的ID非零，如果失败返回SK_INVALID_ID
*@param p_task_name 表示创建的线程的名称，必须是唯一。
*@param task_pfn线程运行的函数的指针。
*@param p_param线程函数的输入/输入参数
*@param p_stack线程栈的指针， 如果为NULL，则由给函数自动分配
*@param stack_size栈的大小
*@param priority优先级
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
*@brief 获取当前线程ID
*@retval  p_cur_task_id 返回线程ID
*@return sk_status_code_t
*/
sk_status_code_t sk_task_cur_id(sk_task_id_t * const p_cur_task_id);

/*!
*@brief 终止本线程
*@return sk_status_code_t
*/
sk_status_code_t sk_task_term(void);

/*!
*@brief 终止线程
*@param  task_id 要终止的线程ID, 
*@return sk_status_code_t
*/
sk_status_code_t sk_task_destroy(const sk_task_id_t  task_id);

/*!
*@brief 设置线程优先级
*@param task_id 线程ID
*@param priority 优先级
*@return sk_status_code_t
*/
sk_status_code_t sk_task_set_priority(const sk_task_id_t  task_id, const U32 priority);

/*!
*@brief 设置线程优先级
*@param task_id 线程ID
*@retval p_priority 返回优先级
*@return sk_status_code_t
*/
sk_status_code_t sk_task_get_priority(const sk_task_id_t  task_id, U32 * const  p_priority);

/*!
*@brief 挂起线程
*@param task_id 线程ID
*@return sk_status_code_t
*/
sk_status_code_t sk_task_suspend(const sk_task_id_t  task_id);

/*!
*@brief 恢复一个挂起的线程,
*@param task_id 线程ID
*@return sk_status_code_t
*/
sk_status_code_t sk_task_resume(const sk_task_id_t  task_id);

/*!
*@brief 设置线程状态
*@param task_id 线程ID
*@retval p_status 返回线程状态
*@return sk_status_code_t
*/
sk_status_code_t sk_task_get_status(const sk_task_id_t  task_id,  sk_task_status_t * const p_status);

/*!
*@brief 线程休眠
*@param ms 休眠时间(ms)
*@return sk_status_code_t
*/
sk_status_code_t sk_task_delay(const U32  ms);

/*!
*@brief 线程休眠
*@param ms 休眠时间(us)
*@return sk_status_code_t
*/
sk_status_code_t sk_task_delay_us(const U32 u32us);

/*!
*@brief 内存管理模块的初始化
*@param	 无
*@return sk_status_code_t
*/
sk_status_code_t sk_mem_init(void);

/*!
*@brief 动态分配size大小的内存
*@param	 size 大小(单位BYTE)
*@return 成功返回内存的地址,失败返回NULL
*/
void * sk_mem_malloc(U32 size);

/*!
*@brief 重新分配p_mem ， size 大小的内存
*@param	 size 大小(单位BYTE)
*@return 成功返回内存的地址,失败返回NULL
*/
void * sk_mem_realloc(void* p_mem, U32 size);

/*!
*@brief 动态分配数组的内存。参见标准C语言
*@param	 num 数组的数量
*@param	 size 数据的大小
*@return 成功返回内存的地址,失败返回NULL
*/
void * sk_mem_calloc(const U32 num, const  U32 size);

/*!
*@brief 释放动态分配的内存
*@param	 p_mem要释放的内存的指针
*@return 无
*/
void sk_mem_free(void * p_mem);

/*!
*@brief 获取ram的信息
*@retval p_ram_size ram size
*@retval p_ram_size ram width
*@return sk_status_code_t
*/
sk_status_code_t sk_mem_get_ram_info(U32 * const  p_ram_size, U32 * const p_ram_width);

/*!
*@brief 动态分配size大小的内存
* 按照block_type的定义分配alloc_size大小的内存块。
* 如果在某一平台上对该类型的内存块分配没有特殊的要求或没有内存块的分配，
* 则可以使用sk_mem_malloc来分配该类型的内存。内存块初始化在sk_mem_init中完成，
* 该函数根据block_type在相应的内存块中分配，其实现主要依靠与平台的要求。
*@param	 alloc_size 大小(单位BYTE)
*@param	 block_type 类型
*@return 成功返回内存的地址,失败返回NULL
*@see block_type
*/
void* sk_mem_block_calloc(const sk_block_mem_type_t block_type, const U32 alloc_size);

/*!
*@brief 按照block_type的定义释放由sk_mem_block_calloc分配的内存
*@param	 p_mem要释放的内存的指针
*@return sk_status_code_t
*/
sk_status_code_t sk_mem_block_free(const sk_block_mem_type_t block_type, void * p_mem);

/*!
*@brief 内存block操作
*@param	 cmd  操作命令,@see sk_mem_cmd_t
*@param	 block_type block类型,@see sk_block_mem_type_t
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
* @brief -设置data mem pool大小，此接口由应用调用，供其根据自身需要设置大小，
*		最终data mem pool大小为nSize + eit mem pool大小
*		此接口应用必须在sky_SYSTEM_Create(也就是sk_mem_init)之前调用
*		如果应用未调用此接口，则data mem pool大小按照原来方式设置
* @author tiger      @date 2014/05/05      create 
************************************************************/
sk_status_code_t sk_mem_set_data_pool_size(int nSize);

/*!
*@brief 创建信号量
*@retval  p_sem_id 创建信号量ID非零，如果失败返回SK_INVALID_ID
*@param p_sem_name 表示创建的信号量的名称，必须是唯一。
*@param init_count 初始化时信号量创建后被赋予的初值
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_create(sk_sem_id_t *const  p_sem_id, const char * p_sem_name,const  U32 init_count);

/*!
*@brief 销毁信号量
*@param  sem_id要销毁的信号量ID
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_destroy(const sk_sem_id_t sem_id);

/*!
*@brief 等待信号量,没有超时,无限等待
*@param  sem_id要等待的信号量ID
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_lock(const sk_sem_id_t sem_id);

/*!
*@brief 等待信号量,
*@param  sem_id要等待的信号量ID
*@param  timeout_ms超时时间(MS),支持无限超时和立即超时两个特殊时间,
*@return sk_status_code_t 如果超时则返回SK_ERROR_TIMEOUT
*/
sk_status_code_t  sk_sem_lock_timeout(const sk_sem_id_t sem_id, const U32 timeout_ms);

/*!
*@brief 释放信号量
*@param  sem_id要释放的信号量ID
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_unlock(const sk_sem_id_t sem_id);

/*!
*@brief semaphore_value
*@param  sem_id 信号量ID
*@retval p_count semaphore_value
*@return sk_status_code_t
*/
sk_status_code_t  sk_sem_count(const sk_sem_id_t sem_id, S32 * p_count);

/*!
*@brief 创建互斥锁
*@retval  p_mutex_id 创建信号量ID非零，如果失败返回SK_INVALID_ID
*@return sk_status_code_t
*/
sk_status_code_t sk_mutex_create(sk_mutex_id_t *const p_mutex_id);

/*!
*@brief 销毁互斥量
*@param  mutex_id要销毁的互斥量ID
*@return sk_status_code_t
*/
sk_status_code_t sk_mutex_destroy(const sk_mutex_id_t mutex_id);

/*!
*@brief 得到，并锁住一个互斥区，
* 在没有退出该互斥区之前，其他的线程不能得到该互斥区。
*@param  mutex_id要等待的互斥量ID
*@param  timeout_ms超时时间(MS),支持无限超时和立即超时两个特殊时间,
*@return sk_status_code_t 如果超时则返回SK_ERROR_TIMEOUT
*/
sk_status_code_t sk_mutex_lock(const sk_mutex_id_t mutex_id, U32 timeout_ms);

/*!
*@brief 退出一个互斥区
*@param  sem_id要释放的互斥量ID
*@return sk_status_code_t
*/
sk_status_code_t sk_mutex_unlock(const sk_mutex_id_t mutex_id);

/*!
*@brief 获取系统tick
*@retval  p_sys_tick 返回系统的tick
*@return sk_status_code_t
*/
sk_status_code_t sk_clock_get_sys_tick(U32* const p_sys_tick);

/*!
*@brief 获取系统时间
*@retval  p_sys_time_ms 返回系统的时间(MS)
*@return sk_status_code_t
*/
sk_status_code_t sk_clock_get_sys_time(U32* const p_sys_time_ms);

/*!
*@brief 计算两个时间的差值
*@retval  start_time 开始时间
*@retval  end_time 结束时间
*@return 差值
*/
U32 sk_clock_sys_time_minus(const U32 start_time, const U32 end_time);

/*!
*@brief 计算两个时间的加值
*@retval  start_time 开始时间
*@retval  end_time 结束时间
*@return 加值
*/
U32 sk_clock_sys_time_plus(const U32 start_time, const U32 end_time);

/*!
*@brief 计算1 s
*@param  void
*@return 1000
*/

U32 sk_clock_sys_time_persec(void);

/*!
*@brief 设置系统时间
*@param  new_sys_ms 系统时间(MS)
*@return sk_status_code_t
*/
sk_status_code_t sk_clock_set_sys_time(const U32  new_sys_time_ms);

/*!
*@brief 设置系统时钟频率
*@retval  p_sys_freg 返回系统时钟频率(Hz)
*@return sk_status_code_t
*/
sk_status_code_t sk_clock_get_sys_freg(U32* const p_sys_freg);

/*!
*@brief tick 初始化,必须在其他tick函数调用前被调用
*@param  无
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_init(void);

/*!
*@brief tick 终止,终止后除了init函数，其他函数都没法调用
*@param  无
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_term(void);

/*!
*@brief 创建一个tick实例
*@retval p_tick_id 返回创建的tick id
*@param tick_pfn 回调函数@see sk_tick_pfn_t
*@param p_param 传给tick_pfn的参数@see sk_tick_pfn_t
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_create(sk_tick_id_t * const  p_tick_id, const sk_tick_pfn_t  tick_pfn, void * p_param);

/*!
*@brief 销毁一个tick实例
*@param tick_id 要销毁的tick 实例id
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_destroy(const sk_tick_id_t tick_id);

/*!
*@brief 启动一个tick
*@param tick_id 
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_start(const sk_tick_id_t  tick_id);

/*!
*@brief 停止一个tick
*@param tick_id 
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_stop(const sk_tick_id_t  tick_id);


/*!
*@brief 设置tick参数
*@param tick_id 实例id
*@param tick_type 类型  @see sk_tick_type_t
*@param tick_time_ms 调用该tick的回调函数的tick的时间(MS). 当tick计数到达该值时，调用回调函数
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_set(const sk_tick_id_t  tick_id, 
							const sk_tick_type_t tick_type,
							const U32 tick_time_ms);

/*!
*@brief 获取tick参数信息
*@param tick_id 实例id
*@retval p_tick_type 返回tick 类型  @see sk_tick_type_t
*@retval p_tick_time_ms 返回tick的定时时间就是 调用该tick的回调函数的tick的时间(MS). 当tick计数到达该值时，调用回调函数
*@retval p_tick_status 返回tick 状态  @see sk_tick_status_t
*@return sk_status_code_t
*/
sk_status_code_t sk_tick_get_info(const sk_tick_id_t  tick_id, 
								  sk_tick_type_t * const  p_tick_type,
								  U32 * const p_tick_time_ms, 
								  sk_tick_status_t * const p_tick_status);

/*!
*@brief 创建一个event
*@retval p_event_id 返回创建的event id
*@param init_is_set 创建时的初始化状态,是否有事件信号
*@return sk_status_code_t
*/
sk_status_code_t sk_event_create(sk_evt_id_t * const p_event_id, const U32 init_is_set);

/*!
*@brief 销毁一个event
*@param event_id 要销毁的event id
*@return sk_status_code_t
*/
sk_status_code_t sk_event_destroy(const sk_evt_id_t  event_id);

/*!
*@brief 等待一个同步事件(event), 直到该事件有信号或者超时
*@param event_id event id
*@param is_not_auto_reset 等到后是否自动复位(注:可以几个线程同时等待一个事件)
*@param timeout_ms 超时时间(MS), 支持无限超时和立即超时两个特殊的等待时间
*@return sk_status_code_t 如果超时则返回SK_ERROR_TIMEOUT
*/
sk_status_code_t sk_event_wait(const sk_evt_id_t  event_id, const U32 is_not_auto_reset, const U32 timeout_ms);

/*!
*@brief 设置事件为有信号状态
*注:可以几个线程同时等待一个事件
*@param event_id event id
*@return sk_status_code_t
*/
sk_status_code_t sk_event_set(const sk_evt_id_t  event_id);

/*!
*@brief 设置事件为无信号状态
*注:复位后为无信号状态
*@param event_id event id
*@return sk_status_code_t
*/
sk_status_code_t sk_event_reset(const sk_evt_id_t  event_id);

/*!
*@brief 创建一个消息队列
*@retval p_queue_id返回创建的事件id
*@param p_queue_name 队列的名字，必须是唯一的
*@param max_msg_count 队列中最大的消息数量
*@return sk_status_code_t
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_create(sk_queue_id_t * const p_queue_id, const char * p_queue_name, const U32 max_msg_count);

/*!
*@brief 发送一个消息到消息队列
*@param queue_id 消息队列id
*@param msg要发送的消息
*@param timeout_ms 超时时间(MS), 支持无限超时和立即超时两个特殊的等待时间
*@return sk_status_code_t 如果超时则返回SK_ERROR_TIMEOUT
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_send(const sk_queue_id_t queue_id, const sk_queue_msg_t msg, const U32 timeout_ms);

/*!
*@brief 从消息队列中接受一个消息
*@param queue_id 消息队列id
*@param msg要接受的消息
*@param timeout_ms 超时时间(MS), 支持无限超时和立即超时两个特殊的等待时间
*@return sk_status_code_t 如果超时则返回SK_ERROR_TIMEOUT
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_receive(const sk_queue_id_t queue_id,const  sk_queue_msg_t msg, const  U32 timeout_ms);

/*!
*@brief 销毁一个消息队列
*@param queue_id 要销毁的消息队列id
*@return sk_status_code_t
*/
sk_status_code_t  sk_queue_destroy(const sk_queue_id_t queue_id);

/*!
*@brief 创建一个消息队列，可以定义消息的大小
*@retval p_queue_id返回创建的事件id
*@param p_queue_name 队列的名字，必须是唯一的
*@param max_msg_count 队列中最大的消息数量
*@param msg_size 消息的大小
*@return sk_status_code_t
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_create_ex(sk_queue_id_t * const p_queue_id, const char * p_queue_name, const U32 max_msg_count, const U32 msg_size);

/*!
*@brief 发送一个消息到消息队列
*@param queue_id 消息队列id
*@param msg要发送的消息
*@param msg 要发送的消息的长度。如果超过创建的长度，那么会返回错误
*@param timeout_ms 超时时间(MS), 支持无限超时和立即超时两个特殊的等待时间
*@return sk_status_code_t 如果超时则返回SK_ERROR_TIMEOUT
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_send_ex(const sk_queue_id_t queue_id, void* msg, const U32 msg_size,const U32 timeout_ms);
/*!
*@brief 发送一个消息到消息队列
*@param queue_id 消息队列id
*@param msg 要接收的消息。
*@param msg 要接收的消息的长度。如果超过创建的长度，那么会返回错误
*@param timeout_ms 超时时间(MS), 支持无限超时和立即超时两个特殊的等待时间
*@return sk_status_code_t 如果超时则返回SK_ERROR_TIMEOUT
*@see sk_queue_msg_t
*/
sk_status_code_t  sk_queue_receive_ex(const sk_queue_id_t queue_id,const  void* msg, const U32 msg_size, const  U32 timeout_ms);

/*!
*@brief 销毁一个消息队列
*@param queue_id 要销毁的消息队列id
*@return sk_status_code_t
*/
sk_status_code_t  sk_queue_destroy_ex(const sk_queue_id_t queue_id);






#ifdef __cplusplus
}
#endif

#endif /*SK_OS_H*/

/*eof*/



