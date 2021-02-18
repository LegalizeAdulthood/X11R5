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
 *	henkan.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Wrriten by Masaki Takeuchi.
 *	Sat May 30 13:13:33 JST 1987
 */
/*
 * $Header: henkan.c,v 1.1 91/04/29 17:56:03 masaki Exp $ SONY;
 */

#include "common.h"
#include "key.h"
#include "rtbl.h"

/* henkan routine	*/

int	i_point;		/* index pointer		*/
int	HKindex[BUFFLENGTH * 2], Hmojilen[BUFFLENGTH * 2];
int	hk_point;
int	bunsetu;

exec_henkan ()
{
	register Conversion	*cv;
	register int		i, j;
	register int		len;
	register int		p_point;
	register u_char		*s;
	register u_char		*p;
	int			moji, lastmoji;
	u_char			Htmp[BUFFLENGTH * 2], Ztmp[BUFFLENGTH * 2];
	u_char			index[BUFFLENGTH * 2];
	register u_char		*src, *dst;
	int			next, now;
	int			plen;
	int			k, l;
	int			zlen, rlen;
	u_char			*ss;

	cv = GetConversion ();
	*Object = '\0';
	i_point = 0;
	p = Object;

	for (i = 0 ; i < cv->span_point ; i ++) {
		len = cv->span_l[i];
		if (len <= 0)
			continue;
		s = (u_char *)cv->span_p[i];
		if (cv->kettei[i] == MODE_ZHIRA) {
			Strncpy (Htmp, s, len);
			Htmp[len] = '\0';
			HAromajiZK (cv->kettei[i], Htmp, Ztmp);
			ZKtoZH (Ztmp, Htmp);
			s = Htmp;
			len = strlen (Htmp);
			for (j = 0; j < len ; j ++)
				index[i_point++] = ObjHenkan;
		}
		else {
			Strncpy (Htmp, s, len);
			Htmp[len] = '\0';
			s = Htmp;
			for (j = 0; j < len ; j ++)
				index[i_point++] = ObjMuhenkan;
		}
		strcpy (p, s);
		p += len;
	}
	index[i_point] = ObjEnd;
	*p = 0;

	if ((bunsetu = SJ2_henkan (Object, cv->Bun, cv->yomiout, cv->kanjiout)) <= 0)
		return (-1);

	i_point = 0;
	cv->Bnum = 0;

	for (i = 0 , j = 0 ; i < bunsetu && j < cv->span_point ; i ++, j ++) {
spantop:
		s = (u_char *)cv->span_p[j];
		len = cv->span_l[j];

		if (len <= 0) {
			j ++;
			if (j < cv->span_point)
				goto spantop;
			else
				break;
		}
		Strncpy (Htmp, s, len);
		Htmp[len] = '\0';
		cv->Bpoint[cv->Bnum] = i;
		cv->Bspan[cv->Bnum] = j;

		if (index[i_point] == ObjMuhenkan) {
			cv->Bkettei[cv->Bnum] = Bmuhenkan;
			strcpy (Ztmp, Htmp);
cat:
			len = strlen (Ztmp);

			if (len < cv->Bun[i].srclen) {
				i_point += len;
					/* separate and shift bunsetu	*/
					/* and re-entry			*/
				bunsetu ++;
				for (k = bunsetu ; k > i ; k --) {
					cv->Bun[k].srclen
						= cv->Bun[k-1].srclen;
					cv->Bun[k].srcstr
						= cv->Bun[k-1].srcstr;
					cv->Bun[k].destlen
						= cv->Bun[k-1].destlen;
					cv->Bun[k].deststr
						= cv->Bun[k-1].deststr;
				}
				cv->Bun[i].srclen = len;
				cv->Bun[i].destlen = len;
				cv->Bun[i+1].srclen -= len;
				cv->Bun[i+1].srcstr += len;
				cv->Bun[i+1].destlen -= len;
				cv->Bun[i+1].deststr += len;
			}
			else if (len > cv->Bun[i].srclen) {
				len = cv->Bun[i].srclen;
				i_point += len;
				if (index[i_point] == ObjMuhenkan &&
				    i < bunsetu - 1) {
					cv->Bun[i].srclen
						+= cv->Bun[i+1].srclen;
					cv->Bun[i].destlen
						+= cv->Bun[i+1].destlen;
					for (k = i + 1 ; k < bunsetu ; k ++) {
						cv->Bun[k].srclen
						    = cv->Bun[k+1].srclen;
						cv->Bun[k].srcstr
						    = cv->Bun[k+1].srcstr;
						cv->Bun[k].destlen
						    = cv->Bun[k+1].destlen;
						cv->Bun[k].deststr
						    = cv->Bun[k+1].deststr;
					}
					bunsetu --;
					i_point -= len;
					goto cat;
				}
					/* separate and shift span	*/
					/* and re-entry			*/
				for (k = cv->span_point ; k > j ; k --) {
					cv->span_p[k] = cv->span_p[k-1];
					cv->span_l[k] = cv->span_l[k-1];
					cv->kettei[k] = cv->kettei[k-1];
				}
				cv->span_point ++;
				len = get_za_len (s, cv->span_l[j], len);
				cv->span_l[j] = len;
				cv->span_l[j+1] -= len;
				cv->span_p[j+1] += len;
			}
			else {
				i_point += len;
			}
		}

		else if (index[i_point] == ObjHenkan) {
			if (cv->kettei[j] == MODE_ZHIRA) {
				HAromajiZK (cv->kettei[j], Htmp, Ztmp);
				ZKtoZH (Ztmp, Htmp);
			}
			zlen = strlen (Ztmp);
			len = strlen (Htmp);
			cv->Bkettei[cv->Bnum] = Bhenkan;

			if (len < cv->Bun[i].srclen) {
				i_point += len;
					/* separate and shift bunsetu	*/
					/* and re-entry			*/
				for (k = bunsetu ; k > i ; k --) {
					cv->Bun[k].srclen
						= cv->Bun[k-1].srclen;
					cv->Bun[k].srcstr
						= cv->Bun[k-1].srcstr;
					cv->Bun[k].destlen
						= cv->Bun[k-1].destlen;
					cv->Bun[k].deststr
						= cv->Bun[k-1].deststr;
				}
				bunsetu ++;
				cv->Bun[i].srclen = len;
				cv->Bun[i+1].srclen -= len;
				cv->Bun[i+1].srcstr += len;
				cv->Bun[i].destlen = zlen;
				cv->Bun[i+1].destlen -= zlen;
				cv->Bun[i+1].deststr += zlen;
				/*
				 * Mosi alphabet no yomi ga kanji
				 * ni henkan sare te kaette kitari suru to
				 * kono zlen dewa fukanzen de,
				 * mohaya zlen wo keisan suru koto wa
				 * dekinai darou.
				 */
			}
			else if (len > cv->Bun[i].srclen) {
/*
 *	Separate dekinai tokigaaru.
 *	Tuujyou no baai wa separate sitai.
 *	Sikasi, sorega romaji no 1tuduri no baai,
 *	Separate dekinai.
 *	Fri Sep 23 23:44:23 JST 1988
 *				masaki
 */
				zlen = 0;
				ss = cv->Bun[i].srcstr;
gege:
			    {
				len = cv->Bun[i].srclen;
				i_point += len;
				zlen += len;
				rlen = get_kana_len (
					cv->kettei[j],
					s, cv->span_l[j],
					ss, zlen
				);
					/* separate and shift span	*/
					/* and re-entry			*/
				for (k = cv->span_point ; k > j ; k --) {
					cv->span_p[k] = cv->span_p[k-1];
					cv->span_l[k] = cv->span_l[k-1];
					cv->kettei[k] = cv->kettei[k-1];
				}
				cv->span_point ++;
				if (rlen > 0) {
					cv->span_l[j] = rlen;
					cv->span_l[j+1] -= rlen;
					cv->span_p[j+1] += rlen;
				}
				else if (rlen == 0 && j < cv->span_point &&
					i < bunsetu) {
					cv->Bnum ++;
					j ++;
					i ++;
					cv->Bpoint[cv->Bnum] = i;
					cv->Bspan[cv->Bnum] = j;
					goto gege;
				}
			    }
			}
			else {
				i_point += len;
			}
		}
		else {
			cv->Bnum ++;
			break;
		}
		cv->Bnum ++;
	}
	cv->Bkettei[cv->Bnum] = Bend;

	GetKanaIndex (HKindex, Hmojilen);
	p_point = 0;
	l = cv->Bnum;
	for (i = 0, k = 0 ; i < l ; i ++, k++) {
		moji = cv->HAindex[p_point];
		cv->Pkettei[k] = cv->Dmojimod[moji];
		cv->Ppoint[k] = &(cv->Halpha[p_point]);
		if (cv->Bkettei[k] == Bmuhenkan) {
			len = cv->span_l[cv->Bspan[k]];
		}
		else {
			now = cv->Bpoint[k];
oncemore:
			if (k + 1 < l)
				next = cv->Bpoint[k + 1];
			else
				next = bunsetu;
			len = cv->Bun[now].srclen;
			for (j = 0 ; j < moji ; j ++)
				len += Hmojilen[j];
			lastmoji = HKindex[len - 1];
			if (HKindex[len] == lastmoji) {
				/* conjunction	*/
				if (next < bunsetu) {
				    if (cv->Bun[next].srclen > 0 &&
					cv->Bun[next].destlen > 0)
				    {
					cv->Bnum --;
					i ++;
					for (j = k + 2 ; j < l ; j ++) {
						cv->Bpoint[j - 1] = cv->Bpoint[j];
						cv->Bspan[j - 1] = cv->Bspan[j];
						cv->Bkettei[j - 1] = cv->Bkettei[j];
					}

					len = cv->Bun[now].srclen;
					src = cv->Bun[next].srcstr;
					dst = cv->Bun[now].srcstr;
					dst += len;
					len = cv->Bun[next].srclen;
					cv->Bun[now].srclen += len;
					cv->Bun[next].srclen = 0;
					while (len --)
						*(dst ++) = *(src ++);

					len = cv->Bun[now].destlen;
					src = cv->Bun[next].deststr;
					dst = cv->Bun[now].deststr;
					dst += len;
					len = cv->Bun[next].destlen;
					cv->Bun[now].destlen += len;
					cv->Bun[next].destlen = 0;
					while (len --)
						*(dst ++) = *(src ++);

					goto oncemore;
				    }
				}
			}

			j = 0;
			while (cv->HAindex[j] <= lastmoji && j < cv->ha_point)
				j ++;
			len = j - p_point;
		}

		if (len < 0)
			len = 0;
		cv->Plen[k] = len;
		p_point += len;
	}

	for (i = 0 ; i < cv->Bnum ; i ++) {
		cv->OrgPlen[i] = cv->Plen[i];
		cv->Pedited[i] = 0;
	}

	if (flush_conversion)
		cv->PreBun = cv->CurBun = -1;
	else
		cv->PreBun = cv->CurBun = 0;

	Bdisp (1);

	cv->Dflag = -1;	/* not a bunsetu number	*/
	cv->Enum = 0;
	cv->e_point = 0;
	cv->Kanji = 1;
}

get_za_len (s, slen, len)
u_char	*s;
int	slen, len;
{
	register int	i;
	u_char		Htmp[BUFFLENGTH * 2], Ztmp[BUFFLENGTH * 2];

	for (i = 1 ; i <= slen ; i ++) {
		Strncpy (Htmp, s, i);
		Htmp[i] = '\0';
		HAtoZA (Htmp, Ztmp);
		if (strlen (Ztmp) == len)
			return (i);
	}
	return (slen);
}

get_kana_len (kettei, s, slen, kana, len)
u_char		kettei;
u_char		*s, *kana;
int		slen, len;
{
	register int	i, j, flag;
	u_char		Htmp[BUFFLENGTH * 2], Ztmp[BUFFLENGTH * 2];

	for (i = 1 ; i <= slen ; i ++) {
		Strncpy (Htmp, s, i);
		Htmp[i] = '\0';
		HAromajiZK (kettei, Htmp, Ztmp);
		ZKtoZH (Ztmp, Htmp);
		if (strlen (Htmp) == len) {
			flag = 1;
			for (j = 0 ; j < len ; j ++) {
				if (Htmp[j] != kana[j]) {
					if (j == len - 1 &&
					    isalpha (Htmp[j]) &&
					    kana[j] == 0xaf) {
						if (s[i] == s[i-1])
						/* xtu	*/
						break;
					}
					flag = 0;
					break;
				}
			}
			if (flag) {
				if (s[i - 1] == 'n' && s[i] == 'n')
					/* nn	*/
					i ++;
				return (i);
			}
		}
	}
	return (0);
}

GetKanaIndex (HKindex, HKmojilen)
register int	*HKindex, *HKmojilen;
{
	register Conversion	*cv;
	register int	i, j, p, len;
	u_char		Htmp[BUFFLENGTH * 2], Ztmp[BUFFLENGTH * 2];

	cv = GetConversion ();
	p = 0;
	j = 0;
	for (i = 0 ; i < cv->moji_count ; i ++) {
		len = cv->Amojilen[i];
		Strncpy (Htmp, &(cv->Halpha[p]), len);
		Htmp[len] = '\0';
		p += len;
		if (cv->Dmojimod[i] == MODE_ZHIRA) {
			HAromajiZK (MODE_ZHIRA, Htmp, Ztmp);
			ZKtoZH (Ztmp, Htmp);
			len = strlen (Htmp);
		}
		else {
			HAtoZA (Htmp, Ztmp);
			len = strlen (Ztmp);
		}
		HKmojilen[i] = len;
		while (len --) {
			HKindex[j ++] = i;
		}
	}
	hk_point = j;
}

Bgetsrc (num, s, len)
register int	num;
register u_char	**s;
register int	*len;
{
	register Conversion	*cv;
	u_char		Ztmp[BUFFLENGTH * 2];

	cv = GetConversion ();
	if (cv->Plen[num] > 0) {
		Strncpy (PHbuff, cv->Ppoint[num], cv->Plen[num]);
		PHbuff[cv->Plen[num]] = '\0';
		HAromajiZK (MODE_ZHIRA, PHbuff, Ztmp);
		if (cv->Ppoint[num][cv->Plen[num]] == cv->Ppoint[num][cv->Plen[num] - 1]
		 && IsaSHIIN (cv->Ppoint[num][cv->Plen[num]])) {
			Ztmp[strlen (Ztmp) - 2] = '\0';
			strcat (Ztmp, STR_TU);
		}
		ZKtoZH (Ztmp, PZbuff);
		*len = strlen (PZbuff);
	}
	else {
		*PZbuff = '\0';
		*len = 0;
	}
	*s = (u_char *)PZbuff;
}

IsHex (c)
register u_char	c;
{
	if (isdigit (c))
		return (1);
	if (isupper (c))
		c = tolower (c);
	else if ('a' <= c && c <= 'f')
		return (1);
	else
		return (0);
}

Getcode (in, out)
register u_char	*in, *out;
{
	register int	c;
	register int	i;
	u_char		s[5];
	u_char		tmp[3];

	*out = '\0';
	i = 0;
	while ((c = *in) != '\0') {
		if (iskanji (c)) {
			in ++;
			*(out ++) = c;
			*(out ++) = *(in ++);
			i = 0;
			continue;
		}
		else if (IsHex (c))
			s[i++] = c;
		else
			break;
		in ++;
		if (i == 4) {
			s[i] = '\0';
			if (getcode (s, tmp))
				break;
			strcpy (out, tmp);
			out += 2;
			i = 0;
		}
	}
	*out = '\0';
	if (i) {
		s[i] = '\0';
		HAtoZA (s, out);
		out += strlen (out);
	}
	if (*in != '\0')
		HAtoZA (in, out);
}

/* bunsetu loading			*/

static u_char	load_mode;	/* 0: hankaku, 1: zenkaku	*/

load_bun (num, s, len)
register int	num;
register u_char	**s;
register int	*len;
{
	register Conversion	*cv;
	register int	type;
	register u_char	c;
	u_char		Ztmp[BUFFLENGTH * 2];

	cv = GetConversion ();
	if (cv->Plen[num] == 0) {
		*len = 0;
		*s = (u_char *)PZbuff;
		*PZbuff = '\0';
		return;
	}

	Strncpy (PHbuff, cv->Ppoint[num], cv->Plen[num]);
	PHbuff[cv->Plen[num]] = '\0';
	type = cv->Pkettei[num];

	switch (type) {
		case MODE_SCODE:
		case MODE_ECODE:
		case MODE_JCODE:
		case MODE_KCODE:
			load_mode = 1;
			PushCode (type);
			Getcode (PHbuff, PZbuff);
			*s = (u_char *)PZbuff;
			*len = strlen (PZbuff);
			PopCode ();
			break;
		case MODE_HALPHA:
			load_mode = 0;
			*s = (u_char *)PHbuff;
			*len = strlen (PHbuff);
			break;
		case MODE_ZALPHA:
			load_mode = 1;
			HAtoZA (PHbuff, PZbuff);
			*s = (u_char *)PZbuff;
			*len = strlen (PZbuff);
			break;
		case MODE_HKATA:
			load_mode = 0;
			c = cv->Ppoint[num][cv->Plen[num]];
			HAromajiZK (type, PHbuff, Ztmp);
			if (c == cv->Ppoint[num][cv->Plen[num] - 1] && IsaSHIIN (c)) {
				Ztmp[strlen (Ztmp) - 2] = '\0';
				strcat (Ztmp, STR_TU);
			}
			ZKtoHK (Ztmp, PZbuff);
			*s = (u_char *)PZbuff;
			*len = strlen (PZbuff);
			break;
		case MODE_ZKATA:
			load_mode = 1;
			c = cv->Ppoint[num][cv->Plen[num]];
			HAromajiZK (type, PHbuff, PZbuff);
			if (c == cv->Ppoint[num][cv->Plen[num] - 1] && IsaSHIIN (c)) {
				PZbuff[strlen (PZbuff) - 2] = '\0';
				strcat (PZbuff, STR_TU);
			}
			*s = (u_char *)PZbuff;
			*len = strlen (PZbuff);
			break;
		case MODE_ZHIRA:
		default:
			load_mode = 1;
			c = cv->Ppoint[num][cv->Plen[num]];
			HAromajiZK (MODE_ZHIRA, PHbuff, Ztmp);
			if (c == cv->Ppoint[num][cv->Plen[num] - 1] && IsaSHIIN (c)) {
				Ztmp[strlen (Ztmp) - 2] = '\0';
				strcat (Ztmp, STR_TU);
			}
			ZKtoZH (Ztmp, PZbuff);
			*s = (u_char *)PZbuff;
			*len = strlen (PZbuff);
			break;
	}
}

exec_muhenkan (first)
int	first;
{
	register Conversion	*cv;

	cv = GetConversion ();
	cv->Bnum = 1;
	cv->Ppoint[0] = &(cv->Halpha[0]);
	cv->Plen[0] = cv->ha_point;
	cv->OrgPlen[0] = cv->Plen[0];
	cv->Pedited[0] = 0;
	if (first) {
		cv->Bkettei[0] = Bmuhenkan;
		cv->Pkettei[0] = cv->Imode;
	}
	else if (cv->Bkettei[0] != Bmuhenkan) {
		cv->Bkettei[0] = Bmuhenkan;
		cv->Pkettei[0] = MODE_ZHIRA;
	}
	else if (cv->Pkettei[0] != MODE_ZHIRA) {
		cv->Pkettei[0] = MODE_ZHIRA;
	}
	else {
		cv->Pkettei[0] = MODE_ZKATA;
	}

	if (flush_conversion)
		cv->PreBun = cv->CurBun = -1;
	else
		cv->PreBun = cv->CurBun = 0;
	cv->Dflag = -1;	/* not a bunsetu number	*/
	cv->Enum = 0;
	cv->e_point = 0;
	cv->Kanji = 1;

	Bdisp (1);
}

/* bunsetu string kettei routine	*/

static u_char	Bstr_mode;	/* 0: hankaku, 1: zenkaku	*/

u_char *
Bstr (i, len)
register int	i;
int		*len;
{
	register Conversion	*cv;
	register int	j;
	u_char		*s;

	cv = GetConversion ();
	if (cv->Bkettei[i] & Bmuhenkan) {
		load_bun (i, &s, len);
		Bstr_mode = load_mode;
	}
	else if (cv->Bkettei[i] & Bedit) {
		j = cv->Edit[i];
		*len = cv->Elen[j];
		if (*len >= DLEN)
			load_bun (i, &s, len);
		else
			s = (u_char *)cv->Epoint[j];
		Bstr_mode = 1;
	}
	else if (cv->Bkettei[i] & Bhenkan) {
		j = cv->Bpoint[i];
		*len = cv->Bun[j].destlen;
		s = (u_char *)cv->Bun[j].deststr;
		Bstr_mode = 1;
	}
	else {
		*len = 0;
		s = (u_char *)0;
	}
	return (s);
}

/* bunsetu mode change routine	*/

mode_muhenkan (mod, bkettei)
register int	mod, bkettei;
{
	register int	def;

	if (!(bkettei & Bmuhenkan))
		return (KEY_ZHIRA);
	def = value_muhenkan ();
	if (mod == MODE_ZHIRA)
		return (KEY_ZKATA);
	else if (mod == MODE_ZKATA) {
		if (def != KEY_ZHIRA && def != KEY_ZKATA)
			return (def);
	}
	return (KEY_ZHIRA);
}

Bchange (mod)
register int	mod;
{
	register Conversion	*cv;
	extern int	def_code;
	register int	i;
	int		PushKettei;

	cv = GetConversion ();
	if (cv->CurBun == -1)
		cv->CurBun = 0;
	i = cv->PreBun = cv->CurBun;

	if (mod == KEY_MUHENKAN)
		mod = mode_muhenkan (cv->Pkettei[i], cv->Bkettei[i]);
	else if (mod == KEY_TOGGLE)
		mod = toggle_mode (cv->Pkettei[i]);

	PushKettei = cv->Bkettei[i];
	if (!(cv->Bkettei[i] & Bmuhenkan)) {
		cv->Bkettei[i] |= Bmuhenkan;
		cv->Bkettei[i] &= ~Bhenkan;
	}
	switch (mod) {
		case KEY_CODE:
			if (cv->Pkettei[i] == MODE_SCODE)
				cv->Pkettei[i] = MODE_ECODE;
			else if (cv->Pkettei[i] == MODE_ECODE)
				cv->Pkettei[i] = MODE_JCODE;
			else if (cv->Pkettei[i] == MODE_JCODE)
				cv->Pkettei[i] = MODE_KCODE;
			else if (cv->Pkettei[i] == MODE_KCODE)
				cv->Pkettei[i] = MODE_SCODE;
			else
				cv->Pkettei[i] = def_code;
			Rdisp ();
			break;
		case KEY_ZHIRA:
			cv->Pkettei[i] = MODE_ZHIRA;
			Rdisp ();
			break;
		case KEY_HALPHA:
			cv->Pkettei[i] = MODE_HALPHA;
			Rdisp ();
			break;
		case KEY_ZALPHA:
			cv->Pkettei[i] = MODE_ZALPHA;
			Rdisp ();
			break;
		case KEY_HKATA:
			cv->Pkettei[i] = MODE_HKATA;
			Rdisp ();
			break;
		case KEY_ZKATA:
			cv->Pkettei[i] = MODE_ZKATA;
			Rdisp ();
			break;
		default:
			cv->Bkettei[i] = PushKettei;
			beep ();
			break;
	}
}

/* bunsetu display		*/

Bdisp (redraw)
{
	register Conversion	*cv;
	register int	i, j;
	int		len;
	register u_char	*s;
	u_char		BDtmp[BUFFLENGTH * 2];

	cv = GetConversion ();
	Cclear (redraw);
	SetVcol (0);
	for (i = 0 ; i < cv->Bnum ; i ++) {
		if ((s = Bstr (i, &len)) == (u_char *)0)
			continue;
		cv->Displen[i] = len;
		cv->Dispmod[i] = Bstr_mode;

		if (len <= 0)
			continue;
		Strncpy (BDtmp, s, len);
		BDtmp[len] = '\0';
		if (i == cv->CurBun)
			vprintR (BDtmp, Bstr_mode);
		else
			vprintU (BDtmp, Bstr_mode);
	}
#ifdef SJX
	Reconfigure ();
#endif /* SJX */
	Flush ();
}

/* re-display bunsetu		*/

Rdisp ()
{
	register Conversion	*cv;
	register int	i, j;
	register int	Total;
	int		len;
	register u_char	*s;
	u_char		BDtmp[BUFFLENGTH * 2];

	cv = GetConversion ();
	Total = 0;
	for (i = 0 ; i < cv->Bnum ; i ++) {
		if ((s = Bstr (i, &len)) == (u_char *)0)
			continue;
		if (cv->Displen[i] != len || cv->Dispmod[i] != Bstr_mode) {
			AfterAllPrint (i, Total);
			return;
		}

		if (len <= 0)
			continue;
		if (i == cv->CurBun || i == cv->PreBun) {
			Strncpy (BDtmp, s, len);
			BDtmp[len] = '\0';
			if (i == cv->CurBun)
				IprintR (BDtmp, Total);
			else
				IprintU (BDtmp, Total);
		}
		Total += len;
	}
	Cgoto (Total);
#ifdef SJX
	Reconfigure ();
#endif /* SJX */
	Flush ();
}

AfterAllPrint (from, Total)
int	from;
register int	Total;
{
	register Conversion	*cv;
	register int	i;
	int		len;
	register u_char	*s;
	u_char		BDtmp[BUFFLENGTH * 2];

	cv = GetConversion ();
	Cgoto (Total);
	for (i = from ; i < cv->Bnum ; i ++) {
		if ((s = Bstr (i, &len)) == (u_char *)0)
			continue;
		cv->Displen[i] = len;
		cv->Dispmod[i] = Bstr_mode;

		if (len <= 0)
			continue;
		Strncpy (BDtmp, s, len);
		BDtmp[len] = '\0';
		if (i == cv->CurBun)
			vprintR (BDtmp, Bstr_mode);
		else
			vprintU (BDtmp, Bstr_mode);
		Total += len;
	}
	ClearToMax ();
	Cgoto (Total);
	Flush ();
}

/* bunsetu right		*/

Bright ()
{
	register Conversion	*cv;
	int	flag;

	cv = GetConversion ();
	flag = 0;
	cv->PreBun = cv->CurBun;
	do {
		cv->CurBun ++;
		if (cv->CurBun >= cv->Bnum) {
			if (flag)
				return;
			else
				flag = 1;
			cv->CurBun = 0;
		}
	} while (cv->Plen[cv->CurBun] <= 0);
	Rdisp ();
}

/* bunsetu left			*/

Bleft ()
{
	register Conversion	*cv;
	int	flag;

	cv = GetConversion ();
	flag = 0;
	cv->PreBun = cv->CurBun;
	do {
		cv->CurBun --;
		if (cv->CurBun < 0) {
			if (flag)
				return;
			else
				flag = 1;
			cv->CurBun = cv->Bnum - 1;
		}
	} while (cv->Plen[cv->CurBun] <= 0);
	Rdisp ();
}
