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
	SK_REPORT_LEVEL_ERROR,       /*!<任何版本都打印*/
	SK_REPORT_LEVEL_INFO,        	 /*!<重要的信息,任何版本都打印*/
	SK_REPORT_LEVEL_DBG,    	 /*!<调试信息,只在DEBUG版本打印*/
	SK_NB_OF_REPORT_LEVEL      
} sk_report_level_t;

/*!
*@brief Defines the supported output devices 
* 注意:tbx实现时要做到不需要初始化,默认往控制台打印,
* 其他打印输出可能需要相关模块初始化之后
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
*@brief 获取版本信息
*/
sk_revision_t sk_tbx_get_revision(void);

/*!
* sk_print:  平台基本的打印函数
* 注意:tbx实现时要做到不需要初始化,默认往控制台打印,
* 其他打印输出可能需要相关模块初始化之后
*@param Format_p 可变参数
*@return sk_status_code_t
*/
sk_status_code_t sk_tbx_print(const char *const p_format, ...);

/*!
*@brief  打印打印级别和系统时间
*级别需要转换撑字符串如SK_REPORT_LEVEL_ERROR:e; SK_REPORT_LEVEL_INFO:i;SK_REPORT_LEVEL_DBG:d
*时间为开机到现在的时间,精确到毫秒,格式是:%02u:%02u:%02u.%03u
*最后的输出类似于:
* e>00:10:00:000]或者
* i>00:10:00:000]或者
* d>00:10:00:000]
*@param leve 打印级别
*@return sk_status_code_t
*/
sk_status_code_t sk_tbx_print_level_and_time(const sk_report_level_t level);


/*!
*@brief  设置输入输出设备
* 默认是往控制台输出的
* device 支持或操作
* 注意:如果要配置往uart,usb & net 输出,必须他们初始化之后才能有输出
* 同时他们相关模块也必须保证不出错
*@param device 打印输出设备
*@return sk_status_code_t
*/
sk_status_code_t sk_tbx_set_device(const sk_report_device_t device);
#endif

#ifdef __cplusplus
}
#endif

#endif /*SK_TBX_H*/

/*eof*/





