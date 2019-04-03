/*****************************************************************************/
/*                   CONEXANT PROPRIETARY AND CONFIDENTIAL                   */
/*                       SOFTWARE FILE/MODULE HEADER                         */
/*                      Conexant Systems Inc. (c) 2005                       */
/*                            Shanghai, China                                */
/*                         All Rights Reserved                               */
/*****************************************************************************/
/*
* Filename:    SKYAPI_OS.c
*
*
* Description: This file contains function implementation of OS abstract layer API for Skyworth .
*
* Author:      Yi Liu
*
*****************************************************************************/

/*****************/
/* Include Files */
/*****************/
#include "sk_os.h"
#include "sk_std_defs.h"
#include "sk_tbx.h"

static U8 os_init_flage = 0;

#define SK_OS_REVISION "SK_OS_V00.00.00-"__DATE__

extern void init_os_thread_info();

sk_revision_t  sk_os_get_revision(void)
{
	return SK_OS_REVISION;
}

sk_status_code_t  sk_os_init(void)
{
	if(os_init_flage != 0)
		return SK_SUCCESS;

	init_os_thread_info();	
	os_init_flage = 1;
	return SK_SUCCESS;
}

sk_status_code_t  sk_os_start()
{
	return SK_SUCCESS;
}

sk_status_code_t  sk_os_stop()
{
	return SK_SUCCESS;
}

sk_status_code_t  sk_os_term(void)
{
	return SK_SUCCESS;
}


