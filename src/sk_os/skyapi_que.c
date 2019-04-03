
#include <stdlib.h>
#include <memory.h>

#include "priv_mac.h"

/**************************************
*
**************************************/
#define MAX_QUE_COUNT 40
#define QUE_MSG_SIZE    sizeof(sk_queue_msg_t)//(4 * sizeof (SKY_QUEUE_MSG))
#define MQ_MAGICNUM 	0x98765432

#define IS_VALID_ID(x) ((x!= 0&&x!=SK_INVALID_ID)?1:0)	

sk_status_code_t  sk_queue_create(sk_queue_id_t * const p_queue_id, const char * p_queue_name, const U32 max_msg_count)
{
	#if 1
	return sk_queue_create_ex(p_queue_id, p_queue_name,max_msg_count,sizeof(sk_queue_msg_t));
	#else
	MsgQ * mq = NULL;
	if(p_queue_id == NULL)
	{
		SK_ERROR(("queue id is null."));
		return SK_ERROR_BAD_PARAMETER;
	}

	*p_queue_id = SK_INVALID_ID;
	mq =(MsgQ *) malloc(sizeof(MsgQ) + max_msg_count * QUE_MSG_SIZE);

	if (NULL == mq) 
		return SK_ERROR_NO_MEMORY;
	
	memset (mq, 0, sizeof(MsgQ) + max_msg_count * QUE_MSG_SIZE);

	/* initialize the remain member */
	mq->read = mq->write = mq->refcnt = 0;
	mq->magicnum = MQ_MAGICNUM;
	mq->mq_maxmsg = max_msg_count;
	mq->mq_msgsize = QUE_MSG_SIZE;
	pthread_mutex_init(&mq->mutex, NULL);
	pthread_cond_init(&mq->read_cond, NULL);
	pthread_cond_init(&mq->write_cond, NULL);

	*p_queue_id  = (sk_queue_id_t)mq;
	return SK_SUCCESS;
	#endif
}

sk_status_code_t  sk_queue_send(const sk_queue_id_t queue_id, const sk_queue_msg_t msg, const U32 timeout_ms)
{
	#if 1
	return sk_queue_send_ex(queue_id, (void*)msg,sizeof(sk_queue_msg_t),timeout_ms);
	#else
	MsgQ * mq = NULL;	
	U8 empty;
	
	struct timespec ts;
	
	int ret = 0;

	
	if(!IS_VALID_ID(queue_id))
	{
		SK_ERROR(("queue id is invalid."));
		return SK_ERROR_BAD_PARAMETER;
	}
	
	mq = (MsgQ*)queue_id;
	if(msg == NULL ||mq->magicnum != MQ_MAGICNUM)
	{
		return SK_ERROR_BAD_PARAMETER;
	}

	mq->refcnt++;

	/* send message */
	pthread_mutex_lock(&mq->mutex);

	if(timeout_ms != SK_TIMEOUT_INFINITY)
	{
		#if 0
		struct timeval tm;		
		if (0 != gettimeofday(&tm, NULL))
			SK_ERROR(("[message_receive_timeout] fatal error, time wrong !!!!!!!!!!\n"));

		
		//tm.tv_sec +=(u32TimeOutMs) / 1000;
		//tm.tv_nsec += (u32TimeOutMs) % 1000 * 1000000;	
		ts.tv_sec = tm.tv_sec +(u32TimeOutMs) / 1000;
		ts.tv_nsec = tm.tv_usec + (u32TimeOutMs) % 1000 * 1000000;
		#endif
		maketimespec(&ts,timeout_ms);

	}
	
	if ((mq->read + mq->mq_maxmsg - mq->write) % mq->mq_maxmsg == 1) 
	{        
		if(timeout_ms == SK_TIMEOUT_INFINITY)
		{
			ret = pthread_cond_wait(&mq->read_cond, &mq->mutex);
		}
		else if(timeout_ms == SK_TIMEOUT_IMMEDIATE)
		{
			ret = ETIMEDOUT;
		}			
		else
		{
			ret = pthread_cond_timedwait(&mq->read_cond, &mq->mutex, &ts);
		}
		if ((ret == ETIMEDOUT) || (mq->magicnum != MQ_MAGICNUM)) 
		{	
			pthread_mutex_unlock(&mq->mutex);
			mq->refcnt--;
			SK_ERROR(("error to send msg %d,magic num is %x,ret is %d>>>>>\n", mq->refcnt,mq->magicnum,ret));
			return SK_ERROR_TIMEOUT;
		}				
	}
	memcpy (mq->msgbuffer + mq->write * mq->mq_msgsize, (void *)msg, mq->mq_msgsize);
	mq->write++;
	if (mq->write == mq->mq_maxmsg) 
		mq->write = 0;
	empty = (mq->write == mq->read);
	if (!empty) 
		pthread_cond_signal(&mq->read_cond);
	pthread_mutex_unlock(&mq->mutex);

	mq->refcnt--;
	return SK_SUCCESS;
	#endif
}


sk_status_code_t  sk_queue_receive(const sk_queue_id_t queue_id,const  sk_queue_msg_t msg, const  U32 timeout_ms)
{
	#if 1
	return sk_queue_receive_ex(queue_id, (void*)msg,sizeof(sk_queue_msg_t),timeout_ms);
	#else
	MsgQ * mq = NULL;	
	//U8 empty;
	int ret = 0;
	U8 isfull;
	//struct timespec tm;
	struct timespec ts;
	
	if(!IS_VALID_ID(queue_id))
	{
		SK_ERROR(("queue id is invalid."));
		return SK_ERROR_BAD_PARAMETER;
	}
	mq = (MsgQ*)queue_id;
	if(msg == NULL ||mq->magicnum != MQ_MAGICNUM)
	{
		return SK_ERROR_BAD_PARAMETER;
	}

	

	/* internal malloc */
	pthread_mutex_lock(&mq->mutex);
	mq->refcnt++;	
	if(timeout_ms != SK_TIMEOUT_INFINITY)
	{
		#if 0
		struct timeval tm;
		if (0 != gettimeofday(&tm, NULL))
			SK_ERROR(("[message_receive_timeout] fatal error, time wrong !!!!!!!!!!\n"));

		
		//tm.tv_sec +=(u32TimeOutMs) / 1000;
		//tm.tv_nsec += (u32TimeOutMs) % 1000 * 1000000;	
		ts.tv_sec = tm.tv_sec +(u32TimeOutMs) / 1000;
		ts.tv_nsec = tm.tv_usec + (u32TimeOutMs) % 1000 * 1000000;
		#endif
		maketimespec(&ts,timeout_ms);
	}
	
	if (mq->read == mq->write) 
	{
		if(timeout_ms == SK_TIMEOUT_INFINITY)
		{
			ret = pthread_cond_wait(&mq->read_cond, &mq->mutex);
		}
		else if(timeout_ms == SK_TIMEOUT_IMMEDIATE)
		{
			ret = ETIMEDOUT;
		}
		else 
		{
			ret = pthread_cond_timedwait(&mq->read_cond, &mq->mutex, &ts);
		}
		if ((ret == ETIMEDOUT) || (mq->magicnum != MQ_MAGICNUM)) 
		{	
			pthread_mutex_unlock(&mq->mutex);
			mq->refcnt--;
			return SK_ERROR_TIMEOUT;
		}
	}
	memcpy (msg, mq->msgbuffer + mq->read * mq->mq_msgsize, mq->mq_msgsize);
	mq->read++;
	if (mq->read == mq->mq_maxmsg) mq->read = 0;
	isfull = ((mq->read + mq->mq_maxmsg - mq->write) % mq->mq_maxmsg == 1);
	if (!isfull) 
		pthread_cond_signal(&mq->write_cond);
	pthread_mutex_unlock(&mq->mutex);

	mq->refcnt--;
	return SK_SUCCESS;
	#endif
}

sk_status_code_t  sk_queue_destroy(const sk_queue_id_t queue_id)
{
	return sk_queue_destroy_ex(queue_id);
}
sk_status_code_t  sk_queue_destroy_ex(const sk_queue_id_t queue_id)
{
	MsgQ * mq = NULL;
	
	if(!IS_VALID_ID(queue_id))
	{
		SK_ERROR(("queue id is invalid."));
		return SK_ERROR_BAD_PARAMETER;
	}
	mq = (MsgQ*)queue_id;
	if((NULL == mq) ||(mq->magicnum != MQ_MAGICNUM))
	{
		return SK_ERROR_BAD_PARAMETER;
	}
	
	if (mq && mq->magicnum != 0) {
		mq->magicnum = 0;
		while (mq->refcnt != 0) {
			pthread_cond_broadcast(&mq->read_cond);
			pthread_cond_broadcast(&mq->write_cond);
		}

		pthread_cond_destroy(&mq->read_cond);
		pthread_cond_destroy(&mq->write_cond);
		pthread_mutex_destroy(&mq->mutex);

		/* free space */

		sk_mem_free(mq);
		mq = NULL;
	}
	return SK_SUCCESS;
}


sk_status_code_t  sk_queue_create_ex(sk_queue_id_t * const p_queue_id, const char * p_queue_name, const U32 max_msg_count, const U32 msg_size)
{
	MsgQ * mq = NULL;
	U32 max_quemsg_count=max_msg_count+1;
	if(p_queue_id == NULL)
	{
		SK_ERROR(("queue id is null."));
		return SK_ERROR_BAD_PARAMETER;
	}

	*p_queue_id = SK_INVALID_ID;
	mq =(MsgQ *) sk_mem_malloc(sizeof(MsgQ) + max_quemsg_count * msg_size);

	if (NULL == mq) 
		return SK_ERROR_NO_MEMORY;
	
	memset (mq, 0, sizeof(MsgQ) + max_quemsg_count * msg_size);

	/* initialize the remain member */
	mq->read = mq->write = mq->refcnt = 0;
	mq->magicnum = MQ_MAGICNUM;
	mq->mq_maxmsg =(max_quemsg_count);  //max_msg_count  ，有一个是用来判断队列是否满，区分队列空的情况
	mq->mq_msgsize = msg_size;
	pthread_mutex_init(&mq->mutex, NULL);
	pthread_cond_init(&mq->read_cond, NULL);
	pthread_cond_init(&mq->write_cond, NULL);

	*p_queue_id  = (sk_queue_id_t)mq;
	return SK_SUCCESS;
}

sk_status_code_t  sk_queue_send_ex(const sk_queue_id_t queue_id, void* msg, const U32 msg_size,const U32 timeout_ms)
{

	MsgQ * mq = NULL;	
	U8 empty;
	
	struct timespec ts;
	
	int ret = 0;

	
	if(!IS_VALID_ID(queue_id))
	{
		SK_ERROR(("queue id is invalid."));
		return SK_ERROR_BAD_PARAMETER;
	}
	
	
	mq = (MsgQ*)queue_id;
	if(msg == NULL || mq->magicnum != MQ_MAGICNUM || msg_size > mq->mq_msgsize )
	{
		SK_ERROR(("queue param is invalid."));
		return SK_ERROR_BAD_PARAMETER;
	}

	/* send message */
	pthread_mutex_lock(&mq->mutex);
	mq->refcnt++;

	if(timeout_ms != SK_TIMEOUT_INFINITY)
	{
		#if 0
		struct timeval tm;		
		if (0 != gettimeofday(&tm, NULL))
			SK_ERROR(("[message_receive_timeout] fatal error, time wrong !!!!!!!!!!\n"));

		
		//tm.tv_sec +=(u32TimeOutMs) / 1000;
		//tm.tv_nsec += (u32TimeOutMs) % 1000 * 1000000;	
		ts.tv_sec = tm.tv_sec +(u32TimeOutMs) / 1000;
		ts.tv_nsec = tm.tv_usec + (u32TimeOutMs) % 1000 * 1000000;
		#endif
		maketimespec(&ts,timeout_ms);

	}

	//if(mq->mq_maxmsg == 3)
	//printf("send the r %d w%d max %d mod %d \r\n", mq->read,mq->write, mq->mq_maxmsg,((mq->read + mq->mq_maxmsg - mq->write) % mq->mq_maxmsg));
	if ((mq->read + mq->mq_maxmsg - mq->write) % mq->mq_maxmsg == 1) 
	{        
		if(timeout_ms == SK_TIMEOUT_INFINITY)
		{
			ret = pthread_cond_wait(&mq->write_cond, &mq->mutex);  //old wait error.
		}
		else if(timeout_ms == SK_TIMEOUT_IMMEDIATE)
		{
			ret = ETIMEDOUT;
		}			
		else
		{
			ret = pthread_cond_timedwait(&mq->write_cond, &mq->mutex, &ts); //old wait error.
		}
		if ((ret == ETIMEDOUT) || (mq->magicnum != MQ_MAGICNUM)) 
		{	
			mq->refcnt--;
			pthread_mutex_unlock(&mq->mutex);
			
			SK_ERROR(("error to send msg %ld,magic num is %lx,ret is %d>>>>>\n", mq->refcnt,mq->magicnum,ret));
			return SK_ERROR_TIMEOUT;
		}				
	}
	
	memcpy (mq->msgbuffer + mq->write * mq->mq_msgsize, (void *)msg, /*mq->mq_msgsize*/msg_size);
	mq->write++;
	if (mq->write == mq->mq_maxmsg) 
		mq->write = 0;
	empty = (mq->write == mq->read);
	if (!empty) 
		pthread_cond_signal(&mq->read_cond);
	
	mq->refcnt--;
	pthread_mutex_unlock(&mq->mutex);

	return SK_SUCCESS;
}


sk_status_code_t  sk_queue_receive_ex(const sk_queue_id_t queue_id,const  void* msg, const U32 msg_size, const  U32 timeout_ms)
{
	MsgQ * mq = NULL;	
	//U8 empty;
	int ret = 0;
	U8 isfull;
	//struct timespec tm;
	struct timespec ts;
	
	if(!IS_VALID_ID(queue_id))
	{
		SK_ERROR(("queue id is invalid."));
		return SK_ERROR_BAD_PARAMETER;
	}
	mq = (MsgQ*)queue_id;
	
	if(msg == NULL || mq->magicnum != MQ_MAGICNUM || msg_size > mq->mq_msgsize )
	{
		SK_ERROR(("queue param is invalid."));
		return SK_ERROR_BAD_PARAMETER;
	}	

	/* internal malloc */
	pthread_mutex_lock(&mq->mutex);
	mq->refcnt++;	
	if(timeout_ms != SK_TIMEOUT_INFINITY)
	{
		#if 0
		struct timeval tm;
		if (0 != gettimeofday(&tm, NULL))
			SK_ERROR(("[message_receive_timeout] fatal error, time wrong !!!!!!!!!!\n"));

		
		//tm.tv_sec +=(u32TimeOutMs) / 1000;
		//tm.tv_nsec += (u32TimeOutMs) % 1000 * 1000000;	
		ts.tv_sec = tm.tv_sec +(u32TimeOutMs) / 1000;
		ts.tv_nsec = tm.tv_usec + (u32TimeOutMs) % 1000 * 1000000;
		#endif
		maketimespec(&ts,timeout_ms);
	}	
	
	if (mq->read == mq->write) 
	{
		if(timeout_ms == SK_TIMEOUT_INFINITY)
		{
			ret = pthread_cond_wait(&mq->read_cond, &mq->mutex);
		}
		else if(timeout_ms == SK_TIMEOUT_IMMEDIATE)
		{
			ret = ETIMEDOUT;
		}
		else 
		{
			ret = pthread_cond_timedwait(&mq->read_cond, &mq->mutex, &ts);
		}
		if ((ret == ETIMEDOUT) || (mq->magicnum != MQ_MAGICNUM)) 
		{	
			mq->refcnt--;
			pthread_mutex_unlock(&mq->mutex);
			return SK_ERROR_TIMEOUT;
		}
	}
	memcpy ((void *)msg, mq->msgbuffer + mq->read * mq->mq_msgsize, /*mq->mq_msgsize*/msg_size);
	mq->read++;
	if (mq->read == mq->mq_maxmsg) mq->read = 0;
	isfull = ((mq->read + mq->mq_maxmsg - mq->write) % mq->mq_maxmsg == 1);
	if (!isfull) 
		pthread_cond_signal(&mq->write_cond);
	
	mq->refcnt--;
	pthread_mutex_unlock(&mq->mutex);

	return SK_SUCCESS;
}


