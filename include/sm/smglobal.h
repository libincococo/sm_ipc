/*
 *	 SMGLOBAL.H				ver 1.0
 *
 * (c)	Copyright  Federal Technologies Limited	2001 - 2002
 *
 *    	Source file name	: smglobal.h
 *
 *    	AUTHOR (S) NAME	: MDR Dec. 2000
 *
 *    	Original Work		:
 *				SmallWin Global Variables.
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

#ifndef SMGLOBAL_H
#define SMGLOBAL_H

#include "smtype.h"
#include "smgrp.h"

typedef struct {
	u8		frame_style,
			title_style,
			posbox_style,
			look3d,
			high_style,
			symbol_use,
			default_font,
			frame_width;
SM_COLOR_REF		transparency,
			smallwin_fcol,
			smallwin_bcol,
			title_bcol,
			status_fcol,
			status_bcol,
			frame_fcol,
			frame_bcol,
			frame_main_col,
			child_fcol,
			child_bcol,
			child_highf,
			child_highb,
			menu_fcol,
			menu_bcol,
			menu_highf,
			menu_highb,
			message_fcol,
			message_bcol,
			message_child_fcol,
			message_child_bcol,
			message_child_highf,
			message_child_highb,
			message_child_high_style,
			message_title_bcol;			
      u8		octet_box_main_size,
			octet_box_child_size,
			popup_hmargin,
			popup_vmargin,
			popup_separator_width;
	s8		popup_interval,
			transparency_rate;
	RGB		palette[32];
} SM_GLOB;

#define HS_INVERT			0
#define HS_FIXED			2
#define HS_3D				4
#define HS_RESERVED			6
#define HS_UNDERLINE		1

#define PAINT_ALL			0
#define PAINT_SBAR			1
#define PAINT_TSUBBAR		2

extern SM_GLOB *get_global(void);
extern void init_global(void);
extern void set_running_smallwin(HSM hsm);
extern HSM get_running_smallwin(void);
extern void wait_event(void);
extern void init_event(void);
extern void signal_event(void);
extern void lock_system(void);
extern void unlock_system(void);
extern void lock_keyin(void);
extern void unlock_keyin(void);
int query_key_lock(void);

extern HSM sm_get_input_smallwin(void);
extern HSM sm_get_smallwin_list(void);
extern HSM sm_get_popup_list(void);

extern s32 sm_is_smallwin(void *smallwin);
extern HSM sm_create_smallwin(u16 sm_type, u16 sm_id, HSM hsm, u16 sm_style,
	SM_RECT *sm_rect, u8 sm_font, char *sm_text, SM_PROC sm_proc, u32 param1, u32 param2);
extern void sm_process_message(HSM hsuspend_smallwin);
extern void sm_express_message(HSM hsm, u32 message, u32 param1, u32 param2);
extern u32 sm_do_modal(HSM hsuspend_smallwin, HSM hdlg);

extern s32 sm_create_tick(HSM hsm, u32 n100msec);
extern s32 sm_create_systick(HSM hsm, u32 n100msec);
extern void sm_destroy_tick(s32 htick);
extern u32 sm_child_process(HSM hsm, u32 message, u32 param1, u32 param2);
extern void run_smallwin(void);

#define cpstrt(dest,src) Memcpy(dest,src,sizeof(dest)) 

#endif

