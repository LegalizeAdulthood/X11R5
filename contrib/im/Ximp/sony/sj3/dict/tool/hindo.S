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
 * Author: Noriyuki Hosoe, Sony Corporation
 */

#include <stdio.h>
#include <sys/types.h>
#include "sj_struct.h"

extern	HindoRec *hindo[];
extern	int	hindo_num;
extern	AssyukuRec *assyuku;
extern	HindoRec *askknj[];
extern	int	askknj_num;
extern	int	kanji_num;

OffsetRec *real_ofsrec();

check_hindo(ptr, len)
u_char	*ptr;
int	len;
{
	int	low, high, mid;
	int	i;

	if (hindo_num <= 0) return 0;

	low = 0; high = hindo_num - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		i = string_cmp(hindo[mid]->kptr, hindo[mid]->klen, ptr, len);
		if (i > 0)
			high = mid - 1;
		else if (i < 0)
			low = ++mid;
		else
			return(hindo[mid]->exist + hindo[mid]->hindo);
	}

	return 0;
}

static	AssyukuRec *makeassyuku(num)
int	num;
{
	AssyukuRec *arec;

	arec = (AssyukuRec *)Malloc(sizeof(AssyukuRec));

	arec -> len = num;
	arec -> nrec = NULL;
	arec -> anext = NULL;

	return arec;
}

static	assyuku_len(hrec)
HindoRec *hrec;
{
	int	org;
	int	old;
	int	new;
	OffsetRec *p;

	if (((*(hrec->kptr) & KanjiModeMask) == OffsetAssyuku) &&
	    (hrec->klen == 2)) {
		p = real_ofsrec(hrec -> kptr);

		org = check_hindo(p -> kptr, p -> klen);

		old = p->klen * org + 2 * (hrec->exist + hrec->hindo);

		new = p->klen + (hrec->exist + hrec->hindo + org);
	}
	else {
		old = hrec -> klen * (hrec -> exist + hrec -> hindo);

		new = hrec -> klen + (hrec -> exist + hrec -> hindo);
	}

	return (old - new);
}

static	set_assyuku(hrec)
HindoRec *hrec;
{
	register AssyukuRec *arec;
	register AssyukuRec *aprev;
	register int	len;

	len = assyuku_len(hrec);

	if (len <= 0) return;

	arec = assyuku; aprev = NULL;
	while (arec) {
		if (arec -> len == len) {
			hrec -> arec = arec;
			hrec -> anext = arec -> nrec;
			arec -> nrec = hrec;
			return;
		}
		else if (arec -> len < len)
			break;

		aprev = arec;
		arec = arec -> anext;
	}

	arec = makeassyuku(len);

	hrec -> anext = NULL;
	hrec -> arec = arec;
	arec -> nrec = hrec;

	if (aprev) {
		arec  -> anext = aprev -> anext;
		aprev -> anext = arec;
	}
	else {
		arec  -> anext = assyuku;
		assyuku = arec;
	}
}

static	reset_assyuku(hrec)
HindoRec *hrec;
{
	AssyukuRec *arec;
	AssyukuRec *aptr;
	AssyukuRec *aprev;
	HindoRec *hptr;
	HindoRec *hprev;

	arec = hrec -> arec;

	if (!arec) return;
	hrec -> arec = NULL;

	hptr = arec -> nrec; hprev = NULL;
	while (hptr) {
		if (hptr == hrec) break;
		hprev = hptr;
		hptr = hptr -> anext;
	}

	if (!hptr) return;

	if (hprev) {
		hprev -> anext = hrec -> anext;
		hrec -> anext = NULL;
		return;
	}

	if (hrec -> anext) {
		arec -> nrec = hrec -> anext;
		hrec -> anext = NULL;
		return;
	}


	if (!assyuku) return;
	aptr = assyuku; aprev = NULL;
	while (aptr) {
		if (aptr == arec) break;
		aprev = aptr;
		aptr = aptr -> anext;
	}

	if (!aptr) return;

	if (aprev)
		aprev -> anext = arec -> anext;
	else
		assyuku = arec -> anext;

	Free(arec);

	hrec -> anext = NULL;
}

static	HindoRec *makehindo(ptr, len, alen)
u_char	*ptr;
int	len;
int	alen;
{
	HindoRec *hrec;
	u_char	*p;

	hrec = (HindoRec *)Malloc(sizeof(HindoRec));
	p = (u_char *)Malloc(len);

	hrec -> klen = len;
	hrec -> kptr = p;
	if (len == alen && len > 2) {
		hrec -> hindo = 0;
		hrec -> exist = 1;
		hrec -> offset = ++kanji_num;
	}
	else {
		hrec -> hindo = 1;
		hrec -> exist = 0;
		hrec -> offset = 0;
	}
	while (len-- > 0) *p++ = *ptr++;
	hrec -> anext  = NULL;
	hrec -> arec   = NULL;

	return hrec;
}

static	set_hindo(ptr, len, alen)
u_char	*ptr;
int	len;
int	alen;
{
	HindoRec *hrec;
	int	low, high, mid;
	int	i;

	if (hindo_num <= 0) {
		hrec = makehindo(ptr, len, alen);
		hindo[0]= hrec;
		hindo_num = 1;

		set_assyuku(hrec);

		return;
	}

	low = 0; high = hindo_num - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		i = string_cmp(hindo[mid]->kptr, hindo[mid]->klen, ptr, len);
		if (i > 0)
			high = mid - 1;
		else if (i < 0)
			low = ++mid;
		else
			break;
	}

	if (i) {
		if (hindo_num >= MaxHindoNumber) {
			fprintf(stderr, "頻度テーブルがあふれました\n");
			exit(1);
		}

		hrec = makehindo(ptr, len, alen);

		for (i = hindo_num ; i > mid ; i--) hindo[i] = hindo[i - 1];
		hindo[mid] = hrec;
		hindo_num++;
	}
	else {
		hrec = hindo[mid];

		reset_assyuku(hrec);

		if (hrec -> exist > 0)
			hrec -> exist += 1;

		else if (len == alen && len > 2) {
			hrec -> exist = 1;
			hrec -> offset = ++kanji_num;
		}

		else
			hrec -> hindo += 1;
	}

	set_assyuku(hrec);
}

static	reset_hindo(p, l)
u_char	*p;
int	l;
{
	HindoRec *hrec;
	int	low, high, mid;
	int	i;

	if (hindo_num <= 0) return;

	low = 0; high = hindo_num - 1;
	while (low <= high) {
		mid = (low + high) / 2;
		i = string_cmp(hindo[mid]->kptr, hindo[mid]->klen, p, l);
		if (i > 0)
			high = mid - 1;
		else if (i < 0)
			low = ++mid;
		else {
			hrec = hindo[mid];

			reset_assyuku(hrec);

			if (hrec -> exist > 0)
				hrec -> exist -= 1;
			else if (hrec -> hindo > 0)
				hrec -> hindo -= 1;

			if (hrec -> exist + hrec -> hindo == 0) {
				while (++mid < hindo_num) {
					hindo[mid - 1] = hindo[mid];
				}
				hindo_num--;
				Free(hrec -> kptr);
				Free(hrec);
			}
			else {
				set_assyuku(hrec);
			}

			return;
			break;
		}
	}
}

knjhnd_set(p, l)
u_char	*p;
int	l;
{
	register u_char	*kp1;
	register u_char	*kp2;
	register int	i;
	register int	kl1;
	u_char	tmp[MaxKanjiLength * 2 + MaxAtrNumber * 2 + 1];

	for (kp1 = p, kl1 = l ; kl1 > 0 ; ) {

		for (i = 0, kp2 = kp1 ; i < kl1 ; ) {
			if (codesize(*kp2) == 1) {
				tmp[i++] = *kp2++;
			}
			else {
				tmp[i++] = *kp2++;
				tmp[i++] = *kp2++;
			}
			set_hindo(tmp, i, l);
		}

		kl1 -= codesize(*kp1);
		kp1 += codesize(*kp1);
	}
}

knjhnd_reset(p, l)
u_char	*p;
int	l;
{
	register u_char	*kp1;
	register u_char	*kp2;
	register int	i;
	register int	kl1;
	u_char	tmp[MaxKanjiLength * 2 + MaxAtrNumber * 2 + 1];

	for (kp1 = p, kl1 = l ; kl1 > 0 ; ) {

		for (i = 0, kp2 = kp1 ; i < kl1 ; ) {
			if (codesize(*kp2) == 1) {
				tmp[i++] = *kp2++;
			}
			else {
				tmp[i++] = *kp2++;
				tmp[i++] = *kp2++;
			}
			reset_hindo(tmp, i);
		}

		kl1 -= codesize(*kp1);
		kp1 += codesize(*kp1);
	}
}

static	check_assyuku(p, q)
register HindoRec *p;
register HindoRec *q;
{
	u_char	ptmp[128], qtmp[128];
	int	plen;
	int	qlen;

	plen = make_knjstr(p -> kptr, p -> klen, ptmp);

	qlen = make_knjstr(q -> kptr, q -> klen, qtmp);

	if (bubun_str(ptmp, plen, qtmp, qlen)) {
		return -1;
	}

	if (overlap_str(ptmp, plen, qtmp, qlen))
		return -1;

	return 0;
}

decide_knjask()
{
	int	i, j;
	AssyukuRec *p;
	HindoRec *q;
	u_char	*r;
	int	len = 0;

	askknj_num = 0;

	for (p = assyuku ; p ; p = p -> anext) {

		if (p -> len <= 0) break;

		for (q = p -> nrec ; q ; q = q -> anext) {

			i = q -> klen;
			r = q -> kptr;
			if (i > 2) {
				while (i > 0) {
					if ((*r & KanjiModeMask)
							== OffsetAssyuku) {
						break;
					}
					j = codesize(*r);
					i -= j;
					r += j;
				}
				if (i) continue;
			}

			if (askknj_num) {
				for (i = 0 ; i < askknj_num ; i++) {
					if (check_assyuku(askknj[i], q))
						break;
				}

				if (i >= askknj_num) {
					askknj[askknj_num++] = q;
					len += p -> len;
					if (askknj_num >= MaxKnjAskNumber)
						goto end;
				}
			}
			else {
				askknj[askknj_num++] = q;
				len += p -> len;
			}
		}
	}
end:
{
	int	i, j, len;
	u_char	*p;

	for (i = 0 ; i < askknj_num ; i++) {
		len = askknj[i] -> klen;
		p   = askknj[i] -> kptr;
		if (len <= 2) continue;
		while (len > 0) {
			if ((*p & KanjiModeMask) == OffsetAssyuku) {
				printf("オフセット圧縮を含む %d\n", i);
			}
			j = codesize(*p);
			len -= j;
			p += j;
		}
	}
}
	return len;
}

clear_hindo()
{
	register int	i;

	for (i = 0 ; i < hindo_num ; i++) {
		Free(hindo[i] -> kptr);
		Free(hindo[i]);
	}
	hindo_num = 0;
	kanji_num = 0;
}

clear_assyuku()
{
	register AssyukuRec *p;

	while (assyuku) {
		p = assyuku;
		assyuku = assyuku -> anext;
		Free(p);
	}
}
