#include <string.h>

#include "priv_mac.h"
#include "sk_tbx.h"

#define MAX_TICK_NAME_LEN  16      //定义tick name 的长度
#define MAX_TICKS_NUM		 25     //定义tick的数目。
#define TICK_THREAD_STACK_SIZE   ( SK_TASK_DEFAULT_STACK_SIZE)  //堆栈的大小

typedef enum
{
	TICKER_FREE    = 0,  /* Entry not currently being used -- is free for use */
	TICKER_CREATED = 1,  /* This entry has been created but timeouts not set yet */
	TICKER_SET     = 2,  /* This entry has been created and timeouts set but not yet started */
	TICKER_STARTED = 3   /* This entry is created, set and started and is currently running */
} tickerState_t;

typedef struct
{
	tickerState_t  TickerState;
	U8           bOneShot;
	U32        uTimeout;
	U32        uTimeRemaining;
	sk_tick_pfn_t    pfnCallback;
	void           *pUserData;
	char           szName[MAX_TICK_NAME_LEN];
} tickerElement_t;




static U8 isInit = 0;

static tickerElement_t KALTickers[MAX_TICKS_NUM];

static sk_task_id_t tick_task_id;
static sk_sem_id_t  gfCriticalSectionSem;
static sk_sem_id_t  gfTickCallbackSem;
static sk_sem_id_t  gfTickTableSem;

static struct timeval StartUTCSystemTime;  //

static U32 kal_handle_time_timeouts(struct timeval* pOldUTCTime );
static void tick_task(void* p);
static sk_status_code_t internal_elapsed_time( struct timeval* pOldUTCSystemTime , U32  *puElapsedTime );
static sk_status_code_t init_os_tick_info();
static sk_status_code_t tick_find_free_table_and_lock (U32 *puFreeIndex,const char  *pszTickName );
static sk_status_code_t tick_store_name (U32     uTickTblIdx,  const char  *pszTickName );

#define TEST_VALID_CNXT_STATUS__RETURN( __kalstatus__ )                        \
{                                                                           \
	if ( __kalstatus__ != SK_SUCCESS )                                   \
{                                                                        \
	SK_ERROR (("ERROR "));\
	return SK_FAILED;                                    \
}                                                                        \
}

#define IS_VALID_TICKID( x )                                \
	(                                                     \
	(                                                  \
	((x)>= 0)&& (x < MAX_TICKS_NUM)    \
	) ? 1 : 0                                          \
	)

/*********************************************
*
**********************************************/
#ifdef SK_ANDROID
static void  com_time_systick_handler(sk_tick_id_t tick_id, void* ptr);

static sk_tick_id_t g_shSysTick = SK_INVALID_HANDLE;
static U32 g_su32SysTick = 0;		//100ms
#endif

sk_status_code_t sk_clock_get_sys_time(U32* const p_sys_time_ms)
{
	if (NULL == p_sys_time_ms)
	{
		SK_ERROR(("param is NULL!"));
		return SK_FAILED;
	}
	
#ifdef SK_ANDROID
#if 0//thread stops run for no reason
	*p_sys_time_ms = g_su32SysTick * 100;
	return SK_SUCCESS;
#else
	struct timeval time;
	gettimeofday(&time, NULL);
	*p_sys_time_ms = time.tv_sec * 1000 + time.tv_usec / 1000;
	return SK_SUCCESS;
#endif
#else
	if(p_sys_time_ms == NULL)
		return SK_ERROR_BAD_PARAMETER;
	return internal_elapsed_time(&StartUTCSystemTime,p_sys_time_ms);
#endif
}

U32 sk_clock_sys_time_minus(const U32 start_time, const U32 end_time)
{
	if (end_time >= start_time)
	{
		return end_time -start_time;
	}

	return (0xFFFFFFFF - end_time + start_time);
}

U32 sk_clock_sys_time_plus(const U32 start_time, const U32 end_time)
{
	return (end_time + start_time);
}

U32 sk_clock_sys_time_persec()
{
	return 1000;
}

sk_status_code_t sk_clock_get_sys_tick(U32* const p_sys_tick)
{
	return SK_ERROR_FEATURE_NOT_SUPPORTED; 
}


/* 设置基础时间的补偿，主要在得到tdt后设置linux的时间与StartUTCSystemTime 得到的初试值冲突的问题
aspec 1 is 正方向  －1 是负方向
u32Sec 秒*/
void sky_clock_set_equalize(S32 aspec , U32 sec)
{
	if(aspec > 0)
	{
		StartUTCSystemTime.tv_sec = sec;
	}
	else  //应该不会到这里
	{
		StartUTCSystemTime.tv_sec = sec;
	}
}


/* 设置系统的时间*/
sk_status_code_t sk_clock_set_sys_time(const U32  new_sys_time_ms)
{
	
	return SK_ERROR_FEATURE_NOT_SUPPORTED;
}

sk_status_code_t sk_clock_get_sys_freg(U32* const p_sys_freg)
{	
	return SK_ERROR_FEATURE_NOT_SUPPORTED;
}




sk_status_code_t sk_tick_init(void)
{
	return init_os_tick_info();
}

sk_status_code_t sk_tick_term(void)
{
	return SK_SUCCESS;
}

static sk_status_code_t init_os_tick_info()
{
	sk_status_code_t    KALStatus;

	U32         uLoop;
	if (isInit)
	{
		return SK_SUCCESS;
	}	

#ifdef SK_ANDROID
	g_su32SysTick = 0;
#endif

	gettimeofday(&StartUTCSystemTime,NULL);
	/* Create semaphore as currently free */
	KALStatus = sk_sem_create (&gfCriticalSectionSem,"CRIT",1);

	/* Create the Tick semaphore as NOT free */
	KALStatus = sk_sem_create ( &gfTickCallbackSem, "TICK", 0 );

	/* Create the Tick table protection semaphore as free */
	KALStatus = sk_sem_create (  &gfTickTableSem , "TICTBL",1);


	/* Create the thread that will monitor when callbacks need to be performed */
	/* Tick manager threead shuuld have higher priority than any other KAL thread 
	** Async callback manager thread has a priority of DEFAULT + 20. Hence 
	** bumping this thread priority by + 30 */
	if(SK_SUCCESS != sk_task_create (&tick_task_id,"TickCB",(sk_task_pfn_t)tick_task,(void*)NULL,NULL,TICK_THREAD_STACK_SIZE,SK_TASK_DEFAULT_PRIORITY))
	{
		SK_ERROR(("can not create task for tick"));
	}

	

	for ( uLoop = 0; uLoop < MAX_TICKS_NUM; uLoop++ )
	{
		KALTickers[uLoop].TickerState = TICKER_FREE;
	}

#ifdef SK_ANDROID
	sk_tick_create(&g_shSysTick, com_time_systick_handler, NULL);
	sk_tick_set(g_shSysTick, SK_TICK_TYPE_PERIO, 100);
	sk_tick_start(g_shSysTick);
#endif

	isInit = 1;
	return SK_SUCCESS;
}

/******************************************************************************/
/*  FUNCTION:    sk_tick_create                                         */
/*                                                                            */
/*  DESCRIPTION: This function creates an application tick timer using the    */
/*               shared hardware tick timer                                   */
/*                                                                            */
/*  INPUTS:      pfnCallback - Function to call when tick counter expires.    */
/*               pUserData - A value which will be passed back whenever the   */
/*                           tick counter callback function is called.        */
/*               tick_id - Caller supplied storage for the newly created tick ID*/
/*                                                                            */
/*  OUTPUTS:     None.                                                        */
/*                                                                            */
/*  RETURNS:     sk_status_code_t                                                 */
/*                  SK_SUCCESS          - Success.                                 */
/*                                                                            */
/*  NOTES:       None.                                                        */
/*                                                                            */
/*                                                                            */
/******************************************************************************/
sk_status_code_t sk_tick_create(sk_tick_id_t * const  p_tick_id, const sk_tick_pfn_t  tick_pfn, void * p_param)
{
	U32     uFreeIndex = -1;
	sk_status_code_t KALStatus;
	*p_tick_id  = SK_INVALID_ID;

	KALStatus = tick_find_free_table_and_lock ( &uFreeIndex, NULL );
	TEST_VALID_CNXT_STATUS__RETURN ( KALStatus );   

	/* Create / Store the timer name */
	KALStatus = tick_store_name ( uFreeIndex, NULL );  
	if ( KALStatus == SK_SUCCESS )
	{
		KALStatus = sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY );
		//TEST_VALID_CNXT_STATUS__RETURN ( KALStatus );
		KALTickers[uFreeIndex].pfnCallback = tick_pfn;
		KALTickers[uFreeIndex].pUserData = p_param;
		sk_sem_unlock( gfTickTableSem );
		if ( KALStatus == SK_SUCCESS )
		{
			*p_tick_id = uFreeIndex ;         
		}
	}
	else
	{
		SK_ERROR(("the error create the tick  "));
		KALStatus = SK_ERROR_BAD_PARAMETER;
	}

	return KALStatus;
}


sk_status_code_t sk_tick_destroy(const sk_tick_id_t tick_id)
{
	sk_status_code_t KALStatus = SK_FAILED;
	U32 uTickTblIdx;

	if(!IS_VALID_TICKID(tick_id))
	{
		SK_ERROR(("the error tick id "));
		return SK_ERROR_BAD_PARAMETER;
	}
	uTickTblIdx =  tick_id;
	KALStatus = sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY );      
	if ( KALTickers[uTickTblIdx].TickerState == TICKER_FREE )
	{
		/* This ticker was never created! (Or was created and
		** then destroyed. */
		/* MLP: I think this should return CNXT_STATUS_BAD_PARAMETER 
		** but the test harness checks for 'not available' */
		SK_ERROR(("the tick id [%ld] have destroyed ", tick_id));
		KALStatus = SK_ERROR_BAD_PARAMETER;
	}
	else
	{
		/* Free this entry */
		KALTickers[uTickTblIdx].TickerState = TICKER_FREE;    
		KALStatus = SK_SUCCESS;
	}
	sk_sem_unlock ( gfTickTableSem );

	return KALStatus;
}

/******************************************************************************/
/*  FUNCTION:    sk_tick_start                                          */
/*                                                                            */
/*  DESCRIPTION: This function starts a timer previously allocated using a    */
/*               call to cnxt_kal_tick_create.                                */
/*                                                                            */
/*  INPUTS:      TickId - The ID of the tick counter to start.                */
/*                                                                            */
/*  OUTPUTS:     None.                                                        */
/*                                                                            */
/*  RETURNS:     sk_status_code_t                                                 */
/*                  OK             - Success.                                 */
/*                  NOT_INIT       - Kal has not been initialized.            */
/*                  INTERNAL_ERROR - Error returned from library function.    */
/*                  BAD_PARAMETER  - Timer was invalid.                       */
/*                  NOT_SET        - Timer was not created yet.               */
/*                                                                            */
/*  NOTES:       It is not defined if a timer that has been restarted after   */
/*               a stop should reinitialize the timeout or just keep what     */
/*               was the last updated timeout -- our implmementation will     */
/*               simply keep the remainder of the timeout.  Which means that  */
/*               if the last time this timer was running and it was stopped   */
/*               with 1 millisecond remaining then it will timeout almost     */
/*               immediately.                                                 */
/*                                                                            */
/*  CONTEXT:     This function must be called from any context.               */
/*                                                                            */
/******************************************************************************/
sk_status_code_t sk_tick_start(const sk_tick_id_t  tick_id)
{
	sk_status_code_t KALStatus = SK_SUCCESS;
	U32   uTickTblIdx;



	if ( !IS_VALID_TICKID (tick_id) )
	{
		SK_ERROR(("the error tick id "));
		return SK_ERROR_BAD_PARAMETER;
	}
	uTickTblIdx =  tick_id;
	KALStatus =sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY );        
	if ( KALTickers[uTickTblIdx].TickerState == TICKER_FREE )
	{
		SK_ERROR(("the tick id %ld not create ", tick_id));
		KALStatus = SK_ERROR_BAD_PARAMETER;          
	}
	else
	{
		if ( KALTickers[uTickTblIdx].TickerState == TICKER_CREATED )
		{
			SK_ERROR(("the tick id %ld not set param ", tick_id));
			KALStatus = SK_ERROR_BAD_PARAMETER; 
		}
		else
		{
			KALTickers[uTickTblIdx].uTimeRemaining = KALTickers[uTickTblIdx].uTimeout;
			KALTickers[uTickTblIdx].TickerState = TICKER_STARTED;
			KALStatus = SK_SUCCESS;

			/* Wake up our timeout processing thread */
			sk_sem_unlock( gfTickCallbackSem );      
		}
	}
	sk_sem_unlock(gfTickTableSem );
	return KALStatus;
}


sk_status_code_t sk_tick_stop(const sk_tick_id_t  tick_id)
{
	sk_status_code_t KALStatus = SK_SUCCESS;
	U32     uTickTblIdx;

	if ( !IS_VALID_TICKID (tick_id) )
	{
		SK_ERROR(("the error tick id "));
		return SK_ERROR_BAD_PARAMETER;
	}
	uTickTblIdx =  tick_id;
	KALStatus =sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY ); 
	if ( KALTickers[uTickTblIdx].TickerState == TICKER_STARTED )
	{
		/* Go back to the set, but not started, state.  Note that 
		** since timer is not being deleted, next time it is started
		** it should be started with the original timeout value (set in tick_set API) */
		KALTickers[uTickTblIdx].TickerState = TICKER_SET;
		KALTickers[uTickTblIdx].uTimeRemaining = KALTickers[uTickTblIdx].uTimeout;
		KALStatus = SK_SUCCESS;
	}
	else
	{
		if ( KALTickers[uTickTblIdx].TickerState == TICKER_SET )
		{
			/* We expect that the ticker was at least started.  But even
			** if it was only set, but not yet started, we will consider
			** that ok.  It does not hurt anything. */
			KALStatus = SK_SUCCESS;
		}
		else	if ( KALTickers[uTickTblIdx].TickerState == TICKER_CREATED )
		{
			KALStatus = SK_SUCCESS;
		}
		else  /* state == free */
		{
			KALStatus = SK_SUCCESS;
		}		
	}
	sk_sem_unlock(gfTickTableSem );
	return KALStatus;

}

/******************************************************************************/
/*  FUNCTION:    sk_tick_set                                            */
/*                                                                            */
/*  DESCRIPTION: This function sets the period and type of a tick counter     */
/*               previously allocated using a call to cnxt_kal_tick_create.   */
/*                                                                            */
/*  INPUTS:      TickId    - The ID of the tick counter to set.               */
/*               uPeriodMs - The desired tick counter period in milliseconds. */
/*               bOneShot  - If SK_TRUE, the tick counter is set to generate only*/
/*                           a single callback after it is started,           */
/*                           if FALSE, periodic callbacks are made.           */
/*                                                                            */
/*  OUTPUTS:     None.                                                        */
/*                                                                            */
/*  RETURNS:     CNXT_STATUS_                                                 */
/*                  OK             - Success.                                 */
/*                  NOT_INIT       - Kal has not been initialized.            */
/*                  INTERNAL_ERROR - Error returned from library function.    */
/*                  BAD_PARAMETER  - uPeriodMs or Timer was invalid.          */
/*                  NOT_SET        - Timer was not created yet.               */
/*                                                                            */
/*  NOTES:       None.                                                        */
/*                                                                            */
/*  CONTEXT:     This function must be called from any context.               */
/*                                                                            */
/******************************************************************************/
sk_status_code_t sk_tick_set(const sk_tick_id_t  tick_id, 
							const sk_tick_type_t tick_type,
							const U32 tick_time_ms)
{
	sk_status_code_t KALStatus;
	U32     uTickTblIdx;
	U8        bRestart = 0;

	if(!IS_VALID_TICKID(tick_id))
	{
		SK_ERROR(("the error tick id "));
		return SK_ERROR_BAD_PARAMETER;
	}


	uTickTblIdx = tick_id;
	KALStatus =sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY );  

	if ( KALTickers[uTickTblIdx].TickerState == TICKER_STARTED )
	{
		bRestart = 1;
		sk_sem_unlock ( gfTickTableSem );
		sk_tick_stop ( tick_id );
		KALStatus = sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY );            
	}
	/* We check for not TICKER_FREE here because this allows the app to 
	** set a new timeout for an existing ticker without destroying
	** it first and then recreating it.  */
	if ( KALTickers[uTickTblIdx].TickerState == TICKER_FREE )
	{
		/* This ticker was never created! (Or was created and
		** then destroyed. */
		SK_ERROR(("the tick id %ld is not create ", tick_id));
		KALStatus = SK_ERROR_BAD_PARAMETER;
	}
	else
	{
		KALTickers[uTickTblIdx].uTimeout = tick_time_ms;
		KALTickers[uTickTblIdx].uTimeRemaining = tick_time_ms;
		KALTickers[uTickTblIdx].bOneShot = tick_type;
		KALTickers[uTickTblIdx].TickerState = TICKER_SET;
	}

	sk_sem_unlock ( gfTickTableSem );
	if ( bRestart )
	{
		sk_tick_start ( tick_id );
	}

	return KALStatus;
}


sk_status_code_t sk_tick_get_info(const sk_tick_id_t  tick_id, 
								  sk_tick_type_t * const  p_tick_type,
								  U32 * const p_tick_time_ms, 
								  sk_tick_status_t * const p_tick_status)
{
	sk_status_code_t KALStatus;
	U32     uTickTblIdx;
	//U8        bRestart = 0;
	if(p_tick_type == NULL || p_tick_time_ms == NULL || p_tick_status == NULL)
		return SK_ERROR_BAD_PARAMETER;
	
	if(!IS_VALID_TICKID(tick_id))
	{
		return SK_ERROR_BAD_PARAMETER;
	}


	uTickTblIdx = tick_id;
	KALStatus =sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY );  

	*p_tick_type = KALTickers[uTickTblIdx].bOneShot;
	*p_tick_time_ms = KALTickers[uTickTblIdx].uTimeout;
	*p_tick_status = KALTickers[uTickTblIdx].TickerState;
	
	sk_sem_unlock ( gfTickTableSem );
	return SK_SUCCESS;
}

static void tick_task(void* p)
{
	sk_status_code_t    KALStatus;
	S32          iStatus;

	struct timeval OldUTCTime;

	/* Start off waiting for the first event */
	U32        uTimeout = SK_TIMEOUT_INFINITY;

	do
	{		
		iStatus = gettimeofday ( &OldUTCTime, NULL );

		//printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$44 \n");
		if ( iStatus == 0 )
		{
			/* This call will return when a new timer is started
			** (which will release the semaphore) or when the timeout
			** occurs.  The FIRST timeout will be a wait 'forever' until
			** an initial tick is set.  Then the amount of timeout is 
			** determined based on the last interation of timeout processing
			** which will find the next soonest tick that should expire.
			*/
			//SK_DEBUG(("tick task timeout set %d ", uTimeout));
			KALStatus = sk_sem_lock_timeout( gfTickCallbackSem, uTimeout );
			if(uTimeout == SK_TIMEOUT_INFINITY)
			{
				gettimeofday ( &OldUTCTime, NULL );
			}
			//KALStatus = sk_sem_lock( gfTickCallbackSem);
			if ( KALStatus == SK_SUCCESS || KALStatus == SK_ERROR_TIMEOUT)
			{
				/* See if any callback functions need to be invoked
				** and determine the next timeout value */
				//uTimeout = kal_handle_time_timeouts (&StartUTCSystemTime);				
				uTimeout = kal_handle_time_timeouts (&OldUTCTime);				
			}
			else
			{
				SK_ERROR(("Can't get timer callback semaphore, 0x%4lx", gfTickCallbackSem));
			}
		}
		else
		{
			SK_ERROR (("Can't get timeofday,"));
		}
	} while ( 1 );
}

static U32 kal_handle_time_timeouts(struct timeval* pOldUTCTime )
{
	/* Make sure we find at least one callback */
	U8           bFoundAtLeastOne = 0;
	U32        uTickTblIdx;
	U32        uNextShortestTimeout = SK_TIMEOUT_INFINITY;
	sk_status_code_t    KALStatus = SK_FAILED;
	U32        uElapsedTime;
	//sk_tick_id_t   TickId;
	uNextShortestTimeout = SK_TIMEOUT_INFINITY;   /* Init to max value */
	KALStatus = sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY );
	//KALStatus = sk_sem_lock( gfTickTableSem);
	TEST_VALID_CNXT_STATUS__RETURN ( KALStatus );

	/* Compute elapsed time in milliseconds.    */
	internal_elapsed_time ( pOldUTCTime, &uElapsedTime );

	/* Loop through entire table (over and over) until we find all timers
	** that have expired and executed their callbacks */
	for ( uTickTblIdx = 0; uTickTblIdx < MAX_TICKS_NUM; uTickTblIdx++ )
	{
		if ( KALTickers[uTickTblIdx].TickerState == TICKER_STARTED )
		{
			if ( KALTickers[uTickTblIdx].uTimeRemaining <= uElapsedTime )
			{
				//TickId = TICKID_FROM_INDEX ( uTickTblIdx );
				KALTickers[uTickTblIdx].pfnCallback ( uTickTblIdx,//TickId,
					KALTickers[uTickTblIdx].pUserData );	        

				/* If we have caused a callback then we don't know how
				** long we spent in that callback. It may have been a
				** significant amount of time so we have to recompute
				** the entire table again.
				**
				** WARNING -- If the callback sets a new timer
				** then this could cause a deadlock because
				** we have locked the semaphore protecting against
				** table changes!!!!
				**
				** The other option is we ignore the time spent
				** in the callback -- or use critical section!
				*/
				if ( KALTickers[uTickTblIdx].bOneShot )
				{
					/* Go back to 'set' but not started -- can't free it because
					** user might want to start it again. */
					KALTickers[uTickTblIdx].TickerState = TICKER_SET;
				}
				/* Reset timer */
				KALTickers[uTickTblIdx].uTimeRemaining = KALTickers[uTickTblIdx].uTimeout;
				bFoundAtLeastOne = 1;
			}
			else  /* Not ready for callback yet so just update timer */
			{
				/* Not timed out yet so just update the time remaining
				** for this guy */
				KALTickers[uTickTblIdx].uTimeRemaining -= uElapsedTime;
			}

			/* The timer state may have changed above so must be sure it
			** is still set THEN check for shortest timeout */
			if ( (KALTickers[uTickTblIdx].TickerState == TICKER_STARTED)
				&& (KALTickers[uTickTblIdx].uTimeRemaining < uNextShortestTimeout) )
			{
				/* Figure out which timer has the next shortest duration
				** before timeout */
				uNextShortestTimeout = KALTickers[uTickTblIdx].uTimeRemaining;	        
			}
		}        /* Else this table index is not in use or not yet started. */
	}           /* For all indices in the table */

	/* Put the semaphore back temporarily so others can set new ticks */
	KALStatus = sk_sem_unlock(gfTickTableSem );
	TEST_VALID_CNXT_STATUS__RETURN ( KALStatus );

	return uNextShortestTimeout;
}

static sk_status_code_t internal_elapsed_time( struct timeval* pOldUTCSystemTime , U32  *puElapsedTime )
{
	struct timeval CurrentUTCSystemTime;
	sk_status_code_t            iRetVal;
	
	if(pOldUTCSystemTime == NULL || puElapsedTime == NULL)
		return SK_FAILED;

	/* Now get the current time */
	iRetVal = gettimeofday ( &CurrentUTCSystemTime, NULL );
	if ( iRetVal == 0 )
	{
		/* Compute elapsed time in milliseconds.  This will wrap after 1193 days
		** but we are not even going to both accounting for that because that
		** makes no sense that any app will wait longer then and not have gotten
		** it's event.  */
		*puElapsedTime =
			(
			CurrentUTCSystemTime.tv_sec -
			pOldUTCSystemTime->tv_sec
			) *
			1000;
		if ( CurrentUTCSystemTime.tv_usec >= pOldUTCSystemTime->tv_usec )
		{
			*puElapsedTime +=
				(
				CurrentUTCSystemTime.tv_usec -
				pOldUTCSystemTime->tv_usec
				) /
				1000;
		}
		else
		{
			/* If microseconds have not fully wrapped then we actually did not get
			** the full value of the 'seconds' increase so we have to give some 
			** time back.  This delta we subtract is the 'unearned' microseconds.*/
			*puElapsedTime -=
				(
				pOldUTCSystemTime->tv_usec -
				CurrentUTCSystemTime.tv_usec
				) /
				1000;
		}
		iRetVal = SK_SUCCESS;
	}
	else
		iRetVal = SK_FAILED;

	return iRetVal;
}


/******************************************************************************/
/*  FUNCTION:    tick_find_free_table_and_lock                              */
/*                                                                            */
/*  DESCRIPTION: Finds a free slot in the semaphore table -- For internal use */
/*               only.                                                        */
/*                                                                            */
/*  INPUTS:      puFreeIndex - Caller supplied storage for the returned index */
/*               pszString   - name of device                                 */
/*                                                                            */
/*  OUTPUTS:     puFreeIndex - Free table index that was found                */
/*                                                                            */
/*  RETURN VALUES:                                                            */
/*    CNXT_STATUS_OK          - Free table index was found.                   */
/*    CNXT_STATUS_INTERNAL_ERROR - An error occurred in a library function.   */
/*                                                                            */
/*  CONTEXT:     For internal use only                                        */
/*                                                                            */
/*  NOTES:                                                                    */
/******************************************************************************/
static sk_status_code_t tick_find_free_table_and_lock (
	U32     *puFreeIndex,
	const char  *pszTickName )
{
	U32     uFreeTblIdx = -1;
	U32     uLoop;
	//S32       cmpStatus;
	sk_status_code_t KALStatus = SK_FAILED;
	//sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY );
	sk_sem_lock(gfTickTableSem);
	for ( uLoop = 0; uLoop < MAX_TICKS_NUM; uLoop++ )
	{
		if ( (uFreeTblIdx == (U32)(-1)) && KALTickers[uLoop].TickerState == TICKER_FREE )
		{
			/* Remember the FIRST free slot that we find */
			uFreeTblIdx = uLoop;
			KALStatus = SK_SUCCESS;
		}

		#if 0
		/* If there is a requested name then check all 'in use' slots and make
		** sure we don't  already have a timer by this name */
		if (KALTickers[uLoop].TickerState != TICKER_FREE )
		{         
			KALStatus = SK_ERROR_BAD_PARAMETER;
			SK_DEBUG_FILE_LINE();
			break;         
		}
		#endif
	}

	if ( KALStatus == SK_SUCCESS )
	{
		/* All is OK so reserve this slot */
		KALTickers[uFreeTblIdx].TickerState = TICKER_CREATED; /*Mark as used */
		*puFreeIndex = uFreeTblIdx;
	}

	sk_sem_unlock( gfTickTableSem );
	return KALStatus;
}  /* end kal_hw_find_free_table_and_lock */

/******************************************************************************/
/*  INTENAL FUNCTION!                                                         */
/*                                                                            */
/*  FUNCTION:    tick_store_name                                          */
/*                                                                            */
/*  DESCRIPTION: This function will create a unique tick name if one          */
/*               was not specified -- otherwise it will save the name         */
/*               requested.                                                   */
/*                                                                            */
/*  INPUTS:      pszTickName - A string used to identify the semaphore.       */
/*               uTickTblIdx - Index in the semaphore table where the sem     */
/*                             name should be stored.                         */
/*                                                                            */
/*  OUTPUTS:     None.                                                        */
/*                                                                            */
/*  RETURN VALUES:                                                            */
/*    CNXT_STATUS_OK             - tick was created successfully.             */
/*    CNXT_STATUS_INTERNAL_ERROR - An error occurred in a library function.   */
/*    CNXT_STATUS_BAD_PARAMETER  - The uInitialValue or pszTickName parameters*/
/*                                  were invalid, or pSemId was NULL.         */
/*                                                                            */
/*  CONTEXT:     This function must be called from non-interrupt context.     */
/*                                                                            */
/*  NOTES:                                                                    */
/******************************************************************************/
static sk_status_code_t tick_store_name (
							   U32     uTickTblIdx,
							   const char  *pszTickName )
{
	char        szTempName[MAX_TICK_NAME_LEN];
	char        *pszFinalName;
	sk_status_code_t KALStatus;
	U32     uStrLen;
	if ( pszTickName != NULL )
	{
		/* The calling application supplied a name so use it */
		uStrLen = strlen ( pszTickName);
		/* Save for final copy -- Use requested name*/
		pszFinalName = (char *)pszTickName;
	}
	else
	{
		/* No name specified so create our own unique name */
		KALStatus = sprintf ( szTempName, "Tick-0x%lx", uTickTblIdx );      
		uStrLen = strlen ( szTempName);      
		pszFinalName = szTempName;
	}

	KALStatus = sk_sem_lock_timeout( gfTickTableSem, SK_TIMEOUT_INFINITY );   

	/* Store the selected name in our table. */
	strncpy ( KALTickers[uTickTblIdx].szName,
		pszFinalName,
		MAX_TICK_NAME_LEN );


	/*Just incase the name was exactly max size we will force a null terminator*/
	KALTickers[uTickTblIdx].szName[MAX_TICK_NAME_LEN - 1] = 0;
	sk_sem_unlock( gfTickTableSem );
	return SK_SUCCESS;
}


void maketimespec(struct timespec * pSpec, U32 u32TimeOutMs)
{
	struct timeval tm;
		
	if(pSpec == NULL)
		return ;

	if (0 != gettimeofday(&tm, NULL))
	{
		SK_ERROR(("[message_receive_timeout] fatal error, time wrong !!!!!!!!!!\n"));
	}
	
	pSpec->tv_sec = tm.tv_sec +(u32TimeOutMs) / 1000;
	pSpec->tv_nsec = tm.tv_usec*1000 + ((u32TimeOutMs) % 1000) * 1000000;	//modify the u sec to n sec error.
	//SK_DEBUG(("the ns is %d", pSpec->tv_nsec));
#if 0
	pSpec->tv_nsec = pSpec->tv_nsec % 1000000000;  //not more than 1 sec. 
#else
	if (pSpec->tv_nsec >= (1E+9))
	{
		pSpec->tv_sec++;
		pSpec->tv_nsec -= (1E+9);
	}
#endif
	//SK_DEBUG(("the ns is %d", pSpec->tv_nsec));
}

#ifdef SK_ANDROID
static void  com_time_systick_handler(sk_tick_id_t tick_id, void* ptr)
{
	g_su32SysTick++;
}
#endif

