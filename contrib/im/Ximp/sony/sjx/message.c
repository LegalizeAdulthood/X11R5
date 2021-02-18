/*
 * Copyright 1991 Sony Corporation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Sony not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Sony makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * SONY DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL SONY
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masaki Takeuchi, Sony Corporation
 */
/*
 *	message.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Sat May 30 14:47:48 JST 1987
 */
/*
 * $Header: message.c,v 1.1 91/09/04 23:08:30 root Locked $ Sony;
 */

char *mess_warning
	="warning: Please copy user dictionary to your home directory.\n\r";
char *mess_error1
	="error: Can't open user dictionary (%s).\n\r";
char *mess_error2
	="error: Can't open main dictionary (%s).\n\r";

char	*SJrun		= "\202\162\202\151\202\167\201\100\201\100";

char	*Mode_buff	= "";
char	*Mode_unbuff	= "";

char	*Mode_Zhira	= "\221\123\202\320\202\347\202\252\202\310";
char	*Mode_Zkata	= "\221\123\203\112\203\136\203\112\203\151";
char	*Mode_Hkata	= "\224\274\203\112\203\136\203\112\203\151";
char	*Mode_Zalpha	= "\221\123\203\101\203\213\203\164\203\100";
char	*Mode_Halpha	= "\224\274\203\101\203\213\203\164\203\100";

char	*Gmode
		= "  \223\157\230\136 \217\301\213\216 \217\103\220\263 \201\100\201\100 code  \224\274\203\101 \221\123\203\101 \224\274\203\112 \221\123\203\112 \202\251\202\310";

char	*GEtc		= "\202\273\202\314\221\274\201\100\201\100";
char	*Getc
		= "  \223\157\230\136 \217\301\213\216 Help sjrc \217\103\220\263  \201\100\201\100 \201\100\201\100 \201\100\201\100 \201\100\201\100 \201\100\201\100";

char	*GHelp		= "Help\201\100Mode";
char	*MHelp		= "  \203\113\203\103\203\150\201\105\203\211\203\103\203\223 %s \202\311\202\310\202\350\202\334\202\265\202\275 ";

/* guide code input	*/

char	*GCshift	= "ShiftJIS\201\100";
char	*GCeuc		= "\201\100\202\144\202\164\202\142\201\100";
char	*GCjis		= "\201\100\202\151\202\150\202\162\201\100";
char	*GCkuten	= "\201\100\201\100\213\346\223\137\201\100";

/* guide toroku		*/

char	*Gtoroku	= "\223\157\230\136";
char	*Mhani		= " \224\315\210\315\202\360\216\167\222\350\202\265\202\304\202\255\202\276\202\263\202\242 ";
char	*Myomi		= " \212\277\216\232 [%s] : \223\307\202\335\223\374\227\315 ";
char	*TOUROKU_OK	= " \212\277\216\232 [%s] : \223\307\202\335 [%s] (y/n)? ";
char	*TOUROKU_FAIL	= " \223\157\230\136\202\305\202\253\202\334\202\271\202\361\202\305\202\265\202\275 ";
char	*TOUROKU_SUCC	= " \223\157\230\136\202\265\202\334\202\265\202\275 ";

/* guide syoukyo	*/

char	*Gsyoukyo	= "\217\301\213\216";
char	*SYOUKYO_FAIL	= " \217\301\213\216\202\305\202\253\202\334\202\271\202\361\202\305\202\265\202\275 ";
char	*SYOUKYO_SUCC	= " \217\301\213\216\202\265\202\334\202\265\202\275 ";

/* get sjrc		*/

char	*SUCCsjrc	= "\202\360\223\307\202\335\215\236\202\335\202\334\202\265\202\275";
char	*FAILsjrc	= "\202\360\223\307\202\335\215\236\202\337\202\334\202\271\202\361\202\305\202\265\202\275";

/* guide hensyuu	*/

char	*Gedit		= "\217\103\220\263 ";
