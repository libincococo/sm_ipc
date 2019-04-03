/*!
*@file sk_tbx.c
*@brief dummy
* Version:	 0.0.1
* Description:  
* Copyright (c) 2009, SkyworthDTV
* All rights reserved.				
*@author skyworth 
*@date 2009-04-16
*/

#include <stdarg.h> /*for va_list*/
#include <string.h> /*for memset*/
#include <stdio.h> /*for printf & vsprintf*/

#include "sk_os.h"
#include "sk_tbx.h"

#ifndef SK_ANDROID
#define TBX_PRINT_BUF_LEN (4*1024)
#define SK_HDI_TBX_REVISION "SK_HDI_TBX_V00.00.00-"__DATE__

static char * arr_level_msg[SK_NB_OF_REPORT_LEVEL] = {"E", "I", "D"};
//static sk_report_device_t s_tbx_report_device = SK_REPORT_DEVICE_CONSOLE;

/*没有保护,可能打印结果不对*/
static char s_print_buf[TBX_PRINT_BUF_LEN+1];

//#define CLOSE_URAT

sk_revision_t sk_tbx_get_revision(void)
{
	return SK_HDI_TBX_REVISION;
}

sk_status_code_t sk_tbx_print(const char *const p_format, ...)
{
	int ret;
	va_list args;
#ifdef CLOSE_URAT 
        return SK_SUCCESS;
#endif
	if(p_format != NULL)
	{
		memset(s_print_buf, 0x00, sizeof(s_print_buf));
		va_start(args, p_format);		
		ret = vsprintf(s_print_buf, p_format, args);
		va_end(args);

		if (ret <= TBX_PRINT_BUF_LEN)
		{
			printf("%s", s_print_buf);
			return SK_SUCCESS;
		}
		printf("e>Formatted string too long\n");			
	}

	return SK_ERROR_BAD_PARAMETER;
}

sk_status_code_t sk_tbx_set_device(const sk_report_device_t device)
{
	return SK_ERROR_FEATURE_NOT_SUPPORTED;
}


sk_status_code_t sk_tbx_print_level_and_time(const sk_report_level_t level)
{
	U32 ms = 0;
	int hours = 0, minutes = 0, seconds = 0;
 	int milliseconds = 0;
	
#ifdef CLOSE_URAT 
        return SK_SUCCESS;
#endif
	if((level>=SK_REPORT_LEVEL_ERROR) && (level<SK_NB_OF_REPORT_LEVEL))
	{
		sk_clock_get_sys_time(&ms);
		hours = (ms/1000)/3600;
		minutes = ((ms/1000/60))%60;
		seconds = ((ms/1000))%60;
		milliseconds = ms%1000;
		
		printf("[{%s} %02u:%02u:%02u.%03u]: ",arr_level_msg[level],hours,minutes,seconds,milliseconds);
	}
	
	return SK_SUCCESS;
}
#endif

