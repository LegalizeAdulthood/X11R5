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
 *	stat_conv.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Fri May 29 15:44:50 JST 1987
 */
/*
 * $Header: stat_conv.c,v 1.1 91/04/29 17:56:11 masaki Exp $ SONY;
 */

#include "common.h"
#include "key.h"

int	max_buflen = 0;		/* This flag is only used in sjx	*/

/* extern			*/

extern int	keyvalue;

/* save span		*/

int		vecho = 1;			/* echo mode 89/05/15	*/

#define SPAN_FLUSH		0
#define SPAN_CONT		1
#define SPAN_END		2
#define SPAN_CHMOD		3

int	sj = 1;

set_sj ()
{
	sj = 1;
}

set_ec ()
{
	sj = 0;
}

end_span ()
{
	save_span (SPAN_END);
}

static int	push_moji_count;

delete_current_span ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	save_span_l ();
	cv->ha_point -= cv->span_l[cv->span_point];
	cv->Halpha[cv->ha_point] = '\0';
	cv->span_point --;
	HAindex_set ();
	cv->moji_count = push_moji_count;
	save_span (SPAN_CONT);
}

next_span ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	NextSpan ();
	push_moji_count = cv->moji_count;
}

NextSpan ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	save_span_l ();
	cv->span_point ++;
	save_span_p ();
}

ChangeSpan ()
{
	save_span (SPAN_CONT);
	save_span (SPAN_CHMOD);
}

get_current_span (s)	/* get current span, hankaku katakana		*/
u_char	*s;
{
	register Conversion	*cv;
	u_char		Htmp[BUFFLENGTH], Ztmp[BUFFLENGTH * 2];
	register int	len;

	cv = GetConversion ();
	romaji_flush (Ztmp);
	save_span_l ();
	len = cv->span_l[cv->span_point];
	Strncpy (Htmp, cv->span_p[cv->span_point], len);
	Htmp[len] = '\0';
	HAromajiZK2 (MODE_ZHIRA, Htmp, s, 0);
}

get_current_span2 (s)	/* get current span, hankaku katakana		*/
u_char	*s;
{
	register Conversion	*cv;
	u_char		Htmp[BUFFLENGTH];
	register int	len;

	cv = GetConversion ();
	save_span_l ();
	len = cv->span_l[cv->span_point];
	Strncpy (Htmp, cv->span_p[cv->span_point], len);
	Htmp[len] = '\0';
	HAromajiZK2 (MODE_ZHIRA, Htmp, s, 0);
}

span_len ()		/* return current span len	*/
{
	register Conversion	*cv;

	cv = GetConversion ();
	return (&(cv->Halpha[cv->ha_point]) - cv->span_p[cv->span_point]);
}

save_span (mode)
register int	mode;
{
	register Conversion	*cv;
	u_char	tmp[SHORTBUFFSIZE], tmp2[SHORTBUFFSIZE * 2];

	cv = GetConversion ();
	switch (mode) {
		case SPAN_FLUSH:
			cv->span_point = 0;
			cv->ha_point = 0;
			save_span_p ();
			break;
		case SPAN_END:
		case SPAN_CHMOD:
			if (cv->o_mode != SPAN_CHMOD && cv->o_mode != SPAN_END) {
				*tmp = '\0';
				romaji_flush (tmp);
				if (!BuffYes () && *tmp != '\0') {
					ZAtoHA (tmp, tmp2);
					strcat (cv->Halpha, tmp2);
					cv->ha_point += strlen (tmp2);
				}
				if (save_span_l ()) {
					span_kettei ();
					cv->span_point ++;
					save_span_p ();
				}
				else if (cv->span_point > 0 &&
				    cv->Jmode == cv->kettei[cv->span_point - 1]) {
					cv->span_point --;
				}
			}
			break;
		case SPAN_CONT:
		default:
			break;
	}
	cv->o_mode = mode;
}

span_kettei ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->kettei[cv->span_point] = cv->Imode;
}

save_span_p ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->span_p[cv->span_point] = &(cv->Halpha[cv->ha_point]);
	cv->span_l[cv->span_point] = 0;
}

save_span_l ()
{
	register Conversion	*cv;
	register int	i;

	cv = GetConversion ();
	i = cv->span_point;
	if (cv->span_p[i] >= &(cv->Halpha[cv->ha_point])) {
		if (i <= 0) {
			i = cv->span_point = 0;
			cv->span_p[i] = cv->Halpha;
		}
		else {
			cv->span_l[i] = 0;
			return (0);
		}
	}
	cv->span_l[i] = &(cv->Halpha[cv->ha_point]) - cv->span_p[i];
	return (cv->span_l[i]);
}

span_clear ()
{
	save_span (SPAN_FLUSH);
}

static	u_char		Htmp[BUFFLENGTH * 2];

load_spanning (num, s, len)
register int	num;	/* span number		*/
register u_char	**s;	/* return: str		*/
register int	*len;	/* return: str len	*/
{
	register Conversion	*cv;
	register int	type;		/* span type	*/
	register int	tmplen;
	register u_char	*p;

	cv = GetConversion ();
	p = cv->span_p[num];
	tmplen = 0;
	if ((cv->ha_point < BUFFLENGTH) && (p != (u_char *)NULL))
		tmplen = &(cv->Halpha[cv->ha_point]) - p;
	if ((tmplen > 0) && (tmplen < BUFFLENGTH)) {
		Strncpy (Htmp, cv->span_p[num], tmplen);
		Htmp[tmplen] = '\0';
		type = cv->Imode;
		return (GetSpanString (type, Htmp, num, s, len, 0));
	}
	else {
		*len = 0;
		return (0);
	}
}

load_span (num, s, len)
register int	num;	/* span number		*/
register u_char	**s;	/* return: str		*/
register int	*len;	/* return: str len	*/
{
	register Conversion	*cv;
	register int	type;		/* span type	*/
	register int	tmplen;

	cv = GetConversion ();
	tmplen = cv->span_l[num];
	Strncpy (Htmp, cv->span_p[num], tmplen);
	Htmp[tmplen] = '\0';
	type = cv->kettei[num];

	return (GetSpanString (type, Htmp, num, s, len, 1));
}

GetSpanString (type, Htmp, num, s, len, flush_mode)
register int	type;
u_char		*Htmp;
register int	num;
register u_char	**s;	/* return: str		*/
register int	*len;	/* return: str len	*/
int		flush_mode;
{
	register Conversion	*cv;
	u_char		Ztmp[BUFFLENGTH * 2];
	int		mod;		/* 0: hankaku, 1: zenkaku	*/

	cv = GetConversion ();
	switch (type) {
		case MODE_SCODE:
		case MODE_ECODE:
		case MODE_JCODE:
		case MODE_KCODE:
			PushCode (type);
			Getcode (Htmp, cv->ZZbuff);
			PopCode ();
			mod = 1;
			break;
		case MODE_HALPHA:
			strcpy (cv->ZZbuff, Htmp);
			mod = 0;
			break;
		case MODE_ZALPHA:
			HAtoZA (Htmp, cv->ZZbuff);
			mod = 1;
			break;
		case MODE_HKATA:
			HAromajiZK2 (type, Htmp, Ztmp, flush_mode);
			ZKtoHK (Ztmp, cv->ZZbuff);
			mod = 0;
			break;
		case MODE_ZKATA:
			HAromajiZK2 (type, Htmp, cv->ZZbuff, flush_mode);
			mod = 1;
			break;
		case MODE_ZHIRA:
		default:
			HAromajiZK2 (MODE_ZHIRA, Htmp, Ztmp, flush_mode);
			ZKtoZH (Ztmp, cv->ZZbuff);
			mod = 1;
			break;
	}
	*s = cv->ZZbuff;
	*len = strlen (cv->ZZbuff);
	return (mod);
}

isspaned ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	if (*(cv->span_p[0]) != '\0')
		return (1);
	else
		return (0);
}

/* pointers clear	*/

buffers_clear ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	pointer_clear ();
	SJ_romaji_clear ();
	display_count_clear ();
	if (flush_conversion)
		cv->CurBun = -1;
	else
		cv->CurBun = 0;
}

pointer_clear ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->ha_point = 0;
	*(cv->Halpha) = '\0';
	cv->Kanji = 0;
	cv->moji_count = 0;
	HAindex_set ();
	span_clear ();
}

typedef struct	_SaveSet {
	u_short HAindex[BUFFLENGTH];
	u_short Amojilen[BUFFLENGTH];
	u_short span_l[BUFFLENGTH];
	u_short ha_point;
	u_short moji_count;
	u_short span_point;
	u_short HAindex_p;
	u_char	*span_p[BUFFLENGTH];
	u_char	Halpha[BUFFLENGTH];
	u_char	Dmojimod[BUFFLENGTH];
	u_char	kettei[BUFFLENGTH];
} SaveSet;

SaveSet		save_set0;

SaveSet *
GetSaveSet ()
{
	return (&save_set0);
}

StoreSaveSet ()
{
	register Conversion	*cv;
	register SaveSet	*sv;

	cv = GetConversion ();
	if (cv->ha_point) {
		sv = GetSaveSet ();
		sv->HAindex_p = cv->HAindex_p;
		sv->span_point = cv->span_point;
		sv->moji_count = cv->moji_count;
		sv->ha_point = cv->ha_point;
		bcopy (cv->HAindex, sv->HAindex,
			cv->HAindex_p * sizeof(u_short));
		bcopy (cv->Amojilen, sv->Amojilen,
			cv->moji_count * sizeof(u_short));
		bcopy (cv->span_l, sv->span_l,
			cv->span_point * sizeof(u_short));
		bcopy (cv->span_p, sv->span_p,
			cv->span_point * sizeof(u_char *));
		strcpy (sv->Halpha, cv->Halpha, cv->ha_point);
		strcpy (sv->Dmojimod, cv->Dmojimod, cv->moji_count);
		strcpy (sv->kettei, cv->kettei, cv->span_point);
	}
}

RestoreSaveSet ()
{
	register Conversion	*cv;
	register SaveSet	*sv;

	sv = GetSaveSet ();
	if (sv->ha_point) {
		cv = GetConversion ();
		cv->HAindex_p = sv->HAindex_p;
		cv->span_point = sv->span_point;
		cv->moji_count = sv->moji_count;
		cv->ha_point = sv->ha_point;
		bcopy (sv->HAindex, cv->HAindex,
			sv->HAindex_p * sizeof(u_short));
		bcopy (sv->Amojilen, cv->Amojilen,
			sv->moji_count * sizeof(u_short));
		bcopy (sv->span_l, cv->span_l,
			sv->span_point * sizeof(u_short));
		bcopy (sv->span_p, cv->span_p,
			sv->span_point * sizeof(u_char *));
		strcpy (cv->Halpha, sv->Halpha, cv->ha_point);
		strcpy (cv->Dmojimod, sv->Dmojimod, cv->moji_count);
		strcpy (cv->kettei, sv->kettei, cv->span_point);
	}
}

history_empty ()
{
	SaveSet		*sv;

	sv = GetSaveSet ();
	if (sv->ha_point)
		return (0);
	else
		return (1);
}

History ()
{
	Conversion	*cv;
	SaveSet		*sv;

	sv = GetSaveSet ();
	if (sv->ha_point) {
		RestoreSaveSet ();
		cv = GetConversion ();
		if (flush_conversion)
			cv->CurBun = -1;
		else
			cv->CurBun = 0;
	}
}

ClearBuffer ()
{
	Ccheck ();
	Cclear (1);
	all_set_up ();
}

/* kettei			*/

SetCflag ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->Cflag = 1;
}

ClearCflag ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->Cflag = 0;
}

IsCflag ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	return (cv->Cflag);
}

exec_kettei ()
{
	register Conversion	*cv;
	register int	i;
	int		len;
	u_char		kanji[BUFFLENGTH * 2];
	u_char		*s;

	cv = GetConversion ();
	save_span (SPAN_END);
#ifdef SJX
	CleanUp ();
#endif /* SJX */
	if (cv->Kanji != 0) {
		Bkanji (kanji);
		s = kanji;
		len = strlen (kanji);
#ifndef SJX
		Ccheck ();
		Cclear (1);
#endif /* !SJX */
		master_write (s, len);
#ifdef SJX
		Ccheck ();
		Cclear (2);
		master_flush ();
#endif /* SJX */
		SJ2_kettei ();
		if (cv->word_regist) {
			cv->word_regist = 0;
		}
		else {
			if (is_bstudy ()) {
				cl_gakusyuu ();
			}
		}
	}
	else if (isspaned ()) {
#ifndef SJX
		if (BuffYes ()) {
			Ccheck ();
			Cclear (1);
		}
#endif /* !SJX */
		for (i = 0 ; i < cv->span_point ; i ++) {
			load_span (i, &s, &len);
			if (!BuffYes ())
				vbackchar (len);
			if (len) {
				master_write (s, len);
			}
		}
#ifdef SJX
		if (BuffYes ()) {
			Ccheck ();
			Cclear (2);
			master_flush ();
		}
#endif /* SJX */
#ifdef SJ2
		if (BuffYes ())
			master_flush ();
#endif /* SJ2 */
		StoreSaveSet ();
	}
#ifdef SJX
	Reconfigure ();
#endif /* SJX */
	buffers_clear ();
	ResetHenkan ();
	SetCflag ();
}

/* toggle buffering mode	*/

ChangeBuffMode ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	save_span (SPAN_CHMOD);
	if (cv->BuffMode == BUFF)
		cv->BuffMode = UNBUFF;
	else
		cv->BuffMode = BUFF;
}

BuffYes ()
{
	register Conversion	*cv;

	cv = GetConversion ();
	if (cv->BuffMode == BUFF)
		return (1);
	else
		return (0);
}

/* convert status		*/

#define STAT_HENKAN	1
#define STAT_MUHENKAN	2

IsHenkan ()
{
	Conversion	*cv;

	cv = GetConversion ();
	return (cv->henkanflag);
}

IsMuhenkan ()
{
	Conversion	*cv;

	cv = GetConversion ();
	return (cv->henkanflag & STAT_MUHENKAN);
}

SetMuhenkan ()
{
	Conversion	*cv;

	cv = GetConversion ();
	cv->henkanflag |= STAT_MUHENKAN;
}

SetHenkan ()
{
	Conversion	*cv;

	cv = GetConversion ();
	cv->henkanflag = STAT_HENKAN;
}

ResetHenkan ()
{
	Conversion	*cv;

	cv = GetConversion ();
	cv->henkanflag = 0;
}

all_set_up ()
{
	buffers_clear ();	/* all buffers initialize		*/
	SetCflag ();		/* cursor save flag on			*/
	ResetHenkan ();
}

static int	convertflag;	/* hold covert status		*/
static int	small;
static int	PushPreCodeMode;

stat_conv_on ()
{
	convertflag = 1;	/* 0: end convert, 1: continue convert	*/
}

stat_init (sj)
int	sj;
{
	stat_conv_on ();
	all_set_up ();
	disp_mode ();		/* display mode line			*/
	small = 0;
}

stat_end ()
{
}

get_pseq ()
{
#ifdef SJX
	return (write_pseq (2));
#else /* SJX */
	return (write_pseq (1));
#endif /* SJX */
}

stat_conv (sj)
int	sj;
{
	register Conversion	*cv;
	extern int	allways_buff;	/* allways buff mode		*/
	register int	inc;	/* input character from inkey routine	*/

	cv = GetConversion ();
	if (sj) {
		cv->word_regist = 0;
		vecho = 1;
	}
	else {
		/* europe */
		vecho = 0;
	}

	while (convertflag) {
	    if ((inc = inkey ()) == EOF)
		break;
		/* Developing sjx 2.00		*/
		/* 1989.08.08 masaki		*/
		/* This is important		*/
	    cv = GetConversion ();
	    if (allways_buff)
		cv->BuffMode = BUFF;
	    if (!sj) {
		cv->BuffMode = UNBUFF;
		switch (keyvalue) {
			case KEY_NORMAL:
				if (inc == EOF)
					break;
				exec_conv (inc);
				break;
			case KEY_KETTEI:
				if (!IsCflag ())
					exec_kettei ();
				break;
			case KEY_CONV:
				if (!IsCflag ())
					exec_kettei ();
				convertflag = 0;
				break;
			case KEY_HENKAN:
				ChangeMode (MODE_HKATA);
				disp_mode ();
				break;
			case KEY_MUHENKAN:
				ChangeMode (MODE_HALPHA);
				disp_mode ();
				break;
		}
	    }
	    else {
		switch (keyvalue) {
		/*
		 * "keyvalue" is a extern, set by inkey ()
		 * "inc" is a input character, in the case of KEY_NORMAL
		 */
		case KEY_NORMAL:
normal:
			small = 0;
			if (inc == EOF)
				break;
			if (IsHenkan ()) {
				if (is_bsdel (inc)) {
					Bdelete ();
					break;
				}
				else if (inc < ' ' &&
					 inc != '\n' &&
					 inc != '\r' &&
					 inc != '\t' &&
					 !IsESC (inc) &&
					 !isintr (inc)){
					beep ();
					break;
				}
				else {
					exec_kettei ();
				}
			}
			exec_conv (inc);
			break;
		case KEY_KETTEI:
			small = 0;
			if (!IsCflag ()) {
				exec_kettei ();
			}
			else if (cv->Imode != MODE_HALPHA ||
				BuffYes ()) {
				if (cv->Imode != MODE_HALPHA)
					exec_mode (KEY_HALPHA);
#ifndef SJX
				if (BuffYes ())
					ChangeBuffMode ();
#endif /* !SJX */
				disp_mode ();
			}
#ifdef SJX
			else if ((inc = get_pseq ()) != 0)
				goto normal;
#else /* SJX */
			else if ((inc = write_pseq (2)) != 0)
				goto normal;
#endif /* SJX */
			break;
		case KEY_CONV:
			small = 0;
			if (!no_flush)
				exec_kettei ();
			convertflag = 0;
			break;
		case KEY_CANCEL:
			small = 0;
			if (!IsCflag ())
				ClearBuffer ();
			else
				goto def;
			break;
		case KEY_RECONV:
			small = 0;
			if (IsCflag ()) {
				if (history_empty ()) {
					goto def;
				}
				else {
					History ();
					Csave ();
					ClearCflag ();
				}
			}
			else {
				if (IsHenkan ()) {
					History ();
					ResetHenkan ();
				}
				else {
					beep ();
					break;
					/* not fall into	*/
				}
			}
			exec_henkan ();
			SetHenkan ();
			break;
		case KEY_HENKAN:
			small = 0;
			if (!IsCflag ()) {
				if (IsMuhenkan ()) {
					exec_henkan ();
					SetHenkan ();
				}
				if (IsHenkan ()) {
					exec_douon ();
				}
				else {
					save_span (SPAN_END);
					Ccheck ();
					StoreSaveSet ();
					exec_henkan ();
					SetHenkan ();
				}
			}
#ifdef SJX
			else if ((inc = get_pseq ()) != 0)
#else /* SJX */
			else if ((inc = write_pseq (3)) >= ' '
			       && inc <= '~')
#endif /* SJX */
				goto normal;
			else if (cv->Imode != MODE_ZHIRA
				|| !BuffYes ()) {
				if (cv->Imode != MODE_ZHIRA) {
					exec_mode (KEY_ZHIRA);
				}
				if (!BuffYes ()) {
					ChangeBuffMode ();
				}
				disp_mode ();
			}
			break;
		case KEY_MUHENKAN:
			small = 0;
			if (flush_conversion && !IsCflag ()
				&& cv->CurBun == -1) {
				if (IsHenkan ()) {
					exec_muhenkan (0);
					SetHenkan ();
				}
				else {
					save_span (SPAN_END);
					Ccheck ();
					exec_muhenkan (1);
					SetHenkan ();
					SetMuhenkan ();
				}
				break;
			}
		case KEY_TOGGLE:
		case KEY_ZHIRA:
		case KEY_HALPHA:
		case KEY_ZALPHA:
		case KEY_HKATA:
		case KEY_ZKATA:
		case KEY_CODE:
			small = 0;
			if (cv->CurBun == -1)
				exec_kettei ();
			else if (IsHenkan ()) {
				Bchange (keyvalue);
				break;
			}
			if (keyvalue == KEY_TOGGLE)
				keyvalue = toggle_mode (cv->Imode);
			exec_mode (keyvalue);
			if (keyvalue == KEY_CODE)
				PushPreCodeMode = cv->Imode;
			disp_mode ();
			break;
		case KEY_WRAP:
			small = 0;
			if (IsHenkan ())
				wrap_douon (0);
			else if ((inc = get_pseq ()) != 0)
				goto normal;
			break;
		case KEY_WRAPBACK:
			small = 0;
			if (IsHenkan ())
				wrap_douon (1);
			else if ((inc = get_pseq ()) != 0)
				goto normal;
			break;
		case KEY_LEFT:
		case KEY_RIGHT:
		case KEY_UP:
		case KEY_DOWN:
			if (IsHenkan ()) {
				switch (keyvalue) {
				case KEY_LEFT:
					Bleft ();
					small = 0;
					break;
				case KEY_RIGHT:
					Bright ();
					small = 0;
					break;
				case KEY_UP:
					small = Bbig ();
					break;
				case KEY_DOWN:
					small = Bsmall (small);
					break;
				}
			}
			else if (IsCode (cv->Imode)) {
				small = 0;
				if (BuffYes () && IsCflag ()) {
					Csave ();
					ClearCflag ();
				}
				kigou (PushPreCodeMode);
				if (flush_conversion)
					exec_kettei ();
			}
#ifdef SJ2	/* 1990.09.18 */
			else if (!BuffYes () && cv->Imode == MODE_HALPHA) {
				small = 0;
				write_pseq (0);
			}
#endif /* EDB */
#ifdef SJX	/* 1990.10.03 */
			else if (IsCflag () && (inc = write_pseq (1)) != 0) {
#else /* SJX */
			else if (IsCflag () && (inc = get_pseq ()) != 0) {
#endif /* SJX */
				small = 0;
				goto normal;
			}
			break;
		case KEY_EDIT:
		case KEY_TOROKU:
		case KEY_SYOUKYO:
			small = 0;
			if (IsCflag () && (inc = get_pseq ()) != 0)
				goto normal;
		case KEY_ETC:
		case KEY_HELP:
		case KEY_SJRC:
			small = 0;
			exec_etc (keyvalue);
			disp_mode ();
			break;
		case KEY_BUFFER:
			small = 0;
			if (BuffYes ())
				exec_kettei ();
			ChangeBuffMode ();
			disp_mode ();
			break;
		case KEY_HA_KET:
			small = 0;
			if (!IsCflag ()) {
			    if (cv->Imode != MODE_HALPHA || BuffYes ()) {
				if (BuffYes ()) {
					exec_kettei ();
					ChangeBuffMode ();
				}
				if (cv->Imode != MODE_HALPHA)
					exec_mode (KEY_HALPHA);
				disp_mode ();
			    }
			}
			else if ((inc = get_pseq ()) != 0)
				goto normal;
			break;
		case KEY_FLUSH:
			small = 0;
			if (!IsCflag ()) {
				exec_kettei ();
			}
			else if ((inc = get_pseq ()) != 0)
				goto normal;
			break;
		default:
def:
			small = 0;
			if ((inc = get_pseq ()) != 0)
				goto normal;
			break;
		}
	    }
		if (keyvalue == KEY_CONV
#ifdef SJX
			|| !BuffYes ()
#endif /* SJX */
		) {
			if (!no_flush)
				exec_kettei ();
			convertflag = 0;
		}
	}
	stat_end ();
}

/* execute romaji kana conversion	*/

exec_conv (c)
register int	c;
{
	register Conversion	*cv;
	register int	c2;
	u_char		s[3], s2[3];
	extern u_char	pars_seq[];

	cv = GetConversion ();
	save_span (SPAN_CONT);
	c2 = c;

	/* back space, delete		*/
	if (is_bsdel (c)) {
		exec_backspace (c, cv->ha_point, 1, 0);
			/*
			 * exec_backspace sets Cflag
			 * 1: if not exists in buffer, write master.
			 */
	}

	/* control code			*/
	else if (c >= 0 && c < ' '
		 && (c == '\n' || c == '\r' || c == '\t'
		|| isintr (c)
		|| IsGoto (c) || IsTrap (c)
#ifndef ESCAPE
		|| IsCflag ()
		|| IsESC (c)
#endif /* !ESCAPE */
		)
	) {
		exec_kettei ();
			/*
			 * exec_kettei sets Cflag on
			 */
		*s = c;
#ifdef SJX
		control_flush (s, 1);
#else /* SJX */
		master_write (s, 1);
#endif /* SJX */
		if (IsGoto (c)) {		/* if escape to HA mode	*/
			cv->BuffMode = UNBUFF;
#ifdef SJ2
			if (exec_mode (KEY_HALPHA))
				disp_mode ();
#endif /* SJ2 */
		}
#ifdef SJ2
		if (IsTrap (c)) {		/* if through next char	*/
			*s = inkey ();
			cv = GetConversion ();
			if (keyvalue != KEY_NORMAL)
				*s = pars_seq[0];
			master_write (s, 1);
		}
#endif /* SJ2 */
	}

	/* punct alphabet numeric	*/
	else if ((c >= ' ' && c <= '~')
#ifdef ESCAPE
		|| IsESC (c)
#endif /* ESCAPE */
	) {
		if (c == ' ' && keyvalue == KEY_HENKAN &&
			(cv->Imode == MODE_ZHIRA || cv->Imode == MODE_ZKATA ||
			 cv->Imode == MODE_ZALPHA)
		   ) {
			s[0] = 0x81;
			s[1] = 0x40;
			s[2] = '\0';
			master_write (s, 2);
		}
		else if (!BuffYes ()) {
			exec_romaji (c, c2);
			if (cv->Imode == MODE_HALPHA || cv->Imode == MODE_ZALPHA) {
				buffers_clear ();
			}
			else {
				pointer_clear ();
			}
		}
		else {
			if (IsCflag ()) {
				Csave ();
				ClearCflag ();
			}
			if (cv->ha_point >= BUFFLENGTH - 3 || max_buflen) {
				/* over flow	*/
				beep ();
				return;
			}
			cv->Halpha[cv->ha_point++] = c;
			cv->Halpha[cv->ha_point] = '\0';
			exec_romaji (c, c2);
		}
	}

	/* kana code			*/
	else if (iskana (c) || iskanji (c)) {
		if (iskanji (c)) {
			c2 = inkey ();
			cv = GetConversion ();
		}
		if (BuffYes ()) {
			if (cv->ha_point > 0 &&
			    ' ' <= cv->Halpha[cv->ha_point - 1] &&
			    cv->Halpha[cv->ha_point - 1] <= '~')
				save_span (SPAN_CHMOD);
			if (IsCflag ()) {
				Csave ();
				ClearCflag ();
			}
			if (cv->ha_point >= BUFFLENGTH - 3 || max_buflen) {
				/* over flow	*/
				beep ();
				return;
			}
			if (iskanji (c)) {
				s[0] = c;
				s[1] = c2;
				s[2] = '\0';
				ZKtoHK (s, s2);
				strcpy (&(cv->Halpha[cv->ha_point]), s2);
				cv->ha_point += strlen (s2);
				if (iskana (*s2)) {
					c = *s2;
					if (iskana(s2[1])) {
						exec_romaji (c, 0);
						c = s2[1];
					}
				}
			}
			else
				cv->Halpha[cv->ha_point++] = c;

			cv->Halpha[cv->ha_point] = '\0';
			exec_romaji (c, c2);
		}
		else {
			exec_romaji (c, c2);
			buffers_clear ();
		}
	}

	/* other code	*/
	else
		beep ();
}

/* excute back space	*/

exec_backspace (c, ha_len, wflag, rev)
int	c;	/* back char						*/
int	ha_len;	/* buffering length					*/
int	wflag;	/* yes, write master flag, if no char			*/
int	rev;	/* yes, buffer backspace: span point  reverse flag	*/
{
	register Conversion	*cv;
	u_char		s[2];
	u_char		tmp[BUFFLENGTH * 2];
	register int	mod;
	register u_char	cc;

	cv = GetConversion ();
	if (SJ_romaji_check ()) {
		/* there is string in SJ_romaji ()'s buffer	*/
		if (is_bs (c)) {
			romaji_backchar ();
			/* for XTU and N	*/
			if (!SJ_romaji_check () && ha_len > 0) {
				mod = cv->Dmojimod[cv->moji_count - 1];
				if (mod == cv->Imode &&
				 (mod == MODE_ZHIRA
				 || mod == MODE_ZKATA
				 || mod == MODE_HKATA)) {
					cc = cv->Halpha[cv->ha_point - 1];
					if (IsaSHIIN (cc) || IsaN (cc)) {
						buffers_backspace (c,rev);
						cv->Halpha[cv->ha_point++] = cc;
						cv->Halpha[cv->ha_point] = '\0';
						exec_romaji (cc, cc);
					}
				}
			}
		}
		else {
			romaji_flush (tmp);
		}
	}
	else if (ha_len > 0) {
		/* there are strings in buffers			*/
		buffers_backspace (c, rev);
		if (!SJ_romaji_check () && ha_len > 0) {
			mod = cv->Dmojimod[cv->moji_count - 1];
			if (mod == cv->Imode &&
			 (mod == MODE_ZHIRA
			 || mod == MODE_ZKATA
			 || mod == MODE_HKATA)) {
				cc = cv->Halpha[cv->ha_point - 1];
				if (IsaSHIIN (cc) || IsaN (cc)) {
					buffers_backspace (c,rev);
					cv->Halpha[cv->ha_point++] = cc;
					cv->Halpha[cv->ha_point] = '\0';
					exec_romaji (cc, cc);
				}
			}
		}
	}
	else if (wflag) {
		/* there is no strings				*/
		*s = c;			/* BS or DEL	*/
#ifdef SJX
		control_flush (s, 1);
#else /* SJX */
		master_write (s, 1);
#endif /* SJX */
		ResetHenkan ();
	}
	else {
		return (1);
	}

	if (cv->ha_point <= 0) {
		SetCflag ();
		buffers_clear ();
	}
#ifdef SJX
	Reconfigure ();
#endif /* SJX */
	return (0);
}

/* buffers backspace processing	*/

buffers_backspace (bc, rev)
int	bc;	/* bs or del			*/
int	rev;	/* yes, reverse span point	*/
{
	register Conversion	*cv;
	u_char		copy[SHORTBUFFSIZE];
	register int	c, cc, i, j;
	int		PushMode;

	cv = GetConversion ();
	/* previous character's moji count	*/
	if (cv->moji_count > 0)
		cv->moji_count --;

	/* remove from  buffer			*/
	cv->ha_point -= cv->Amojilen[cv->moji_count];
	if (cv->ha_point < 0)
		cv->ha_point = 0;
	/* do not remove by null character to use to copy	*/
	HAindex_set ();

	/* check displaied characters		*/
	i = cv->Dmojilen[cv->moji_count];

	/* and remove displaied characters	*/
	vbackchar (i);

	/* check remaining characters		*/
	j = cv->Amojilen[cv->moji_count] - 1;

	PushMode = cv->Imode;			/* push current Imode	*/
	cv->Imode = cv->Dmojimod[cv->moji_count];		/* Imode change		*/

	if (j < 1) {
		/* perform invalid shiin reconvert	*/
		cc = cv->Halpha[cv->ha_point - 1];
		if (cv->ha_point > 0 && (IsaSHIIN (cc) || IsaN (cc))) {
			if (cv->moji_count > 0)
				cv->moji_count --;
			cv->ha_point -= cv->Amojilen[cv->moji_count];
			if (cv->ha_point < 0)
				cv->ha_point = 0;
			HAindex_set ();
			i = cv->Dmojilen[cv->moji_count];
			vbackchar (i);
			j = cv->Amojilen[cv->moji_count];
			cv->Imode = cv->Dmojimod[cv->moji_count];
			for (i = 0 ; i < j ; i ++) {
				if (iskanji (c = cv->Halpha[cv->ha_point++])) {
					exec_romaji (c, cv->Halpha[cv->ha_point++]);
					i ++;
				}
				else {
					exec_romaji (c, c);
				}
			}
			if (rev && IsaN (cc) && cv->span_point > 0 &&
			    cv->span_p[cv->span_point] == &(cv->Halpha[cv->ha_point]))
				cv->span_point --;
		}
	}
	else if (is_bs (bc)) {
		/* perform disconvert romaji	*/
		/* remaining string to copy	*/
		for (i = 0 ; i < j  ; i ++)
			copy[i] = cv->Halpha[cv->ha_point + i];
		copy[i] = '\0';

		/* convert copy to romaji	*/
		for (i = 0 ; i < j  ; i ++) {
			if (iskanji (c = copy[i]))
				break;
			cv->Halpha[cv->ha_point++] = c;
			cv->Halpha[cv->ha_point] = '\0';
			exec_romaji (c, c);
		}
	}
	cv->Halpha[cv->ha_point] = '\0';	/* now remove		*/

	/* redisplay mode of insert	*/
	if (cv->Imode != PushMode)
		disp_mode ();

	/* recover correct span point	*/
	if (cv->span_point > 0 &&
	    cv->span_p[cv->span_point] > &(cv->Halpha[cv->ha_point]))
		cv->span_point --;
}

ChangeMode (mode)
int	mode;
{
	register Conversion	*cv;

	cv = GetConversion ();
	if (cv->Imode != mode) {
		cv->Jmode = mode;
		save_span (SPAN_CHMOD);
		cv->Imode = mode;
		return (1);
	}
	else
		return (0);
}

exec_mode (key)
register int	key;
{
	register Conversion	*cv;
	extern int	def_code;
	register int	mode;
	int		changed;

	cv = GetConversion ();
	if (key == KEY_MUHENKAN)
		key = eval_muhenkan ();
	switch (key) {
		case KEY_CODE:
			if (cv->Imode == MODE_SCODE)
				ChangeMode (MODE_ECODE);
			else if (cv->Imode == MODE_ECODE)
				ChangeMode (MODE_JCODE);
			else if (cv->Imode == MODE_JCODE)
				ChangeMode (MODE_KCODE);
			else if (cv->Imode == MODE_KCODE)
				ChangeMode (MODE_SCODE);
			else
				ChangeMode (def_code);
			ChangeCode (cv->Imode);
			mode = MODE_HALPHA;
			changed = 1;
			break;
		case KEY_HALPHA:
		case KEY_ZALPHA:
		case KEY_HKATA:
		case KEY_ZKATA:
		case KEY_ZHIRA:
		default:
			mode = eval_mode (key);
			changed = ChangeMode (mode);
			break;
	}
	return (changed);
}

toggle_mode (mod)
register int	mod;
{
	register int	value;

	switch (mod) {
		case MODE_ZHIRA:
			value = KEY_ZKATA;
			break;
		case MODE_ZKATA:
			value = KEY_HKATA;
			break;
		case MODE_HKATA:
			value = KEY_ZALPHA;
			break;
		case MODE_ZALPHA:
			value = KEY_HALPHA;
			break;
		case MODE_HALPHA:
		case MODE_SCODE:
		case MODE_ECODE:
		case MODE_JCODE:
		case MODE_KCODE:
		default:
			value = KEY_ZHIRA;
			break;
	}
	return (value);
}

eval_mode (key)
register int	key;
{
	register Conversion	*cv;
	int		mod;

	cv = GetConversion ();
	switch (key) {
		case KEY_ZHIRA:
			mod = MODE_ZHIRA;
			break;
		case KEY_ZKATA:
			mod = MODE_ZKATA;
			break;
		case KEY_HKATA:
			mod = MODE_HKATA;
			break;
		case KEY_ZALPHA:
			mod = MODE_ZALPHA;
			break;
		case KEY_HALPHA:
		case KEY_CODE:
		default:
			mod = MODE_HALPHA;
			break;
	}
	return (mod);
}

IsESC (c)
u_char	c;
{
	if (c == ESC)
		return (1);
	else
		return (0);
}

Strncpy (d, s, len)
u_char 	*d, *s;
int	len;
{
	if ((len > 0) && (s != (u_char *)NULL))
		return (strncpy (d, s, len));
}

is_bsdel (c)
register int	c;
{
	if (is_bs (c) || is_del (c))
		return (1);
	else
		return (0);
}

static int	bskey = BS;
static int	delkey = DEL;

clear_bsdel ()
{
	bskey = BS;
	delkey = DEL;
}

set_bs (c)
int	c;
{
	bskey = c;
}

set_del (c)
int	c;
{
	delkey = c;
}

is_bs (c)
register int	c;
{
	if (c == bskey)
		return (1);
	else
		return (0);
}

is_del (c)
register int	c;
{
	if (c == delkey)
		return (1);
	else
		return (0);
}

#ifdef SJX
Sdisp (redraw)
{
	register Conversion	*cv;
	register int	i;
	register int	mod;
	char		*s;
	int		len;

	cv = GetConversion ();
	Cclear (redraw);		/* sjx: redraw 0	*/
	SetVcol (0);
	for (i = 0 ; i < cv->span_point ; i ++) {
		mod = load_span (i, &s, &len);
		if (len)
			vprintU (s, mod);
	}
	mod = load_spanning (i, &s, &len);
	if (len)
		vprintU (s, mod);
}
#endif /* SJX */

static Conversion	*saved_conversion;

SaveConversion ()
{
	saved_conversion = GetConversion ();
}

AnotherConversion ()
{
	if (saved_conversion == GetConversion ())
		return (0);
	else
		return (1);
}

EisuuSet ()
{
	if (oasys)
		sj_atbl_set1 ();
}

KanaSet ()
{
	if (oasys)
		sj_atbl_set2 ();
}

