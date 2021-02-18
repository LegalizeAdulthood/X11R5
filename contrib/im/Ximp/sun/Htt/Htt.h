/*
 *        %Z%%M% %I% %E%  
 * 
 * Delived from @(#)Xfeoak.h	2.1 91/08/13 11:28:54 FUJITSU LIMITED. 
 */
/****************************************************************************

              Copyright 1991, by FUJITSU LIMITED
              Copyright 1991, by Sun Microsystems, Inc.	      

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED and Sun
Microsystems, Inc. not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. makes no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

FUJITSU LIMITED AND SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Auther: Riki	Kawaguchi	FUJITSU LIMITED
	Tomoki	Kuriki		FUJITSU LIMITED
	Hideki  Hiura           (hhiura@Sun.COM) Sun Microsystems, Inc.
	
****************************************************************************/

#ifndef lint
#ifdef sccs
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif
#endif

#ifndef _HTT_H_
#define _HTT_H_

#include <X11/Xlib.h>
#ifdef	XSF_X11R5
#include "HttXsfSun.h"
#include "XSunIMProt.h"
#endif				/* XSF_X11R5 */

#ifdef XV11R4
/*
 * The following is specification defines a set of rutines
 * 
 * X Window System, Version 11 Input Method Specifications Post-Public Review
 * Draft 1.1 - propsed
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
    unsigned short  count_styles;
    XIMStyle       *supported_styles;
}               XIMStyles;

#endif				/* XV11R4 */

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

#ifdef	XSF_X11R5
#define XSF_COMMIT_CMSG_MAX	11
#endif				/* XSF_X11R5 */

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

#if	defined(XSF_X11R5) && defined(USE_X11R5_LIB)
typedef struct _FXimpFont {
    GC              normal_gc ;
    GC              reverse_gc ;
    GC              invert_gc ;
    XFontSet	   *font_set ;
    short           max_font_width ;
    short           max_font_height ;
    short           max_font_descent ;
    short           max_font_ascent ;
    unsigned char   status ;
} FXimpFont;
#else
typedef struct _FXimpFont {
    GC              ascii_kana_gc;
    GC              kanji_gc;
    GC              gaiji_gc;
    GC              ascii_kana_reverse_gc;
    GC              kanji_reverse_gc;
    GC              gaiji_reverse_gc;
    GC              invert_gc;
    XFontStruct    *ascii_kana_font_struct;
    XFontStruct    *kanji_font_struct;
    XFontStruct    *gaiji_font_struct;
    short           ascii_kana_font_width;
    short           kanji_font_width;
    short           max_font_height;
    short           max_font_descent;
    short           max_font_ascent;
    unsigned char   fixe_width;
    unsigned char   status;
}               FXimpFont;
#endif	/* XSF_X11R5 */

#ifdef	XSF_X11R5
typedef struct _FXimp_PreeditAreaInfo {
    short	    area_start_pos ;
    short	    area_draw_length ;
    XRectangle	    preedit_area ;
} FXimp_PreeditAreaInfo ;
#endif	/* XSF_X11R5 */

#ifdef	XSF_X11R5
typedef struct _XsfClientFunc {
    int             (*preedit_start) ();
    int             (*preedit_draw) ();
    int             (*preedit_caret) ();
    int             (*preedit_done) ();
    int             (*status_start) ();
    int             (*status_draw) ();
    int             (*status_done) ();
    int             (*lookup_start) ();
    int             (*lookup_draw) ();
    int             (*lookup_process) ();
    int             (*lookup_done) ();
    int             (*aux_start) ();
    int             (*aux_draw) ();
    int             (*aux_done) ();
}               XsfClientFunc;
#endif				/* XSF_X11R5 */

#ifdef	XSF_X11R5
typedef struct _FXimp_Client {
    Window          window;
    XIMStyle        style;
    unsigned char   prot_type;
    unsigned char   imp_mode;
    unsigned char   ext_ui_back_front;
    unsigned char   frontend_key_press_mask;
    unsigned char   ext_ui_statuswindow;
    unsigned char   rsv2;	/* rserved */
    unsigned char   window_type;
    unsigned char   check_preedit_area_size;
    Ximp_PreeditPropRec *preedit;
    Ximp_StatusPropRec *status;
    char           *preedit_fontname;
    char           *status_fontname;
    FXimpFont      *preedit_font;
    FXimpFont      *status_font;
    Atom            ximp_readprop_atom;
    Window          focus_window;
    Window          preedit_window;
    Window          preedit_window2;
    Window          status_window;
    short           used_preedit_window_no;
#ifdef	USE_X11R5_LIB
    FXimp_PreeditAreaInfo preedit_area_info[2] ;
#else
    XRectangle      preedit_area[2];
    short           preedit_line_length[2];
    short           preedit_line_no;
#endif	/* USE_X11R5_LIB */
    unsigned char   preedit_visible;
    unsigned char   status_visible;
    unsigned char   henkan_mode;
    unsigned char   siftmode;
    unsigned char   romaji_mode;
    unsigned char   hankakumode;
    unsigned char   physical_kanashift;
    unsigned char   logical_kanashift;
    unsigned char   logical_capslock;
    unsigned char   stage;	/* reserved */
    unsigned long   old_state;
    unsigned char   select_kanji_no;
    unsigned char   select_kanji_mod;
    short           last_colums;
    wchar_t	   *output_buff;
    unsigned short *input_buff;
    short           romaji_colums;
    short           current_colums;
    short           reverse_colums;
    short           input_colums;
    short           convert_colums;
    short           convert_count;
    short           input_flag;
    unsigned char   slcrtok_nhtype;
    unsigned char   slcrtok_input_no;
    unsigned char   slcrtok_inbuf[4];
    unsigned char   slcrtok_outbuf[4];
    /* Public IMServer Engine Interface */
    wchar_t	   *status_message ;
    int		    status_length ;
    int             callback_flg;
    unsigned int    lookup_choice;
    XIMFeedback	   *preedit_attr;
    /* Private SunImLogic Interface */
    int             id;
    XSFrrvCell     *cell_link;
    iml_status_t    iml_status;
    XsfClientFunc   func;
}               FXimp_Client;
#else
typedef struct _FXimp_Client {
    Window          window;
    XIMStyle        style;
    unsigned char   prot_type;
    unsigned char   imp_mode;
    unsigned char   ext_ui_back_front;
    unsigned char   frontend_key_press_mask;
    unsigned char   ext_ui_statuswindow;
    unsigned char   rsv2;	/* rserved */
    unsigned char   window_type;
    unsigned char   check_preedit_area_size;
    Ximp_PreeditPropRec *preedit;
    Ximp_StatusPropRec *status;
    char           *preedit_fontname;
    char           *status_fontname;
    FXimpFont      *preedit_font;
    FXimpFont      *status_font;
    Atom            ximp_readprop_atom;
    Window          focus_window;
    Window          preedit_window;
    Window          preedit_window2;
    Window          status_window;
    XRectangle      preedit_area[2];
    short           used_preedit_window_no;
    short           preedit_line_length[2];
    short           preedit_line_no;
    unsigned char   preedit_visible;
    unsigned char   status_visible;
    unsigned char   henkan_mode;
    unsigned char   siftmode;
    unsigned char   romaji_mode;
    unsigned char   hankakumode;
    unsigned char   physical_kanashift;
    unsigned char   logical_kanashift;
    unsigned char   logical_capslock;
    unsigned char   stage;	/* reserved */
    unsigned long   old_state;
    unsigned char   select_kanji_no;
    unsigned char   select_kanji_mod;
    short           last_colums;
    wchar_t		   *output_buff;
    unsigned short *input_buff;
    short           romaji_colums;
    short           current_colums;
    short           reverse_colums;
    short           input_colums;
    short           convert_colums;
    short           convert_count;
    short           input_flag;
    unsigned char   slcrtok_nhtype;
    unsigned char   slcrtok_input_no;
    unsigned char   slcrtok_inbuf[4];
    unsigned char   slcrtok_outbuf[4];
}               FXimp_Client;
#endif				/* XSF_X11R5 */

typedef struct _FXimp_Client *FXimp_ICID;

extern char    *command_name;
extern Display *display;
extern int      screen;
extern Window   root;
extern FXimp_Client *focus_in_client;
extern FXimp_Client *current_client;
extern FXimp_Client *root_client;
extern FXimpFont *current_preedit_font;
extern short    keybord_type;
extern short    no_beep;
extern short    control_modifiers_grap;
extern char     control_modifiers_flag;

int             InitialClientTable();
int             ExitClientTable();
FXimp_Client   *NewClient();
int             DeleteClient();
FXimp_Client   *FindClient();
FXimp_Client   *FindClientByICID();
FXimp_Client   *FindClientByFocusWindow();
FXimp_Client   *FindClientByPreeditOrStatusWindow();
unsigned short  Hankaku_To_ZenkakuAsciiHiragana();
void            ErrorMessage();
void            WarningMessage();

#define KS_TOP          1
#define KS_BOTTOM       2
#define KS_FOWARD       3
#define KS_BACKWARD     4
#define KS_NEXT         5
#define KS_PREV         6

#ifdef DEBUG
#define Static			/**/
#else
#define Static static
#endif
#endif /* _HTT_H_ */
