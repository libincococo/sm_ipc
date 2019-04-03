/*
 *	SMTYPE.H				ver 1.0
 *
 * (c)	Copyright  Federal Technologies Limited	2001 - 2002
 *
 *    	Source file name	: smtype.h
 *
 *    	AUTHOR (S) NAME	: MDR Jan. 2001
 *
 *    	Original Work		:
 *					Small window type definitions
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
//#include "swboxcfg.h"
#ifndef SMTYPE_H
#define SMTYPE_H

typedef unsigned char			u8;
typedef signed char				s8;
typedef unsigned short int		u16;
typedef signed short int		s16;
typedef unsigned int			u32;
typedef signed int				s32;

typedef u32						HSM;
typedef u32						(*SM_PROC)(HSM, u32, u32, u32);

#endif

