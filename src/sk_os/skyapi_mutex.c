#include "sk_os.h"
#include <pthread.h>
#include "sk_tbx.h"
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <time.h> 

sk_status_code_t sk_mutex_create(sk_mutex_id_t *const p_mutex_id)
{	
	pthread_mutex_t* t = NULL;
	if(p_mutex_id == NULL)
	{
		return SK_ERROR_BAD_PARAMETER;
	}

	t = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if ( !t )
	{
		SK_ERROR(("no memory!"));
		return SK_ERROR_NO_MEMORY;
	}
	memset(t, 0x00, sizeof(pthread_mutex_t));
	
	if(0 == pthread_mutex_init(t, NULL))
	{		
		
		*p_mutex_id = (sk_mutex_id_t)(t);
		return SK_SUCCESS;	
	}	
	free(t);
	*p_mutex_id = SK_INVALID_ID;
	SK_ERROR(("create mutex is error %s", strerror(errno)));
	return SK_FAILED;
}

//
sk_status_code_t sk_mutex_create_ex(sk_mutex_id_t *const p_mutex_id)
{	
	pthread_mutexattr_t ma;
	pthread_mutex_t * t = NULL;

	if(p_mutex_id == NULL)
	{
		return SK_ERROR_BAD_PARAMETER;
	}

	t = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if ( !t )
	{
		SK_ERROR(("no memory!"));
		return SK_ERROR_NO_MEMORY;
	}
	memset(t, 0x00, sizeof(pthread_mutex_t));

	pthread_mutexattr_init(&ma);
	pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_DEFAULT);
	
	if(0 == pthread_mutex_init(t, &ma))
	{		
		
		*p_mutex_id = (sk_mutex_id_t)(t);
		pthread_mutexattr_destroy(&ma);
		return SK_SUCCESS;	
	}
	pthread_mutexattr_destroy(&ma);
	free(t);
	*p_mutex_id = SK_INVALID_ID;
	SK_ERROR(("create mutex is error %s ",strerror(errno)));
	return SK_FAILED;
}

sk_status_code_t sk_mutex_destroy(const sk_mutex_id_t mutex_id)
{
	pthread_mutex_t* t = (pthread_mutex_t*)mutex_id;
	
	if( (SK_INVALID_ID==mutex_id)||(0==mutex_id))
	{
		SK_ERROR(("mutex id is error id. "));
		return SK_ERROR_BAD_PARAMETER;
	}
	
	if(0 == pthread_mutex_destroy(t))
	{
		memset(t, 0x00, sizeof(pthread_mutex_t));
		free(t);
		return SK_SUCCESS;	
	}
	
	SK_ERROR(("destroy  mutex is error"));
	return SK_FAILED;

}

sk_status_code_t sk_mutex_lock(const sk_mutex_id_t mutex_id, U32 timeout_ms)
{
	pthread_mutex_t* t = (pthread_mutex_t*)mutex_id;
	struct timespec ts;
	int err=0;
	
	if( (SK_INVALID_ID==mutex_id)||(0==mutex_id))
	{
		SK_ERROR(("mutex id is error id. "));
		return SK_ERROR_BAD_PARAMETER;
	}
	//SK_DEBUG(("lock %p ",t));

	if(timeout_ms == SK_TIMEOUT_INFINITY)
	{
		if(0 == pthread_mutex_lock(t))
		{
			//SK_DEBUG(("lock %p ",t));
			return SK_SUCCESS;	
		}
	}
	else
	{
		maketimespec(&ts,timeout_ms);
		err=pthread_mutex_timedlock(t, &ts);
		if(err==0)
		{
			return SK_SUCCESS;		
		}
		else //if(err==ETIMEDOUT)
		{
			//printf(" lock mutex timeout: %d\n", err); 
		}
	}
	
	SK_ERROR(("lock mutex is error %s,%d",strerror(errno),err));
	return SK_FAILED;

}

sk_status_code_t sk_mutex_unlock(const sk_mutex_id_t mutex_id)
{
	pthread_mutex_t* t = (pthread_mutex_t*)mutex_id;
	
	if( (SK_INVALID_ID==mutex_id)||(0==mutex_id))
	{
		SK_ERROR(("mutex id is error id. "));
		return SK_ERROR_BAD_PARAMETER;
	}
	//SK_DEBUG(("un lock %p ",t));
	if(0 == pthread_mutex_unlock(t))
	{
		//SK_DEBUG(("un lock %p ",t));
		return SK_SUCCESS;	
	}
	
	SK_ERROR(("unlock mutex is error %s ",strerror(errno)));
	return SK_FAILED;

}


