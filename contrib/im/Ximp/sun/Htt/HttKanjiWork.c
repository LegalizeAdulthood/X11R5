/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from @(#)KanjiWork.c	2.1 91/08/13 11:28:30 FUJITSU LIMITED. */
/*
****************************************************************************

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

Auther: Takashi Fujiwara	FUJITSU LIMITED
				fujiwara@a80.tech.yk.fujitsu.co.jp
	Kiyoaki oya		FUJITSU LIMITED
	Riki	Kawaguchi	FUJITSU LIMITED
	Tomoki	Kuriki		FUJITSU LIMITED
	Hideki  Hiura           (hhiura@Sun.COM) Sun Microsystems, Inc.

****************************************************************************
*/

#ifndef lint
static char     derived_from_sccs_id[] = "@(#)KanjiWork.c	2.1 91/08/13 11:28:30 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

#include <stdio.h>

typedef unsigned short ushort;
#include "HttKanjiTbl.h"

char           *top_kanji_buff;
char           *end_kanji_buff;
char           *current_kanji_buff;

int
InitialKanjiBuff(size)
    int             size;
{
    extern char    *malloc();
    extern char    *command_name;
    top_kanji_buff = malloc(size);
    if (top_kanji_buff == 0) {
	fprintf(stderr, "%s : error malloc (kanji buff)\n", command_name);
	return (-1);
    }
    current_kanji_buff = top_kanji_buff;
    end_kanji_buff = top_kanji_buff + size;
    return (0);
}

char           *
GetKanjiBuff(size)
    int             size;
{
    char           *new_kanji_bufp;

    if (size & 0x1)
	size++;

    if ((current_kanji_buff + size) >= end_kanji_buff)
	return ((char *) 0);
    new_kanji_bufp = current_kanji_buff;
    current_kanji_buff += size;
    return (new_kanji_bufp);
}

int
FreeKanjiBuff()
{
    current_kanji_buff = top_kanji_buff;
}


int
InitialKanjiItem()
{
    top_select_kanji_line = (SelectKanjiLine *) GetKanjiBuff(sizeof(SelectKanjiLine));
    end_select_kanji_line = top_select_kanji_line;
    top_select_kanji_line->item_no = 0;
    top_select_kanji_line->next_line = (SelectKanjiLine *) 0;
    top_select_kanji_line->previous_line = (SelectKanjiLine *) 0;

    select_kanji_item_no = 0;
    top_select_kanji_item = (SelectKanjiItem *) 0;
    end_select_kanji_item = (SelectKanjiItem *) 0;
}

int
AddKanjiItem(length, wbuff)
    int             length;
    ushort         *wbuff;
{
    SelectKanjiItem *get_kanji_item, *current_kanji_item;
    int             size;

    for (current_kanji_item = top_select_kanji_item;
	 current_kanji_item > (SelectKanjiItem *) 0;
	 current_kanji_item = current_kanji_item->next_item) {
	if (current_kanji_item->kanji_length == length) {
	    if (memcmp(current_kanji_item->kanji_data, wbuff, length * 2) == 0) {
		return (1);
	    }
	}
    }

    size = length * 2 - 2 + sizeof(SelectKanjiItem);
    get_kanji_item = (SelectKanjiItem *) GetKanjiBuff(size);
    if (!get_kanji_item)
	return (-1);
    if (end_select_kanji_item)
	end_select_kanji_item->next_item = get_kanji_item;
    else {
	top_select_kanji_item = get_kanji_item;
	top_select_kanji_line->top_item = get_kanji_item;
    }
    get_kanji_item->next_item = (SelectKanjiItem *) 0;
    get_kanji_item->kanji_length = length;
    CopyUshort(get_kanji_item->kanji_data, wbuff, length);
    select_kanji_item_no++;
    end_select_kanji_item = get_kanji_item;
    return (0);
}
