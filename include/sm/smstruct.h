/*
 *	 SMSTRUCT.H				ver 1.0
 *
 * (c)	Copyright  Federal Technologies Limited	2001 - 2002
 *
 *    	Source file name	: smstruct.h
 *
 *    	AUTHOR (S) NAME	: MDR Nov.2000
 *
 *    	Original Work		:
 *						  SmallWin Structure Definitions
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

#ifndef SMSTRUCT_H
#define SMSTRUCT_H

#include "smtype.h"

#define MAX_SM_TEXT_LEN 32

#define COMMON_SMALLWIN_ELEMENTS								\
	struct tag_common_smallwin		*prev;						\
	struct tag_common_smallwin		*next;						\
	u16							sm_type;						\
	u16							sm_id;							\
	char						sm_text[MAX_SM_TEXT_LEN];		\
	SM_PROC						sm_proc;						\

typedef struct tag_common_smallwin {
	COMMON_SMALLWIN_ELEMENTS
} COMMON_SM;

#endif

