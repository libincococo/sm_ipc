#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdlib.h>

#include "priv_mac.h"

/*******************************************************
* event  function. these fun conx not implement . will use linux fun
*******************************************************/

sk_status_code_t sk_event_create(sk_evt_id_t * const p_event_id, const U32 init_is_set)
{
	int rc = -1;
	skyapi_event_t *e = NULL;
	if(NULL == p_event_id)
		return SK_ERROR_BAD_PARAMETER;

	e = (skyapi_event_t *) malloc (sizeof(skyapi_event_t));
	if (e == NULL) 		
	{
		SK_ERROR(("not malloc mem. "));
		return SK_ERROR_NO_MEMORY;
	}

	memset(e, 0, sizeof(skyapi_event_t));

	rc = pthread_mutex_init(&e->mutex, NULL);
	if(rc != 0)
	{
		free(e);
		SK_ERROR(("event id [%ld] error %s. ", *p_event_id,strerror(errno)));
		return SK_FAILED;
	}
	rc = pthread_mutex_lock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", *p_event_id,strerror(errno)));
		pthread_mutex_destroy(&e->mutex);
		free(e);
		return SK_FAILED;
		
	}
	rc = pthread_cond_init(&e->cond, NULL);	
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", *p_event_id,strerror(errno)));
		rc = pthread_mutex_unlock(&e->mutex);
		rc = pthread_mutex_destroy(&e->mutex);
		free(e);
		return SK_FAILED;
	}
	
	if(init_is_set)
		e->state = 1;
	else
		e->state = 0;
	
	*p_event_id = (sk_evt_id_t )(e);
	rc = pthread_mutex_unlock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", *p_event_id,strerror(errno)));
		rc = pthread_mutex_destroy(&e->mutex);
		rc = pthread_cond_destroy(&e->cond);
		free(e);
		*p_event_id = SK_INVALID_ID;
		return SK_FAILED;
	}
	return SK_SUCCESS;

}

sk_status_code_t sk_event_destroy(const sk_evt_id_t  event_id)
{
	int rc;

	skyapi_event_t * e = (skyapi_event_t*)(event_id);

	if (e == NULL || SK_INVALID_ID == event_id)
	{
		SK_ERROR(("event id is error [%ld] ", event_id));
		return SK_ERROR_BAD_PARAMETER;
	}
	

	rc = pthread_mutex_lock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", event_id,strerror(errno)));
	}

	rc = pthread_cond_signal(&e->cond);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s.", event_id,strerror(errno)));
	}
	
	rc = pthread_cond_destroy(&e->cond);
	if(rc != 0)
	{
		SK_ERROR(("destroy event [%ld] cond failed %s", event_id,strerror(errno)));
	}
	
	rc = pthread_mutex_unlock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", event_id,strerror(errno)));
	}

	rc = pthread_mutex_destroy(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("destroy event [%ld] mutex failed %s", event_id,strerror(errno)));
	}
	
	free(e);
	return SK_SUCCESS;	
}

//#define JIFFIES_TIMER

sk_status_code_t sk_event_wait(const sk_evt_id_t  event_id, const U32 is_auto_reset, const U32 timeout_ms)
{	
	struct timespec tout;
	int rc = 0;
	
	skyapi_event_t * e = (skyapi_event_t*)(event_id);

	if (e == NULL|| SK_INVALID_ID == event_id) 
	{
		return SK_ERROR_BAD_PARAMETER;
	}

	if (timeout_ms == SK_TIMEOUT_INFINITY) 
	{
		rc = pthread_mutex_lock(&e->mutex);
		if(rc != 0)
		{
			SK_ERROR(("event id [%ld] error. ", event_id));
		}
		
		while (e->state != 1)
			pthread_cond_wait(&e->cond, &e->mutex);

		//that state will set is_auto_reset
		if( is_auto_reset )
			e->state = 1;
		else
			e->state = 0;
	
		rc = pthread_mutex_unlock(&e->mutex);
		if(rc != 0)
		{
			SK_ERROR(("event id [%ld] error %s. ", event_id,strerror(errno)));
		}
		
		return SK_SUCCESS;
	}
	
	maketimespec(&tout,timeout_ms);

	rc = pthread_mutex_lock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error. ", event_id));
	}
	while (e->state != 1) 
	{
		rc = pthread_cond_timedwait(&e->cond, &e->mutex, &tout);
		if (rc == ETIMEDOUT) 
		{		
			pthread_mutex_unlock(&e->mutex);
			return SK_ERROR_TIMEOUT;
		}
	}
	
	if( is_auto_reset )
		e->state = 1;
	else
		e->state = 0;
	
	rc = pthread_mutex_unlock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", event_id,strerror(errno)));
	}
	return SK_SUCCESS;

}

sk_status_code_t sk_event_set(const sk_evt_id_t  event_id)
{
	skyapi_event_t * e = (skyapi_event_t*)(event_id);
	int rc;
	
	if (e == NULL|| SK_INVALID_ID == event_id) 		
		return  SK_ERROR_BAD_PARAMETER;
	
	rc = pthread_mutex_lock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", event_id,strerror(errno)));
	}
	e->state = 1;
	rc = pthread_cond_signal(&e->cond);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", event_id,strerror(errno)));
	}
	rc = pthread_mutex_unlock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", event_id,strerror(errno)));
	}

	return SK_SUCCESS;
}

/* set this event to no singal status*/
sk_status_code_t sk_event_reset(const sk_evt_id_t  event_id)
{	
	int rc;
	skyapi_event_t * e = (skyapi_event_t*)(event_id);
	
	if (e == NULL|| SK_INVALID_ID == event_id) 		
		return  SK_ERROR_BAD_PARAMETER;

	rc = pthread_mutex_lock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s. ", event_id,strerror(errno)));
	}
	e->state = 0;	
	rc = pthread_mutex_unlock(&e->mutex);
	if(rc != 0)
	{
		SK_ERROR(("event id [%ld] error %s.", event_id,strerror(errno)));
	}
	return SK_SUCCESS;
}



