/* @(#)Xfeoak.h	2.2 91/07/05 17:02:41 FUJITSU LIMITED. */
/******************************************************************

              Copyright 1991, by FUJITSU LIMITED

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of FUJITSU LIMITED
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
FUJITSU LIMITED makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

FUJITSU LIMITED DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Takashi Fujiwara     FUJITSU LIMITED 
                               fujiwara@a80.tech.yk.fujitsu.co.jp
          Kiyoaki oya          FUJITSU LIMITED

******************************************************************/

/*  Xfeoak : Input Server (kana-kanji covertion front end)	*/
/*         : Version 2.0					*/

#ifndef _XFEOAK_H_
#define _XFEOAK_H_

#include <X11/Xlib.h>
#ifdef XV11R4
/*
 * The following is specification defines a set of rutines
 *
 *	X Window System, Version 11
 *	 Input Method Specifications
 *	Post-Public Review Draft 1.1 - propsed
 */
typedef unsigned long XIMStyle;

#define XIMPreeditArea          0x0001L
#define XIMPreeditCallbacks     0x0002L
#define XIMPreeditPosition      0x0004L
#define XIMPreeditNothing       0x0008L
#define XIMPreeditNone          0x0010L

#define XIMStatusArea           0x0100L
#define XIMStatusCallbacks      0x0200L
#define XIMStatusNothing        0x0400L
#define XIMStatusNone           0x0800L

typedef struct {
    unsigned short count_styles;
    XIMStyle * supported_styles;
    } XIMStyles;

#endif /* XV11R4 */

#include "XIMProto.h"

#define	FPRT_XIM		0x01
#define	FPRT_XJP		0x02

#define	FIMP_DESTROY		0x00
#define	FIMP_CREATE		0x01
#define	FIMP_BEGIN		0x02
#define	FIMP_SETFOCUS		0x04

#define FS_HIRAGANA		0x01
#define FS_KATAKANA		0x02
#define FS_KANA			0x03
#define FS_UPPER_ASCII		0x04
#define FS_LOWER_ASCII		0x08
#define FS_ASCII		0x0C

#define FXIMP_LINEBUFF_LENGTH   42
#define FXIMP_MAX_CLIENT_NO  	32 

#define FINF_NONE		0	
#define FINF_ADD		1
#define FINF_MODY		2

#define FKEY_NONE		0
#define FKEY_G_OYA		1
#define FKEY_G_JIS		2

#define FF_FONT_LOAD		0x01
#define FF_CREATE_GC		0x10

#define FC_SAME_ROOT_PREEDIT_WINDOW	0x01
#define FC_SAME_ROOT_PREEDIT_GC		0x02
#define FC_SAME_ROOT_PREEDIT_FONT	0x04
#define FC_SAME_ROOT_PREEDIT		0x07

#define FC_SAME_ROOT_STATUS_WINDOW	0x10
#define FC_SAME_ROOT_STATUS_GC		0x20
#define FC_SAME_ROOT_STATUS_FONT	0x40
#define FC_SAME_ROOT_STATUS		0x70

typedef struct  _FXimpFont {
	GC		ascii_kana_gc;
	GC		kanji_gc;
	GC		gaiji_gc;
	GC		ascii_kana_reverse_gc;
	GC		kanji_reverse_gc;
	GC		gaiji_reverse_gc;
	GC		invert_gc;
	XFontStruct 	*ascii_kana_font_struct;
	XFontStruct 	*kanji_font_struct;
	XFontStruct 	*gaiji_font_struct;
	short		ascii_kana_font_width;
	short		kanji_font_width;
	short		max_font_height;
	short		max_font_descent;
	short		max_font_ascent;
	unsigned char	fixe_width;
	unsigned char	status;
	} FXimpFont;

typedef struct  _FXimp_Client {
	Window  	window;        
	XIMStyle 	style;
	unsigned char	prot_type;
	unsigned char   imp_mode;    
	unsigned char   ext_ui_back_front;    
	unsigned char   frontend_key_press_mask;    
	unsigned char   ext_ui_statuswindow;
	unsigned char   rsv2;    /* rserved */
	unsigned char   window_type;
	unsigned char   check_preedit_area_size;    
	Ximp_PreeditPropRec 	*preedit;
	Ximp_StatusPropRec 	*status;
	char   		*preedit_fontname;
	char   		*status_fontname;
	FXimpFont   	*preedit_font;
	FXimpFont   	*status_font;
	Atom		ximp_readprop_atom;	
	Window		focus_window;
	Window  	preedit_window;
	Window  	preedit_window2;
	Window  	status_window; 
	XRectangle 	preedit_area[2];
	short		used_preedit_window_no;
	short		preedit_line_length[2];
	short		preedit_line_no;
	unsigned char	preedit_visible;
	unsigned char	status_visible;
	unsigned char	henkan_mode;
	unsigned char	siftmode;
	unsigned char	romaji_mode;
	unsigned char	hankakumode;	
	unsigned char	physical_kanashift;	
	unsigned char	logical_kanashift;
	unsigned char	logical_capslock;
	unsigned char	stage;		/* reserved */
	unsigned long	old_state;
	unsigned char	select_kanji_no;	
	unsigned char	select_kanji_mod;
	short		last_colums;	
	unsigned short  *output_buff;
	unsigned short  *input_buff;
	short		romaji_colums;
	short		current_colums;
	short		reverse_colums;
	short		input_colums;
	short		convert_colums;
	short		convert_count;
	short		input_flag;
	unsigned char	slcrtok_nhtype;
	unsigned char	slcrtok_input_no;
	unsigned char	slcrtok_inbuf[4];
	unsigned char	slcrtok_outbuf[4];
	} FXimp_Client;

typedef struct _FXimp_Client *FXimp_ICID;

extern char     	*command_name;
extern Display		*display;
extern int      	screen;
extern Window   	root;
extern FXimp_Client 	*focus_in_client;	
extern FXimp_Client 	*current_client;
extern FXimp_Client 	*root_client;
extern FXimpFont	*current_preedit_font;
extern short		keybord_type;
extern short		no_beep;
extern short		control_modifiers_grap;
extern char		control_modifiers_flag;	

int 	   	InitialClientTable();	
int	   	ExitClientTable();	
FXimp_Client 	*NewClient();	
int 	   	DeleteClient();
FXimp_Client 	*FindClient();
FXimp_Client 	*FindClientByICID();
FXimp_Client 	*FindClientByFocusWindow();
FXimp_Client 	*FindClientByPreeditOrStatusWindow();
unsigned short	Hankaku_To_ZenkakuAsciiHiragana();	
void		ErrorMessage();
void		WarningMessage();

#define KS_TOP          1
#define KS_BOTTOM       2
#define KS_FOWARD       3
#define KS_BACKWARD     4
#define KS_NEXT         5
#define KS_PREV         6

#ifdef DEBUG
#define Static /**/
#else
#define Static static
#endif

#endif _XFEOAK_H_
