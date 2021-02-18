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
 *	sj_getromaji.c
 *	Copyright (c) 1987 1988 1989 Sony Corporation.
 *	Written by Masaki Takeuchi
 *	Tue Jun 23 13:31:11 JST 1987
 */
/*
 * $Header: sj_getromaji.c,v 1.1 91/04/29 17:56:44 masaki Exp $ SONY;
 */

#include "sjparam.h"
#include "sj3lib.h"
#include "rtbl.h"

int	ec = 0;

extern struct romaji	**sj_Rtable;
extern int		Relement;
extern struct romaji	Rtable[];

struct romaji	(*def_Rtable) = Rtable;

int	tuduri_missmatch;
int	tuduri_match;

sj_getromaji2 (a, kana)
register char	*a;
register u_char	*kana;
{
	/*
	 *	Rtable[element number][dimension][str]
	 *		dimension: 0: romaji 1: zenkaku kana
	 *	Return: muched or unmuched point (j)
	 */
	char		alpha[SHORTBUFFSIZE];
	u_char		Htmp[SHORTBUFFSIZE];
	u_char		tmp[RSIZE];
	register int	i, j, k;

	tuduri_missmatch = 0;
	tuduri_match = 0;

	*kana = '\0';
	if (*a == '\0')
		return (0);

	strcpy (alpha, a);
	i = j = 0;
	while (i < Relement) {
		if (sj_Rtable[i]->tuduri[j] == alpha[j]) {
			j++;
			if (sj_Rtable[i]->tuduri[j] == '\0') {
				/* match	*/
				if (*(sj_Rtable[i]->kana) != '\0') {
					strcpy (kana, sj_Rtable[i]->kana);
					return (0);
				}
			}
			else if (alpha[j] == '\0') {
				/* no match	*/
				return (j);
			}
		}
		else {
			i++;
		}
		if (j && i < Relement) {
			for (k = 0 ; k < j ; k ++) {
				if (sj_Rtable[i]->tuduri[k] != alpha[k]) {
					/* no match	*/
					goto next;
				}
			}
		}
	}
next:
	if (!ec)
		tolowerstr (a, alpha);
	i = j = 0;
	while (*(def_Rtable[i].tuduri) != '\0') {
		if (def_Rtable[i].tuduri[j] == alpha[j]) {
			j++;
			if (def_Rtable[i].tuduri[j] == '\0') {
				/* match	*/
				if (*(def_Rtable[i].kana) != '\0') {
					strcpy (kana, def_Rtable[i].kana);
					return (0);
				}
			}
			else if (alpha[j] == '\0') {
				/* no match	*/
				return (j);
			}
		}
		else {
			i++;
		}
		if (j) {
			for (k = 0 ; k < j ; k ++) {
				if (def_Rtable[i].tuduri[k] != alpha[k]) {
					/* no match	*/
					goto fin;
				}
			}
		}
	}
fin:
	if (ec) {
		strcpy (kana, a);
		j = 0;
		tuduri_match = strlen (a);
		tuduri_missmatch = 0;
		return (j);
	}
	else {
		*Htmp = *a;
		Htmp[1] = '\0';
		if (IsaN ((int)*Htmp))
			strcpy (kana, STR_NN);
		else if (IsaSHIIN (*Htmp) && *Htmp == a[1])
			strcpy (kana, STR_TU);
		else
			HAtoZK (Htmp, kana);
		j = sj_getromaji2 (&a[1], tmp);
		strcat (kana, tmp);
		if (!j && !tuduri_match)
			tuduri_match = strlen (&a[1]);

		tuduri_missmatch = 1;
		return (j);
	}
}
