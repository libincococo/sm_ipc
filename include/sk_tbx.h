/*!
*@file sk_tbx.h	
*@brief Public Header of toolbox API
* Version:	 0.0.1
* Description:  
* Copyright (c) 2009, SkyworthDTV
* All rights reserved.				
*@author skyworth 
*@date 2009-04-09 
*/

#ifndef SK_TBX_H
#define SK_TBX_H

#include <stdio.h> /*for printf & vsprintf*/

#include "sk_std_defs.h"

#ifdef __cplusplus
extern "C" {
#endif



#ifdef SK_ANDROID

#ifndef LOG_TAG
#define LOG_TAG "SKAPI"
#endif

#include <utils/Log.h>

#ifndef SKDEBUG
#define SK_ASSERT(ignore) 	((void)0)
#define SK_DEBUG(ignore)		((void)0)
#define SK_DEBUG_FILE_LINE()	((void)0)
#else
#define SK_ASSERT(e) LOG_ASSERT(e,"### Bad condition###")
#define SK_DEBUG(x) LOGD x;LOGD("[%s %d]",__FUNCTION__, __LINE__)
#define SK_DEBUG_FILE_LINE()	LOGD("[%s %d]------",__FUNCTION__, __LINE__)
#endif

#define SK_INFO(x) LOGI x;LOGI("[%s %d]",__FUNCTION__, __LINE__)
#define SK_ERROR(x) LOGE x;LOGE("[%s %d]",__FUNCTION__, __LINE__)
#define sk_tbx_print LOGD

#define SK_TRACE sk_tbx_print
#define sk_tbx_print_level_and_time(n) LOGD("DEBUG %d RUN TIME:", (n)) 
#ifndef LOGV
#define LOGV SLOGV
#endif
#ifndef LOGD
#define LOGD SLOGD
#endif
#ifndef LOGI
#define LOGI SLOGI
#endif
#ifndef LOGW
#define LOGW SLOGW
#endif
#ifndef LOGE
#define LOGE SLOGE
#endif
#ifndef LOGE_IF
#define LOGE_IF SLOGE_IF
#endif
#ifndef LOG_ASSERT
#define LOG_ASSERT ALOG_ASSERT
#endif
#else

#ifdef SK_OS_LINUX
#define SK_TRACE printf
#else
#define SK_TRACE sk_tbx_print
#endif

/*!
* Defines the levels of the report 
*/
typedef enum sk_report_level_e
{                     
	SK_REPORT_LEVEL_ERROR,       /*!<�κΰ汾����ӡ*/
	SK_REPORT_LEVEL_INFO,        	 /*!<��Ҫ����Ϣ,�κΰ汾����ӡ*/
	SK_REPORT_LEVEL_DBG,    	 /*!<������Ϣ,ֻ��DEBUG�汾��ӡ*/
	SK_NB_OF_REPORT_LEVEL      
} sk_report_level_t;

/*!
*@brief Defines the supported output devices 
* ע��:tbxʵ��ʱҪ��������Ҫ��ʼ��,Ĭ��������̨��ӡ,
* ������ӡ���������Ҫ���ģ���ʼ��֮��
*/
typedef enum sk_report_device_e
{
    SK_REPORT_DEVICE_NULL   =  0,   
    SK_REPORT_DEVICE_CONSOLE    =  1,  
    SK_REPORT_DEVICE_UART0   =  2,  
    SK_REPORT_DEVICE_UART1  =  4,  
    SK_REPORT_DEVICE_NET  =  8, 
    SK_REPORT_DEVICE_USB  =  16, 
    SK_REPORT_DEVICE_FILE =  32
} sk_report_device_t;


#if 0
	#define SK_INFO(msg) ((void)0)
	#define SK_ERROR(err_info) ((void)0)
#else
#define SK_INFO(msg)  \
	do{ \
		sk_tbx_print_level_and_time(SK_REPORT_LEVEL_INFO); \
		SK_TRACE msg; \
		SK_TRACE("[%s %d] \n",__FUNCTION__,__LINE__);\
	}while(0)
	
#define SK_ERROR(err_info)  \
	do{ \
		SK_TRACE("\033[1;40;31m ");\
		sk_tbx_print_level_and_time(SK_REPORT_LEVEL_ERROR); \
		SK_TRACE err_info; \
		SK_TRACE("[%s %d] \033[0m \n",__FUNCTION__,__LINE__);\
	}while(0)
#endif	
	
#ifndef SKDEBUG
#define SK_ASSERT(ignore) 	((void)0)
#define SK_DEBUG(ignore)		((void)0)
#define SK_DEBUG_FILE_LINE()	((void)0)
#else
#define SK_ASSERT(e) \
	do{ \
		((e) ? (void)0 : (void)(SK_TRACE("\033[1;40;31m [SK_ASSERT]assertion failed: %s ,file %s, line %d \033[0m \r\n", #e, __FUNCTION__, __LINE__))); \
	}while(0)

#define SK_DEBUG(dbg_info) \
	do{ \
		sk_tbx_print_level_and_time(SK_REPORT_LEVEL_DBG); \
		SK_TRACE dbg_info; \
		SK_TRACE("[%s %d] \n",__FUNCTION__,__LINE__);\
	}while(0)
	
#define SK_DEBUG_FILE_LINE()	\
	do{ \
		sk_tbx_print_level_and_time(SK_REPORT_LEVEL_DBG); \
		SK_TRACE("[%s %d] \n",__FUNCTION__,__LINE__);  \
	}while(0)
#endif


/*!
*@brief ��ȡ�汾��Ϣ
*/
sk_revision_t sk_tbx_get_revision(void);

/*!
* sk_print:  ƽ̨�����Ĵ�ӡ����
* ע��:tbxʵ��ʱҪ��������Ҫ��ʼ��,Ĭ��������̨��ӡ,
* ������ӡ���������Ҫ���ģ���ʼ��֮��
*@param Format_p �ɱ����
*@return sk_status_code_t
*/
sk_status_code_t sk_tbx_print(const char *const p_format, ...);

/*!
*@brief  ��ӡ��ӡ�����ϵͳʱ��
*������Ҫת�����ַ�����SK_REPORT_LEVEL_ERROR:e; SK_REPORT_LEVEL_INFO:i;SK_REPORT_LEVEL_DBG:d
*ʱ��Ϊ���������ڵ�ʱ��,��ȷ������,��ʽ��:%02u:%02u:%02u.%03u
*�������������:
* e>00:10:00:000]����
* i>00:10:00:000]����
* d>00:10:00:000]
*@param leve ��ӡ����
*@return sk_status_code_t
*/
sk_status_code_t sk_tbx_print_level_and_time(const sk_report_level_t level);


/*!
*@brief  ������������豸
* Ĭ����������̨�����
* device ֧�ֻ����
* ע��:���Ҫ������uart,usb & net ���,�������ǳ�ʼ��֮����������
* ͬʱ�������ģ��Ҳ���뱣֤������
*@param device ��ӡ����豸
*@return sk_status_code_t
*/
sk_status_code_t sk_tbx_set_device(const sk_report_device_t device);
#endif

#ifdef __cplusplus
}
#endif

#endif /*SK_TBX_H*/

/*eof*/





