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
 *	douon.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi.
 *	Wed Jul  8 16:06:08 JST 1987
 */
/*
 * $Header: douon.c,v 1.1 91/04/29 17:56:00 masaki Exp $ SONY;
 */

#include "common.h"
#include "key.h"

#define DNUM		5		/* number of douon display	*/

static int		Dcount;
static int		Dpoint;
static struct douon	HenkanDouon[DOUON_N];
static char		Dkanji[BUFFLENGTH * 2];

SJ_getdouon2 (s, d, n, choice, reconv)
char		*s;
struct douon	*d;
int		n;
int		choice;
int		reconv;
{
	if (SJ2_getdouon2 (s, d, n, choice, reconv) == 0) {
		SJ_getdsub (s, d);
	}
}

SJ_getdouon (s, d, n)
char		*s;
struct douon	*d;
int		n;
{
	Dcount = SJ2_getdouon (s, d, n, 0, 0);
	if (Dcount == 0) {
		Dcount = 1;
		SJ_getdsub (s, d);
		return (0);		/* gakusyuu not ok	*/
	}
	else {
		return (1);		/* gakusyuu ok		*/
	}
}

SJ_getdsub (s, d)
char		*s;
struct douon	*d;
{
	register int	len;
	len = strlen (s);
	Strncpy ((char *)d[0].ddata, s, len);
	d[0].dlen = len;
}

/* now douon search		*/

exec_douon ()
{
	register Conversion	*cv;
	int	val;

	cv = GetConversion ();

	if (cv->CurBun == -1) {
		cv->CurBun = 0;
		Rdisp ();
	}

	cv->PreBun = cv->CurBun;
	if (cv->Bkettei[cv->CurBun] & Bmuhenkan)
		val = Sdouon (2);
	else
		val = Sdouon (1);		/* 1: Choice douon	*/
	if (!val)
		Rdisp ();
}

wrap_douon (back)
int	back;	/* 1: wrap back		*/
{
	register Conversion	*cv;
	int	val;

	cv = current_conversion;

	if (cv->CurBun == -1) {
		cv->CurBun = 0;
		Rdisp ();
	}

	cv->PreBun = cv->CurBun;
	if (cv->Dflag == cv->CurBun)
		val = Rdouon (back);
	else {
		val = Sdouon (2);
		cv->Dflag = cv->CurBun;
	}
	if (!val)
		Rdisp ();
}

/* search and display douon	*/

Cdouon (yomi, gnum)
char	*yomi;
int	gnum;
{
	/*
	 * douon get and choice
	 */
	register int	i, j;
	register int	len;
	register char	*s;
	register int	dispnum;
	int		num;
	int		displen;
	int		base;
	char		tmp[BUFFLENGTH], tmp2[BUFFLENGTH * 2];
	int		gakusyuu;
	u_short		row, col;
	register Conversion	*cv;

	cv = current_conversion;

	gakusyuu = SJ_getdouon (yomi, HenkanDouon, gnum);

	dispnum = DNUM;
	if (dispnum > Dcount)
		j = Dcount;
	else
		j = dispnum;
	len = 0;
	for (i = 0 ; i < j ; i ++)
		len += HenkanDouon[i].dlen;
	len /= j;
	if (len < 1)
		len = 1;

	len += 6;
	sprintf (tmp, "%d/%d ", Dcount, Dcount);

#ifdef SJX
	StartGuide (&row, &col);
	displen = cv->column - strlen (tmp) - 1;
	EndGuide (&row, &col);
#else /* SJX */
	displen = cv->column - strlen (tmp) - 1;
#endif /* SJX */

	if (displen < 1)
		displen = 1;
	num = displen / len;
	if (num < 1)
		num = 1;
	if (num < dispnum && num > 0)
		dispnum = num;

	for (i = 0 ; ; ) {
		if (i >= Dcount)
			i = 0;
		StartGuide (&row, &col);
		Clear_EOL ();
		base = i;

		sprintf (tmp, "%d/%d", i + 1, Dcount);
		printR (tmp);
		put_space (1);

		for (j = 0 ; j < dispnum ; j ++, i ++) {
			if (i >= Dcount)
				break;
			len = HenkanDouon[i].dlen;

			if (len > displen)
				len = displen;

			s = (char *)HenkanDouon[i].ddata;
			Strncpy (tmp, s, len);
			tmp[len] = '\0';
			sprintf (tmp2, "[%d] %s ", j+1, tmp);
			SJ_print (tmp2);
		}

		EndGuide (row, col);
		Flush ();
		num = Dselect (j);	/* j == just dispnum or less	*/

		/* forward	*/
		if (num == 0)
			continue;

		/* reverse	*/
		else if (num == -1) {
			if ((i >= Dcount) && ((Dcount % dispnum) != 0))
				i = Dcount - (Dcount % dispnum);
			else
				i -= dispnum;
			i -= dispnum;
			if (i < 0) {
				if (Dcount > dispnum) {
					if ((Dcount % dispnum) == 0)
						i = Dcount - dispnum;
					else
						i = Dcount - (Dcount % dispnum);
				}
				else
					i = 0;
			}
			continue;
		}

		/* no select	*/
		else if (num > j) {
			disp_mode ();
			return (FALSE);
		}

		/* select	*/
		else
			break;
	}

	disp_mode ();

	Dpoint = base + num - 1;
	len = HenkanDouon[Dpoint].dlen;
	s = (char *)HenkanDouon[Dpoint].ddata;
	Strncpy (Dkanji, s, len);
	Dkanji[len] = '\0';
	PushGD (gnum, &HenkanDouon[Dpoint].dcid);

	if (gakusyuu)
		SJ2_study (&HenkanDouon[Dpoint].dcid);

	return (TRUE);
}

Dselect (lim)
register int	lim;
{
	extern int	keyvalue;
	register int	c, num;
	char		s[2];

	SaveConversion ();
	while ((c = inkey ()) != EOF) {
		if (AnotherConversion ()) {
			unget_key (c, keyvalue);
			break;
		}
		if (keyvalue == KEY_KETTEI)
			break;			/* no select		*/
		else if (keyvalue == KEY_HENKAN || keyvalue == KEY_RIGHT)
			return (0);		/* select continue	*/
		else if (c == ' ')
			return (0);
		else if (keyvalue == KEY_MUHENKAN || keyvalue == KEY_LEFT
			|| is_bsdel (c))
			return (-1);		/* back search		*/
		else if (isdigit (c)) {
			*s = c;
			s[1] = '\0';
			num = atoi (s);
			if (num > 0 && num <= lim)
				return (num);	/* return selected	*/
		}
		else if (keyvalue != KEY_NORMAL)
			break;			/* no select		*/
		else if (keyvalue == KEY_NORMAL && IsESC (c))
			break;
	}
	return (lim + 1);			/* no select		*/
}


Gdouon2 (yomi, num, choice, reconv)
char	*yomi;
int	num;
int	choice;
int	reconv;
{
	/*
	 * Get Douon
	 */
	register int	len;
	register char	*s;

	SJ_getdouon2 (yomi, HenkanDouon, num, choice, reconv);

	Dpoint = 0;
	len = HenkanDouon[Dpoint].dlen;
	s = (char *)HenkanDouon[Dpoint].ddata;
	Strncpy (Dkanji, s, len);
	Dkanji[len] = '\0';
	PushGD (num, &HenkanDouon[Dpoint].dcid);
}

Gdouon (yomi, num)
char	*yomi;
int	num;
{
	/*
	 * Get Douon
	 */
	register int	len;
	register char	*s;

	SJ_getdouon (yomi, HenkanDouon, num);

	Dpoint = 0;
	len = HenkanDouon[Dpoint].dlen;
	s = (char *)HenkanDouon[Dpoint].ddata;
	Strncpy (Dkanji, s, len);
	Dkanji[len] = '\0';
	PushGD (num, &HenkanDouon[Dpoint].dcid);
}

Sdouon (Choice)
int Choice;
{
	register Conversion	*cv;
	register int	i, j;
	register int	k;
	int		len;
	char		*s;
	char		yomi[BUFFLENGTH];
	int		count;

	cv = current_conversion;
	count = 0;
	if (Choice == 0)
		i = cv->PreBun;
	else
		i = cv->CurBun;
top:
	Bgetsrc (i, &s, &len);
	Strncpy (yomi, s, len);
	yomi[len] = '\0';
	if (len <= 0) {
		cv->Bkettei[i] |= Bmuhenkan;
		cv->Bkettei[i] &= ~Bhenkan;

		cv->Pkettei[i] = MODE_ZHIRA;

		*Dkanji = '\0';
		goto null;
	}
	else if (len >= DOUONLEN) {
		cv->Bkettei[i] |= Bmuhenkan;
		cv->Bkettei[i] &= ~Bhenkan;
		*Dkanji = '\0';
		goto null;
	}
	else if (Choice == 0 && (cv->Bkettei[i] & Bmuhenkan)) {
		goto tail;
	}
	else {
		cv->Bkettei[i] |= Bhenkan;
		cv->Bkettei[i] &= ~Bmuhenkan;
	}

	if (Choice == 1) {
		if (Cdouon (yomi, i) == FALSE)
			return (1);
		cv->Pkettei[i] = MODE_ZHIRA;
	}
	else if (Choice == 2) {
		Gdouon (yomi, i);
	}
	else if (Choice == 3)
		Gdouon2 (yomi, i, count, 1);	/* 1: reconvert	*/
	else
		Gdouon2 (yomi, i, count, 0);
null:
	len = strlen (Dkanji);

	if (!(cv->Bkettei[i] & Bedit)) {
		if (cv->Enum >= MAXENUM)
			return (1);
		j = cv->Enum ++;
		cv->Edit[i] = j;
		cv->Epoint[j] = &(cv->Ebuff[cv->e_point]);
		cv->e_point += DLEN;
	}
	else
		j = cv->Edit[i];
	if (0 < len && len <= DOUONLEN*2) {
		cv->Bkettei[i] |= Bedit;
		cv->Pkettei[i] = MODE_ZHIRA;
	}
	cv->Elen[j] = len;
	strcpy (cv->Epoint[j], Dkanji);

tail:
	if (Choice == 0 && i != cv->CurBun) {
		i = cv->CurBun;
		goto top;
	}

	if (cv->PreBun == cv->Bnum)
		cv->Bkettei[cv->Bnum] = Bend;
	return (0);
}

/* re-display douon		*/

Rdouon (back)
int	back;	/* 1: wrap back	0: forward	*/
{
	register Conversion	*cv;
	register int	i, j;
	register int	len;
	register char	*s;

	cv = current_conversion;
	if (Dcount > 0) {
		if (back) {
			Dpoint --;
			if (Dpoint < 0)
				Dpoint = Dcount - 1;
		}
		else {
			Dpoint ++;
			if (Dpoint >= Dcount)
				Dpoint = 0;
		}
		len = HenkanDouon[Dpoint].dlen;
		s = (char *)HenkanDouon[Dpoint].ddata;
		Strncpy (Dkanji, s, len);
		Dkanji[len] = '\0';

		i = cv->CurBun;
		j = cv->Edit[i];
		cv->Elen[j] = len;
		strcpy (cv->Epoint[j], Dkanji);
		return (0);
	}
	return (1);
}

Bkanji (kanji)
char	*kanji;
{
	register Conversion	*cv;
	register int	i, j;
	int		len;
	register char	*s;
	char		*Bstr ();

	cv = current_conversion;
	*kanji = '\0';
	for (i = 0 ; i < cv->Bnum ; i ++) {
		if ((s = Bstr (i, &len)) == (char *)0 || len == 0)
			continue;
		strncat (kanji, s, len);
	}
}

struct studyrec	Gdata[BUFFLENGTH];

PushGD (num, wordid)
int		num;
struct studyrec	*wordid;
{
	Gdata[num] = *wordid;
}

cl_gakusyuu ()
{
	register Conversion	*cv;
	register int	i, j;
	register int	o_point, n_point;

	cv = current_conversion;
	for (i = 0 ; i < cv->Bnum ; i ++) {
		if (cv->OrgPlen[i] != cv->Plen[i]) {
			o_point = n_point = 0;
			j = i;
			while (j < cv->Bnum) {
				o_point += cv->OrgPlen[j];
				n_point += cv->Plen[j];
				j ++;
				if (o_point == n_point)
					break;
			}
			j --;
			check_and_gakusyuu (i, j);
			i = j;
		}
	}
}

check_and_gakusyuu (m, n)
int	m, n;
{
	register Conversion	*cv;
	register int	i, j;

	cv = current_conversion;
	i = m;
	j = i + 1;
	while (j <= n && j < cv->Bnum) {
		if (cv->Bkettei[i] & Bedit && !(cv->Bkettei[i] & Bmuhenkan) &&
		    cv->Bkettei[j] & Bedit && !(cv->Bkettei[j] & Bmuhenkan))
			go_gakusyuu (i);
		i ++;
		j ++;
	}
}

go_gakusyuu (n)
int	n;
{
	register Conversion	*cv;
	register int	i, j;
	register int	ilen, jlen;
	char		yomi[BUFFLENGTH], kanji[BUFFLENGTH];
	char		*src, *dst;
	int		len;

	cv = current_conversion;
	i = n;
	j = i + 1;

	if (cv->Pedited[i] == 1 || cv->Pedited[j] == 1)
		return;

	ilen = cv->Elen[cv->Edit[i]];
	jlen = cv->Elen[cv->Edit[j]];

	if (ilen >= DLEN || jlen >= DLEN)
		return;

	/* get yomi and kanji	*/
	dst = yomi;
	Bgetsrc (i, &src, &len);
	if (len <= 0)
		return;
	Strncpy (dst, src, len);
	dst += len;
	*dst ++ = '\0';
	dst = kanji;
	Bgetsrc (j, &src, &len);
	if (len <= 0)
		return;
	strcpy (dst, src, len);
	dst += len;
	*dst ++ = '\0';

	/* go clgakusyuu	*/
	SJ2_clstudy (yomi, kanji, &Gdata[j]);
}
