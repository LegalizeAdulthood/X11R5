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
 *	edit.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Sun Jul 26 01:18:13 JST 1987
 */
/*
 * $Header: edit.c,v 1.1 91/04/29 17:56:01 masaki Exp $ SONY;
 */

#include "common.h"
#include "key.h"

/* push and pop Object info	*/

static u_char	*jpoint;
static int	iedit, jedit;
static int	ilen, jlen;

push_obj (i, j)
register int	i, j;
{
	register Conversion	*cv;

	cv = GetConversion ();
	jpoint = cv->Ppoint[j];
	ilen = cv->Plen[i];
	jlen = cv->Plen[j];
	iedit = cv->Bkettei[i];
	jedit = cv->Bkettei[j];
}

pop_obj (i, j)
register int	i, j;
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->Ppoint[j] = jpoint;
	cv->Plen[i] = ilen;
	cv->Plen[j] = jlen;
	cv->Bkettei[i] = iedit;
	cv->Bkettei[j] = jedit;
}

/* bunsetu kakucyou		*/

Bbig ()
{
	register Conversion	*cv;
	register int		i, j, k, moji;
	int			val;

	cv = GetConversion ();
	if (cv->CurBun < 0)
		cv->CurBun = 0;
	i = cv->CurBun;
	j = i + 1;
	while (j < cv->Bnum) {
		if (cv->Plen[j] > 0) {
			push_obj (i, j);

			moji = cv->HAindex[cv->Ppoint[j] - cv->Ppoint[0]];
			cv->Plen[i] += cv->Amojilen[moji];
			cv->Plen[j] -= cv->Amojilen[moji];
			cv->Ppoint[j] = cv->Ppoint[i] + cv->Plen[i];

			cv->PreBun = j;

			if (cv->Bkettei[i] & Bhenkan) {
				val = Sdouon (0);
				/* 0: No Choice	*/
			}
			else {
				cv->Bkettei[i] |= Bmuhenkan;
				cv->Bkettei[i] &= ~Bhenkan;
				cv->Bkettei[j] |= Bmuhenkan;
				cv->Bkettei[j] &= ~Bhenkan;
				val = 0;
			}
			if (!val) {
				cv->Dflag = -1;
				Rdisp ();
				return (1);
			}
			break;
		}
		else
			j ++;
	}
	beep ();
	if (j < cv->Bnum)
		pop_obj (i, j);
	return (0);
}

/* bunsetu syukusyou		*/

Bsmall (small)
int	small;		/* 1 iff continued small or big key, else 0.	*/
{
	register Conversion	*cv;
	register int	i, j, k, moji;
	int		val;

	cv = GetConversion ();
	if (cv->CurBun < 0)
		cv->CurBun = 0;
	i = cv->CurBun;
	j = i + 1;
	if (j <= cv->Bnum && cv->Plen[i] > 1) {
top:
		if (j == cv->Bnum) {
			cv->Bnum ++;
			cv->Bkettei[cv->Bnum] = Bend;
			cv->Plen[j] = 0;
			cv->Bkettei[j] |= Bmuhenkan;
			cv->Bkettei[j] &= ~Bhenkan;
		}
		else if (small) {
			while (j < cv->Bnum - 1 && cv->Plen[j] == 0)
				j ++;
			if (j == cv->Bnum)
				j --;
		}
		else if (cv->Plen[j] != 0 && cv->Enum < MAXENUM) {
			for (k = cv->Bnum ; k > j ; k --) {
				cv->Plen[k] = cv->Plen[k - 1];
				cv->Ppoint[k] = cv->Ppoint[k - 1];
				cv->Pkettei[k] = cv->Pkettei[k - 1];

				cv->Bspan[k] = cv->Bspan[k - 1];
				cv->Bpoint[k] = cv->Bpoint[k - 1];
				cv->Bkettei[k] = cv->Bkettei[k - 1];

				cv->Displen[k] = cv->Displen[k - 1];
				cv->Dispmod[k] = cv->Dispmod[k - 1];

				cv->Edit[k] = cv->Edit[k - 1];
			}

			cv->Bnum ++;

			cv->Plen[j] = 0;
			cv->Bkettei[j] = Bmuhenkan;
			cv->Displen[j] = 0;
		}
		else if (cv->Plen[j] != 0) {
			while (j < cv->Bnum - 1 && cv->Plen[j] == 0)
				j ++;
			if (j == cv->Bnum)
				j --;
		}
		else {
			while (j < cv->Bnum - 1 && cv->Plen[j] == 0 && cv->Plen[j+1] == 0)
				j ++;
		}

		push_obj (i, j);

		moji = cv->HAindex[cv->Ppoint[i] - cv->Ppoint[0] + cv->Plen[i] - 1];
		cv->Plen[i] -= cv->Amojilen[moji];
		cv->Plen[j] += cv->Amojilen[moji];
		cv->Ppoint[j] = cv->Ppoint[i] + cv->Plen[i];

		if (cv->Plen[i] < 1) {
			beep ();
			pop_obj (i, j);
			return;
		}

		cv->PreBun = j;

		if (cv->Bkettei[i] & Bhenkan) {
			val = Sdouon (0);
			/* 0: No Choice	*/
		}
		else {
			cv->Bkettei[i] |= Bmuhenkan;
			cv->Bkettei[i] &= ~Bhenkan;
			cv->Bkettei[j] |= Bmuhenkan;
			cv->Bkettei[j] &= ~Bhenkan;
			val = 0;
		}
		if (!val) {
			cv->Dflag = -1;
			Rdisp ();
			return (1);
		}
		beep ();
		pop_obj (i, j);
	}
	else {
		beep ();
	}
	return (0);
}

/* bunsetu syoukyo		*/

Bdelete ()
{
	register Conversion	*cv;
	register int	i, j, deletelen, val;

	cv = GetConversion ();
	if (cv->CurBun < 0) {
		cv->CurBun = 0;
		return;
	}
	i = cv->CurBun;
	j = i + 1;
	if (j <= cv->Bnum && cv->Plen[i] > 0) {
		while (j < cv->Bnum && cv->Plen[j] == 0)
			j ++;
		if (j == cv->Bnum) {
			j = i - 1;
			while (j >= 0 && cv->Plen[j] == 0)
				j --;
			if (j == -1)
				j = i;
		}

		push_obj (i, j);

		deletelen = cv->Plen[i];
		cv->Plen[i] = 0;

		cv->PreBun = i;
		cv->CurBun = j;
			/* 0: No Choice	*/
		val = Sdouon (0);
		if (!val) {
			cv->Dflag = -1;
			Chain (i, deletelen);
			Rdisp ();
			return;
		}
	}
	else {
		beep ();
		all_set_up ();
		return;
	}
	beep ();
	pop_obj (i, j);
}

/* chain after deleted bunsetu	*/

Chain (n, len)
register int	n;	/* deleted bunsetu number	*/
register int	len;	/* deleted length		*/
{
	register Conversion	*cv;
	register int		i, mlen;
	register u_char		*p;
	register u_short	*pp;

	cv = GetConversion ();

	if (cv->Ppoint[n+1] == 0)
		return;

	mlen = strlen (cv->Ppoint[n+1]);
	p = cv->Ppoint[n];
	for (i = 0 ; i < mlen ; i ++)
		p[i] = p[i+len];
	p[mlen] = '\0';
	for (i = n + 1 ; i < cv->Bnum ; i ++)
		cv->Ppoint[i] -= len;

	pp = &(cv->HAindex[cv->Ppoint[n] - cv->Ppoint[0]]);
	for (i = 0 ; i < mlen ; i ++)
		pp[i] = pp[i+len];
	pp[mlen] = '\0';
	cv->ha_point -= len;
}

exec_edit ()
{
	register Conversion	*cv;
	u_char			Htmp[BUFFLENGTH], Ztmp[BUFFLENGTH];
	int			bun;
	int			edited, editlen;
	int			len, diff;
	int			Hcol, Zcol;
	register int		edit, inc, i;
	extern int		keyvalue;
	u_char			number[3];
	int			num, numpoint;
	register u_char		*s;

	cv = GetConversion ();
	bun = cv->CurBun;
	len = cv->Plen[bun];
	Strncpy (Htmp, cv->Ppoint[bun], len);
	Htmp[len] = '\0';
	HAtoZA (Htmp, Ztmp);
	TopEdit ();
	SJ_print (Ztmp);
	Clear_EOL ();
	backspace (strlen (Ztmp));
	Hcol = Zcol = 0;
#ifdef SJX
	CursorOn (&Ztmp[Zcol]);
#endif /* SJX */
	Flush ();

	/* edit loop */
	edited = 0;
	edit = 1;
	numpoint = 0;
	SaveConversion ();
	while (edit) {
		inc = inkey ();
		cv = GetConversion ();
		if (AnotherConversion ()) {
			unget_key (inc, keyvalue);
			edited = 0;
#ifdef SJX
			PopGotoTxtwin ();
#endif /* SJX */
			break;
		}
		if (keyvalue == KEY_CONV)
			return (0);
		if (keyvalue == KEY_HENKAN || keyvalue == KEY_KETTEI)
			break;
		else if (keyvalue == KEY_UP)
			inc = 'a';
		else if (keyvalue == KEY_DOWN)
			inc = 'i';
		else if (keyvalue == KEY_RIGHT)
			inc = 'l';
		else if (keyvalue == KEY_LEFT)
			inc = 'h';
		switch (inc) {
			case EOF:
			case ESC:
				edit = 0;
				break;
			case 'l':
			case ' ':
				num = getnum (number, &numpoint);
				if (Hcol >= len - 1) {
					beep ();
					break;
				}
				else if (iskanji(Htmp[Hcol])
					&& Hcol >= len - 2) {
					beep ();
					break;
				}
#ifdef SJX
				CursorOff ();
#endif /* SJX */
				for (i = 0 ; i < num  && Hcol < len - 1 ; i ++)
					Zforw (Htmp, Ztmp, &Hcol, &Zcol);
#ifdef SJX
				CursorOn (&Ztmp[Zcol]);
#endif /* SJX */
				Flush ();
				break;
			case 'h':
			case BS:
				num = getnum (number, &numpoint);
				if (Hcol <= 0) {
					beep ();
					break;
				}
#ifdef SJX
				CursorOff ();
#endif /* SJX */
				for (i = 0 ; i < num && Hcol > 0 ; i ++)
					Zback (Htmp, &Hcol, &Zcol);
#ifdef SJX
				CursorOn (&Ztmp[Zcol]);
#endif /* SJX */
				Flush ();
				break;
			case '\014':	/* control-L	*/
			case '$':
				num = getnum (number, &numpoint);
				if (len <= 0)
					break;
				Hcol = len - 1;
				Zcol = Zcal (Htmp, Hcol);
				TopEdit ();
				SJ_print (Ztmp);
				Clear_EOL ();
				backspace (2);
#ifdef SJX
				CursorOn (&Ztmp[Zcol]);
#endif /* SJX */
				Flush ();
				break;
			case 'a':
				if (len > 0) {
					Zforw (Htmp, Ztmp, &Hcol, &Zcol);
					Flush ();
				}
#ifdef SJX
				CursorOn (0);
#endif /* SJX */
				/* fall into next entry		*/
			case 'i':
				num = getnum (number, &numpoint);
				if (editlen = Insert (&Htmp[Hcol])) {
					len += editlen;
					Hcol += editlen;
					Zcol = Zcal (Htmp, Hcol);
					HAtoZA (Htmp, Ztmp);
					edited = 1;
				}
				if (Hcol > 0)
					Zback (Htmp, &Hcol, &Zcol);
				Print_EOL (&Ztmp[Zcol]);
#ifdef SJX
				CursorOn (&Ztmp[Zcol]);
#endif /* SJX */
				Flush ();
				break;
			case DEL:
			case 'x':
				num = getnum (number, &numpoint);
				if (len <= 0) {
					beep ();
					break;
				}
				len -= EditDel (&Htmp[Hcol], &Ztmp[Zcol], num);
				if (Hcol >= len && Hcol > 0)
					Zback (Htmp, &Hcol, &Zcol);
				Print_EOL (&Ztmp[Zcol]);
#ifdef SJX
				CursorOn (&Ztmp[Zcol]);
#endif /* SJX */
				Flush ();
				edited = 1;
				break;
			case 'r':
				num = getnum (number, &numpoint);
				if (len <= 0) {
					beep ();
					break;
				}
				inc = inkey ();
				cv = GetConversion ();
				if (AnotherConversion ()) {
					unget_key (inc, keyvalue);
					edited = 0;
#ifdef SJX
			PopGotoTxtwin ();
#endif	/* SJX */
					break;
				}
				if (inc>=' ' && inc<='~' || iskana (inc)) {
					Htmp[Hcol] = inc;
					HAtoZA (Htmp, Ztmp);
					Zstay (&Ztmp[Zcol]);
#ifdef SJX
					CursorOn (&Ztmp[Zcol]);
#endif /* SJX */
					Flush ();
					edited = 1;
				}
				else
					beep ();
				break;
			case 's':
				num = getnum (number, &numpoint);
				if (len <= 0) {
					beep ();
					break;
				}
				Hcol += Substitute (&Htmp[Hcol], &Ztmp[Zcol],
						num, &len);
				Zcol = Zcal (Htmp, Hcol);
				if (Hcol > 0)
					Zback (Htmp, &Hcol, &Zcol);
				Print_EOL (&Ztmp[Zcol]);
#ifdef SJX
				CursorOn (&Ztmp[Zcol]);
#endif /* SJX */
				Flush ();
				edited = 1;
				break;
			case '0':
				if (numpoint == 0) {
					num = getnum (number, &numpoint);
					TopEdit ();
					Flush ();
					Hcol = Zcol = 0;
					break;
				}
				/* fall into next entry		*/
			default:
				if (isdigit (inc)) {
					if (numpoint > 1) {
						/* over flow		*/
						/* two digit limit	*/
						number[0] = number[1];
						number[1] = inc;
					}
					else
						number[numpoint ++] = inc;
				}
				else {
					num = getnum (number, &numpoint);
					beep ();
				}
				break;
		}
	}

	if (edited) {
		s = cv->Ppoint[bun];
		s += cv->Plen[bun];
		if (*s)
			strcat (Htmp, s);
		strcpy (cv->Ppoint[bun], Htmp);
		if ((diff = len - cv->Plen[bun]) != 0) {
			cv->Plen[bun] = len;
			cv->ha_point += diff;
			for (i = bun + 1 ; i < cv->Bnum ; i ++)
				cv->Ppoint[i] += diff;
		}
		ModifyIndex (Htmp, bun, diff);
		cv->Pedited[bun] = 1;	/* yes, edited		*/
		cv->Dflag = -1;
		return (1);		/* yes, edited		*/
	}
	return (0);			/* no, not edited	*/
}

/*	calc Zcol from Htmp and Hcol		*/

Zcal (s, col)
register u_char	*s;
int		col;
{
	register int	i, j;

	i = 0;
	j = 0;
	while (i < col) {
		if (iskanji (s[i++])) {
			i ++;
		}
		j += 2;
	}
	return (j);
}

/*	print forward by one zenkaku char	*/

Zforw (Hs, Zs, Hcol, Zcol)
u_char	*Hs, *Zs;
int	*Hcol, *Zcol;
{
	u_char	tmp[3];

	sprintf (tmp, "%c%c", Zs[*Zcol], Zs[*Zcol + 1]);
	SJ_print (tmp);
	if (iskanji (Hs[(*Hcol)++]))
		(*Hcol)++;
	(*Zcol) += 2;
}

/*	print backward by one zenkaku char	*/

Zback (Hs, Hcol, Zcol)
register u_char	*Hs;
register int	*Hcol, *Zcol;
{
	register int	i, kanji;

	backspace (2);
	for (i = 0 , kanji = 0 ; i < *Hcol ; ) {
		if (iskanji (Hs[i++])) {
			i ++;
			kanji = 1;
		}
		else
			kanji = 0;
	}
	(*Hcol)--;
	if (kanji)
		(*Hcol)--;
	(*Zcol) -= 2;
}

/*	print one zenkaku char and stay there	*/

Zstay (s)
u_char	*s;
{
	u_char	tmp[3];

	sprintf (tmp, "%c%c", s[0], s[1]);
	SJ_print (tmp);
	backspace (2);
}

ModifyIndex (Hs, bun, adiff)
u_char		*Hs;
register int	bun;
int		adiff;
{
	register Conversion	*cv;
	register int	i, j, base;
	int		diff;
	int		basemoji, num;
	int		buff[BUFFLENGTH];

	cv = GetConversion ();
	/* modify index		*/

	num = 0;
	j = cv->Ppoint[bun] - cv->Ppoint[0];
	base = cv->HAindex[j];
	basemoji = base;			/* use at moji modify	*/
	if (bun + 1 < cv->Bnum) {
		j = cv->Ppoint[bun + 1] - cv->Ppoint[0];
		num = cv->HAindex[j - adiff] - basemoji;
						/* use at moji modify	*/
		for (i = j ; i < cv->ha_point ; i ++)
			buff[i] = cv->HAindex [i - adiff];
						/* back up		*/
	}
	j = cv->Ppoint[bun] - cv->Ppoint[0];
	for (i = 1 ; i < cv->Plen[bun] ; i ++)
		cv->HAindex [j + i] = ++base;
	if (bun + 1 < cv->Bnum) {
		j = cv->Ppoint[bun + 1] - cv->Ppoint[0];
		diff = ++base - buff[j];
		for (i = j ; i < cv->ha_point ; i ++)
			cv->HAindex[i] = buff[i] + diff;
						/* restore		*/
	}

	/* modify moji count	*/

	if (bun + 1 < cv->Bnum) {
		for (i = basemoji + num ; i < cv->moji_count ; i ++)
			buff[i] = cv->Amojilen[i];
	}
	for (i = 0 , j = 0 ; j < cv->Plen[bun] ; i ++) {
		cv->Amojilen[basemoji + i] = 1;
		if (iskanji (Hs[j++]))
			j++;
	}
	if (bun + 1 < cv->Bnum) {
		j = cv->Plen[bun] - num;
		for (i = basemoji + num ; i < cv->moji_count ; i ++)
			cv->Amojilen[i + j] = buff[i];
		cv->moji_count += j;
	}
}

getnum (number, numpoint)
u_char	number[];
int	*numpoint;
{
	if (*numpoint == 0)
		return (1);
	number[*numpoint] = '\0';
	*numpoint = 0;
	return (atoi (number));
}

Substitute (Htmp, Ztmp, num, len)
register u_char	*Htmp, *Ztmp;
register int	num, *len;
{
	u_char		work[BUFFLENGTH];
	u_char		work2[BUFFLENGTH];
	register int	i, j, diff;

	i = 0;
	j = 0;
	while (Htmp[j] != '\0') {
		i++;
		if (iskanji (Htmp[j++]))
			j++;
	}
	if (i >= num)
		diff = num - 1;
	else
		diff = i - 1;
	if (diff) {
		diff *= 2;
		Strncpy (work, Ztmp, diff);
	}
	work[diff] = '\0';
	sprintf (work2, "%s$$", work);
	SJ_print (work2);
	backspace (diff + 2);
#ifdef SJX
	CursorOn (0);
#endif /* SJX */
	Flush ();
	*len -= EditDel (Htmp, Ztmp, num);
	if (diff = Insert (Htmp)) {
		*len += diff;
		HAtoZA (Htmp, Ztmp);
	}
	Flush ();
	return (diff);
}

Insert (s)
register u_char	*s;
{
	int		editlen;
	u_char		tmp[BUFFLENGTH], Ztmp[3];
	u_char		atr[BUFFLENGTH];
	register u_char	*p, *q;
	register int	inc, inedit;
	extern int	keyvalue;

	p = tmp;
	*p = '\0';
	q = atr;
	*q = 0;
	inedit = 1;
	SaveConversion (GetConversion ());
	while (inedit) {
		inc = inkey ();
		if (AnotherConversion (GetConversion ())) {
			unget_key (inc, keyvalue);
			*tmp = '\0';
#ifdef SJX
			PopGotoTxtwin ();
#endif	/* SJX */
			break;
		}
		switch (inc) {
			case ESC:
				inedit = 0;
				break;
			case BS:
			case DEL:
				if (p == tmp) {
					beep ();
					break;
				}
#ifdef SJX
				CursorOff (0);
#endif /* SJX */
				backspace (2);
#ifdef SJX
				CursorOn (0);
#endif /* SJX */
				Flush ();
				p --;
				*p = '\0';
				q --;
				if (*q == 1) {	/* iskanji	*/
					p --;
					*p = '\0';
					*(q --) = 0;
				}
				*q = 0;
				break;
			default:
				if (!isprint (inc) && !iskana (inc)
					&& !iskanji (inc)) {
					beep ();
					break;
				}
				*p = inc;
				if (iskanji (inc)) {
					*(p + 1) = inkey ();
					*(p + 2) = '\0';
				}
				else
					*(p + 1) = '\0';
				HAtoZA (p, Ztmp);
				SJ_print (Ztmp);
#ifdef SJX
				CursorOn (0);
#endif /* SJX */
				Flush ();
				if (iskanji (inc)) {
					p += 2;
					*(q ++) = 1;	/* iskanji	*/
					*(q ++) = 1;
					*q = 0;
				}
				else {
					p ++;
					*(q ++) = 0;
					*q = 0;
				}
				break;
		}
	}
	editlen = strlen (tmp);
	if (editlen) {
		if (*s != '\0')
			strcat (tmp, s);
		strcpy (s, tmp);
	}
	return (editlen);
}

EditDel (Hs, Zs, num)
register u_char	*Hs, *Zs;
register int	num;
{
	u_char		work[BUFFLENGTH];
	register int	i, j;

	i = 0;
	j = 0;
	while (Hs[j] != '\0') {
		i++;
		if (iskanji (Hs[j++]))
			j++;
	}
	if (i >= num) {
		j = 0;
		while (num --) {
			if (iskanji (Hs[j++]))
				j++;
		}
		strcpy (work, &Hs[j]);
		strcpy (Hs, work);
		HAtoZA (Hs, Zs);
		return (j);
	}
	else {
		*Hs = '\0';
		*Zs = '\0';
		return (j);
	}
}

Print_EOL (s)
register u_char	*s;
{
	if (*s != '\0')
		SJ_print (s);
	Clear_EOL ();
	if (*s != '\0')
		backspace (strlen (s));
}

backspace (n)
int	n;
{
	register int	i;

	for (i = 0 ; i < n ; i ++)
		Backspace ();
}

TopEdit ()
{
	extern u_char	*Gedit;

	TopGuide ();
	spaceR (Gedit);
}
