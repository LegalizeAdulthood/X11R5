/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from @(#)Henkan.c	2.1 91/08/13 11:28:29 FUJITSU LIMITED. */
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
static char     derived_from_sccs_id[] = "@(#)Henkan.c	2.1 91/08/13 11:28:29 FUJITSU LIMITED.";
static char     sccsid[] = "%Z%%M% %I% %E% Sun Microsystems Inc.";
#endif

typedef unsigned short ushort;
typedef unsigned char uchar;

#include <stdio.h>
#include "HttHenkan.h"

#ifdef DEBUG
#define Static			/**/
#else
#define Static static
#endif

DictionaryFuncRec *current_dictionary_func;
extern DictionaryFuncRec dmy_dictionary_func;

#ifdef FUJITSU_OAK
extern DictionaryFuncRec sxg_dictionary_func;
#endif				/* FUJITSU_OAK */

#ifdef SJ3_DICTIONARY
extern DictionaryFuncRec sj3_dictionary_func;
#endif				/* SJ3_DICTIONARY */

DictionaryFuncRec *dictionary_func_tbl[] = {

#ifdef FUJITSU_OAK
    &sxg_dictionary_func,
#endif				/* FUJITSU_OAK */

#ifdef SJ3_DICTIONARY
    &sj3_dictionary_func,
#endif				/* SJ3_DICTIONARY */

    &dmy_dictionary_func
};

int             dictionary_no = 0;

int             no_of_dictionary_func = sizeof(dictionary_func_tbl)
/ sizeof(dictionary_func_tbl[0]);

int
Dic_Argv(argc, argv)
    int             argc;
    char           *argv[];
{
    int             j, rc;
    DictionaryFuncRec *current_dictionary_func;

    for (j = 0; j < no_of_dictionary_func - 1; j++) {
	current_dictionary_func = dictionary_func_tbl[j];
	if (*current_dictionary_func->selecut_name
	    && !strcmp(argv[0], current_dictionary_func->selecut_name)) {
	    dictionary_no = j;
	    return (1);
	}
    }

    for (j = 0; j < no_of_dictionary_func; j++) {
	current_dictionary_func = dictionary_func_tbl[j];
	if ((int *) *current_dictionary_func->argv) {
	    rc = (*current_dictionary_func->argv) (argc, argv);
	    if (rc != 0)
		return (rc);
	}
    }
    return (0);
}

int
Dic_Help(fp)
    FILE           *fp;
{
    int             j;
    DictionaryFuncRec *current_dictionary_func;

    if (no_of_dictionary_func > 2) {
	for (j = 0; j < no_of_dictionary_func - 1; j++) {
	    current_dictionary_func = dictionary_func_tbl[j];
	    if (*current_dictionary_func->selecut_name) {
		fprintf(fp, "\t[%s ", current_dictionary_func->selecut_name);
		if ((int *) *current_dictionary_func->help) {
		    (*current_dictionary_func->help) (fp);
		}
		fprintf(fp, "]\n");
	    }
	}
    } else {
	current_dictionary_func = dictionary_func_tbl[0];
	if ((int *) *current_dictionary_func->help) {
	    fprintf(fp, "\t");
	    (*current_dictionary_func->help) (fp);
	    fprintf(fp, "\n");
	}
    }
    return (0);
}

int
Dic_Open()
{
    current_dictionary_func = dictionary_func_tbl[dictionary_no];
    if ((int *) *current_dictionary_func->open)
	return (*current_dictionary_func->open) ();
    return (0);
}

int
Dic_Close()
{
    if ((int *) *current_dictionary_func->close)
	return (*current_dictionary_func->close) ();
    return (0);
}

int
Dic_Henkan(cnvrt, count, yomi, yomi_len, out_buff, out_size)
    uchar           cnvrt;
    int             count;
    ushort         *yomi;
    int             yomi_len;
    ushort         *out_buff;
    int             out_size;
{
    if ((int *) *current_dictionary_func->henkan)
	return (*current_dictionary_func->henkan) (cnvrt, count, yomi,
					      yomi_len, out_buff, out_size);
    return (0);
}

int
Dic_Zenkouho(type, yomi, yomi_len)
    int             type;
    ushort         *yomi;
    int             yomi_len;
{
    if ((int *) *current_dictionary_func->zenkouho)
	return (*current_dictionary_func->zenkouho) (type, yomi, yomi_len);
    return (0);
}

int
Dic_HenkanCancel()
{
    if ((int *) *current_dictionary_func->cancel)
	return (*current_dictionary_func->cancel) ();
    return (0);
}
