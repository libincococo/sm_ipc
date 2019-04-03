#include "sk_os.h"
#include <semaphore.h>
#include "sk_tbx.h"
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

sk_status_code_t  sk_sem_create(sk_sem_id_t * const  p_sem_id, const char * p_sem_name,const  U32 init_count)
{
	if(NULL == p_sem_id )
	{
		SK_ERROR(("error in create sem"));
		return SK_ERROR_BAD_PARAMETER;
	}

	sem_t *sem = (sem_t *)malloc(sizeof(sem_t));

	if (!sem) 
	{
		SK_ERROR(("error in create sem"));
		*p_sem_id = SK_INVALID_ID;	
		return SK_ERROR_NO_MEMORY;
	}
	/*
	* here we allocate the semaphore memory. i have to remind myself
	* the memory is mine. so i can free it when it useless.
	*/
	// SAVE THE POINTER
	memset (sem, 0, sizeof(sem_t));
	if (0 != sem_init(sem, 0, init_count)) 
	{
		if(sem != NULL) 
		{
			free (sem);
			sem = NULL;
		}
		
		*p_sem_id = SK_INVALID_ID;	
		SK_ERROR((" Semaphore create error %s ",strerror(errno)));
		return SK_FAILED;
	}
	*p_sem_id = (sk_sem_id_t)(sem);
	return SK_SUCCESS;
}

sk_status_code_t  sk_sem_destroy(const sk_sem_id_t sem_id)
{
	sem_t * Semaphore = (sem_t *)sem_id;

	if(Semaphore == NULL ||SK_INVALID_ID == sem_id )
	{
		SK_ERROR(("error in destroy sem"));
		return SK_ERROR_BAD_PARAMETER;
	}

	if( 0 == sem_destroy(Semaphore))
	{
		free (Semaphore);
		return SK_SUCCESS;
	}

	SK_ERROR((" Semaphore destroy error %s ",strerror(errno)));
	return SK_FAILED;
}

sk_status_code_t  sk_sem_lock(const sk_sem_id_t sem_id)
{
	int ret = 0;
	sem_t * Semaphore = (sem_t *)sem_id;

	if(sem_id == 0 || SK_INVALID_ID == sem_id)
	{
		SK_ERROR(("Semaphore param is bad. "));
		return SK_ERROR_BAD_PARAMETER;
	}

	ret = sem_wait(Semaphore);
	if(ret != 0)
		return SK_FAILED;
	else
		return SK_SUCCESS;
}

sk_status_code_t  sk_sem_lock_timeout(const sk_sem_id_t sem_id, const U32 timeout_ms)
{
	int ret = -1;	
	sem_t * Semaphore = (sem_t *)sem_id;

	if (Semaphore == NULL || SK_INVALID_ID == sem_id) 
	{
		SK_ERROR(("error in lock "));
		return SK_ERROR_BAD_PARAMETER;
	}

	/* normal block wait */
	if (SK_TIMEOUT_INFINITY == timeout_ms) 
	{
		ret =  sem_wait(Semaphore);
	}	
	else
	{
		struct timeval    UTCSystemTime;
   		struct timespec   absTimeout;
		U32           uSleepMilliseconds;
   
		gettimeofday ( &UTCSystemTime, NULL );
		uSleepMilliseconds = timeout_ms + ( UTCSystemTime.tv_usec / 1000 );

		/* Now convert those milliseconds to seconds and add the current number
		** of seconds in the current system clock. */
		absTimeout.tv_sec = UTCSystemTime.tv_sec + uSleepMilliseconds / 1000;

		/* Now figure out how many milliseconds were left over that were too
		** small to be converted to seconds and convert to nanoseconds */
		uSleepMilliseconds = uSleepMilliseconds % 1000;
		absTimeout.tv_nsec = uSleepMilliseconds * 1000 * 1000;

		//ret = sem_timedwait(Semaphore,&absTimeout );
		ret = sem_wait(Semaphore );
	}
	if(ret == 0)
		return SK_SUCCESS;
	return SK_ERROR_TIMEOUT;
}



sk_status_code_t  sk_sem_unlock(const sk_sem_id_t sem_id)
{
	int ret = -1;
	sem_t * Semaphore = (sem_t *)sem_id;

	if(Semaphore != NULL && SK_INVALID_ID != sem_id)
	{
		ret = sem_post(Semaphore);		
	}
	if(ret == 0)
		return SK_SUCCESS;
	return SK_FAILED;
}

sk_status_code_t  sk_sem_count(const sk_sem_id_t sem_id, S32 * p_count)
{
	int nRet = -1;
	sem_t * pSemaphore = (sem_t *)sem_id;
	int nValue = 0;
	
	if (pSemaphore == NULL || SK_INVALID_ID == sem_id || p_count == NULL)
	{
		SK_ERROR(("Bad param \n"));
		
		return SK_ERROR_BAD_PARAMETER;
	}
	
	nRet = sem_getvalue(pSemaphore, &nValue); 
	if (nRet != 0)
	{
		SK_ERROR(("Failed to get sem value %s ",strerror(errno)));		
		return SK_FAILED;
	}

	*p_count = nValue;

	return SK_SUCCESS;
}

