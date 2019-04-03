/*
 *	SMGLOBAL.C				ver 1.0
 *
 * (c)	Copyright  Federal Technologies Limited	2001 - 2002
 *
 *    	Source file name	: smglobal.c
 *
 *    	AUTHOR (S) NAME	: KBA, Jan.2001
 *
 *    	Original Work		:
 *						SmallWin Global variables.
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
#include "smtype.h"
#include "smglobal.h"
#include "smsetup.h"
#include "smallwin.h"




static sk_sem_id_t	sem_system;
static sk_sem_id_t	sem_event;


void init_global(void)
{
	sk_sem_create(&sem_system,"system", 1);
	sk_sem_create(&sem_event, "event",0);
}

void wait_event(void)
{
	sk_sem_lock(sem_event);
}

void signal_event(void)
{
	sk_sem_unlock(sem_event);
}

void lock_system(void)
{
	sk_sem_lock(sem_system);
}

void unlock_system(void)
{
	sk_sem_unlock(sem_system);
}


