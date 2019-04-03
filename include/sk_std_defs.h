/*!
*@file sk_std_defs.h	
*@brief sk_std_defs
* Version:	 0.0.1
* Description:  
* Copyright (c) 2009, SkyworthDTV
* All rights reserved.				
*@author skyworth 
*@date 2009-04-09 
*/

#ifndef SK_STD_DEFS_H
#define SK_STD_DEFS_H

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/*! Common unsigned types */
#ifndef SK_DEFINED_U8
#define SK_DEFINED_U8
typedef unsigned char  U8;
#endif

#ifndef SK_DEFINED_U16
#define SK_DEFINED_U16
typedef unsigned short U16;
#endif

#ifndef SK_DEFINED_U32
#define SK_DEFINED_U32
typedef unsigned long   U32;
#endif

#ifndef SK_DEFINED_U64
#define SK_DEFINED_U64
typedef unsigned long long U64;
#endif

/*! Common signed types */
#ifndef SK_DEFINED_S8
#define SK_DEFINED_S8
typedef signed char  S8;
#endif

#ifndef SK_DEFINED_S16
#define SK_DEFINED_S16
typedef signed short S16;
#endif

#ifndef SK_DEFINED_S32
#define SK_DEFINED_S32
typedef signed long   S32;
#endif

#ifndef SK_DEFINED_S64
#define SK_DEFINED_S64
typedef signed long long S64;
#endif

#ifndef		NULL
#define	NULL		((void *)0)
#endif


#ifndef SK_TRUE 
#define SK_TRUE 1
#endif

#ifndef SK_FALSE
#define SK_FALSE 	0
#endif

/*! ʱ�䶨�� */
typedef U32  sk_clock_t;

/*! Revision structure */
typedef const char * sk_revision_t;

/*! id */
typedef unsigned long sk_id_t;

/*! handle */
typedef unsigned long sk_handle_t;

#define SK_INVALID_ID			((U32)(-1))
#define SK_INVALID_HANDLE		((U32)(-1))

/*! error code  */
typedef enum sk_status_code_e
{
	SK_SUCCESS= 0,
	SK_ERROR_BAD_PARAMETER,          					 /* Bad parameter passed       */
	SK_ERROR_NO_MEMORY,               						  /* Memory allocation failed   */
	SK_ERROR_UNKNOWN_DEVICE,          					  /* Unknown device name        */
	SK_ERROR_ALREADY_INITIALIZED,       				  /* Device already initialized */
	SK_ERROR_NO_FREE_HANDLES,          					 /* Cannot open device again   */
	SK_ERROR_INVALID_HANDLE,           					 /* Handle is not valid        */
	SK_ERROR_INVALID_ID,           						 /* ID is not valid        */
	SK_ERROR_FEATURE_NOT_SUPPORTED,     				/* Feature unavailable        */
	SK_ERROR_INTERRUPT_INSTALL,         					/* Interrupt install failed   */
	SK_ERROR_INTERRUPT_UNINSTALL,       				/* Interrupt uninstall failed */
	SK_ERROR_TIMEOUT,                  						 /* Timeout occured            */
	SK_ERROR_DEVICE_BUSY,               					 /* Device is currently busy   */
	SK_ERROR_NO_INIT,									 /* not init    */
	SK_ERROR_MAX_COUNT,								 /* that more than max count    */
	SK_ERROR_INSUFFICIENT_BUFFER,                    /* insufficient buffer    */
	SK_FAILED,              							 		/* UNKNOWN ERROR   */
	SK_STATUS_ENABLE,
	SK_STATUS_DISABLE
} sk_status_code_t;

/*! week days  */
typedef enum sk_week_days_s
{
	SK_MONDAY = 1,
	SK_TUESDAY,
	SK_WEDNESDAY,
	SK_THURSDAY,
	SK_FRIDAY,
	SK_SATURDAY,
	SK_SUNDAY 
}sk_week_days_t;

/*! time  */
typedef struct sk_time_s
{
   U16 			m_year; 					/*!< Decimal value*/
   U8 			m_month; 					/*!< [1..12]*/
   U8 			m_day; 						/*!< [1..31]*/
   U8 			m_hour;						/*!< [0..23]*/
   U8 			m_minute; 					/*!< [0..59]*/
   U8 			m_seconds; 					/*!< [0..59]*/
   sk_week_days_t m_day_of_the_week; 		/*!@see sk_week_days_t*/
} sk_time_t;

typedef struct 
{
	U32		mjd;
	U16		year;
	U8		month;
	U8		day;
	U8		weekday;
	U8		hour;
	U8		min;
	U8		sec;
}date_time;

/*! MULTI LANGUAGE  */
// https://en.wikipedia.org/wiki/Lists_of_ISO_639_codes
typedef enum sk_lang_type_s
{
	SK_LANG_CHI,  	/*!<"China"*/
	SK_LANG_ENG,		/*!<"English"*/
	SK_LANG_FIN,		/*!<"Finnish"*/
	SK_LANG_FRA,	/*!<"French"*/
	SK_LANG_DEU, 	/*!<"German"*/
	SK_LANG_ELL,	/*!<"Greek"*/
	SK_LANG_HUN, 		/*!<"Hungarian"*/
	SK_LANG_ITA,		/*!<"Italian"*/
	SK_LANG_NOR, 		/*!<"Norwegian"*/
	SK_LANG_POL	,	/*!<"Polish"*/
	SK_LANG_POR,		 /*!<"Portuguese"*/
	SK_LANG_RON, 	/*!<"Romanian"*/
	SK_LANG_RUS, 		/*!<"Russian"*/
	SK_LANG_SLV	, 	/*!<"Slovenian"*/
	SK_LANG_SPA, 	/*!<"Spanish"*/
	SK_LANG_SWE,	 /*!<"Swedish"*/
	SK_LANG_TUR,		 /*!<"Turkish"*/
	SK_LANG_ARA, 		/*!<"Arabic"*/
	SK_LANG_ZHO,	/*!<"Chinese"*/
	SK_LANG_CES, 	/*!<"Czech"*/
	SK_LANG_DAN, 		/*!<"Danish"*/
	SK_LANG_NLD,	/*!<"Dutch"*/
	SK_LANG_SLK,       /*!<"Slovak"*/ 
	SK_LANG_EST,	   /*!<"Estonian "*/
	SK_LANG_IRA,       /*!<"Iranian "*/ 
	SK_LANG_WEN,        /*!<"Sorbian "*/
	SK_LANG_SCO,        /*!<"Scots "*/
	SK_LANG_FAS,        /*!<"Persian "*/
	SK_LANG_NNO,	    /*!<"Norwegian Nynorsk "*/
	SK_LANG_KUR,        /*!<"Kurdish "*/
	SK_LANG_ISL,        /*!<"Icelandic "*/
	SK_LANG_GLE,        /*!<"Irish "*/
	SK_LANG_HEB,        /*!<"Hebrew "*/
	SK_LANG_QAA,		/*!<"Private"*/ 
	SK_LANG_MUL,		/*!<"Multiple languages"*/ 
	SK_LANG_BUL,		/*!<"Bulgarian"*/
	SK_LANG_PRIVATE,	/*!<"Private"*/ 	
	SK_LANG_IND,        /*!<"Indonesian "*/
	SK_LANG_MIS,	    /*!<"Uncoded languages"*/	
	SK_LANG_VIE,		/*!<"Vietnamese"*/
    SK_LANG_THA,        /*!<"Thai "*/
	SK_LANG_HRV,        /*!<"Croatian "*/
	SK_LANG_BRE,        /*!<"Breton "*/
	SK_LANG_CAT,        /*!<"Catalan; Valencian "*/
	SK_LANG_LIT,        /*!<"Lithuanian "*/
	SK_LANG_MKD,        /*!<"Macedonian "*/
	SK_LANG_UKR,        /*!<"Ukrainian "*/		       
    SK_LANG_BOS,        /*!<"Bosnian "*/

	SK_NB_OF_LANG
}sk_lang_type_t;



typedef enum SKY_TIME_ZONE
{
	SKY_ZONE_AUTO = 0,
	SKY_ZONE_GMT_E_12_0,
	SKY_ZONE_GMT_E_11_5,
	SKY_ZONE_GMT_E_11_0,
	SKY_ZONE_GMT_E_10_5,
	SKY_ZONE_GMT_E_10_0,
	SKY_ZONE_GMT_E_9_5,
	SKY_ZONE_GMT_E_9_0,
	SKY_ZONE_GMT_E_8_5,
	SKY_ZONE_GMT_E_8_0,
	SKY_ZONE_GMT_E_7_5,
	SKY_ZONE_GMT_E_7_0,
	SKY_ZONE_GMT_E_6_5,
	SKY_ZONE_GMT_E_6_0,
	SKY_ZONE_GMT_E_5_5,
	SKY_ZONE_GMT_E_5_0,
	SKY_ZONE_GMT_E_4_5,
	SKY_ZONE_GMT_E_4_0,
	SKY_ZONE_GMT_E_3_5,
	SKY_ZONE_GMT_E_3_0,
	SKY_ZONE_GMT_E_2_5,
	SKY_ZONE_GMT_E_2_0,
	SKY_ZONE_GMT_E_1_5,
	SKY_ZONE_GMT_E_1_0,
	SKY_ZONE_GMT_E_0_5,	
	SKY_ZONE_GMT_0,
	SKY_ZONE_GMT_W_0_5,	
	SKY_ZONE_GMT_W_1_0,
	SKY_ZONE_GMT_W_1_5,
	SKY_ZONE_GMT_W_2_0,
	SKY_ZONE_GMT_W_2_5,
	SKY_ZONE_GMT_W_3_0,
	SKY_ZONE_GMT_W_3_5,
	SKY_ZONE_GMT_W_4_0,
	SKY_ZONE_GMT_W_4_5,
	SKY_ZONE_GMT_W_5_0,
	SKY_ZONE_GMT_W_5_5,
	SKY_ZONE_GMT_W_6_0,
	SKY_ZONE_GMT_W_6_5,
	SKY_ZONE_GMT_W_7_0,
	SKY_ZONE_GMT_W_7_5,
	SKY_ZONE_GMT_W_8_0,
	SKY_ZONE_GMT_W_8_5,
	SKY_ZONE_GMT_W_9_0,
	SKY_ZONE_GMT_W_9_5,
	SKY_ZONE_GMT_W_10_0,
	SKY_ZONE_GMT_W_10_5,
	SKY_ZONE_GMT_W_11_0,
	SKY_ZONE_GMT_W_11_5,
	SKY_ZONE_GMT_W_12_0,
	SKY_ZONE_GMT_NUMBER,
}SKY_TIME_ZONE;

#ifdef __cplusplus
}
#endif

#endif /*SK_STD_DEFS_H*/

/*eof*/





