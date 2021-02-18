/* @(#)KanjiTbl.h	2.1 91/07/03 11:28:50 FUJITSU LIMITED. */
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

typedef struct _SelectKanjiItem {
	struct _SelectKanjiItem *next_item;
	short			kanji_length;	
	unsigned short		kanji_data[1];
	} SelectKanjiItem;

typedef struct _SelectKanjiLine {
	SelectKanjiItem		*top_item;	
	struct _SelectKanjiLine *next_line;
	struct _SelectKanjiLine *previous_line;	
	int			item_no;	
	} SelectKanjiLine;

typedef struct _SelectKanjiDisplayItem {
	int			x;
	int			y;
	int			width;
	SelectKanjiItem		*select_kanji_item_pointer;
	} SelectKanjiDisplayItem;

extern int 			height_select_kanji_display_item;
extern int 			select_kanji_item_no;
extern SelectKanjiItem 		*top_select_kanji_item;
extern SelectKanjiItem 		*end_select_kanji_item;
extern int 			select_kanji_display_item_no;
extern SelectKanjiDisplayItem 	select_kanji_display_item[10];

extern SelectKanjiLine 		*top_select_kanji_line;
extern SelectKanjiLine 		*end_select_kanji_line;
extern SelectKanjiLine 		*current_select_kanji_line;

extern char  			*top_kanji_buff;
extern char  			*end_kanji_buff;
