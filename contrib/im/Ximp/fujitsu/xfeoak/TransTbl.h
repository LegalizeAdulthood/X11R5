/* @(#)TransTbl.h	2.2 91/07/05 17:02:15 FUJITSU LIMITED. */
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

typedef int (*FtActionProc)();
typedef unsigned long FtArg;

typedef struct _FtActionRec {
	char		*name;		/* Action Name	*/
	FtActionProc	proc;		/* Action proc	*/
	} FtActionRec;

typedef struct _FtModify {
	char	*name;
	int	state;			/* Key state		*/
	} FtModify;

#define	FtKey		0		/* KeyPress		*/
#define	FtKeyUp		(1<<16)		/* KeyRelease		*/

#define FT_ASCII	0x7FFF01
#define FT_KANA		0x7FFF02

typedef struct _FtTransTable {
	struct _FtTransTable *next_item;/* next_item _FtTransTable */
	unsigned int state;		/* key or button mask	*/
	KeySym	keysym;
	FtActionProc proc;
	FtArg	arg;
	} FtTransTable;

typedef struct _FtTransMode {			
	struct _FtTransTable *hiragana;		
	struct _FtTransTable *katakana;		
	struct _FtTransTable *bigascii;		
	struct _FtTransTable *smallascii;	
	struct _FtTransTable *henkanoff;	
	} FtTransMode;

#define FK_HIRAGANA	1
#define FK_KATAKANA	2
#define FK_UPPER_ASCII	3
#define FK_LOWER_ASCII	4
#define FK_ROMA		6
#define FK_HANKAKU	7
#define FK_NIHONGO	8
#define FK_ROMA_ON	10
#define FK_ROMA_OFF	11
#define FK_NIHONGO_ON	12
#define FK_NIHONGO_OFF	13
#define FK_KANA_ON	14
#define FK_KANA_OFF	15

extern int	Fj_PutKana();
extern int	Fj_PutAscii();
extern int	Fj_Henkan();
extern int	Fj_Muhenkan();
extern int	Fj_Maekouho();
extern int	Fj_HenkanCancel();
extern int	Fj_HenkanExecute();
extern int	Fj_HenkanExecuteLinefeed();
extern int	Fj_BackSpace();
extern int	Fj_Tab();
extern int	Fj_CursorMoveTop();
extern int	Fj_CursorMoveBottom();
extern int	Fj_CursorMoveFoward();
extern int	Fj_CursorMoveBackward();
extern int	Fj_DeleteChar();
extern int	Fj_InsertChar();
extern int	Fj_SelectKanji();
extern int	Beep();
extern int	Fj_ChangeMode();
extern int	Fj_SendEvent();

extern FtTransMode    *transmode;
extern FtTransMode    default_jis_mode;
extern FtTransMode    default_oak_mode;
extern FtTransMode    default_non_mode;
