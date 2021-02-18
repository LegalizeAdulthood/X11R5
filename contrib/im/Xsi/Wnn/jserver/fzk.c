/*
 * $Id: fzk.c,v 1.2 1991/09/16 21:31:33 ohm Exp $
 */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"
#include "ddefine.h"
#include "de_header.h"
#include "fzk.h"
#include "kaiseki.h"
#include "wnn_malloc.h"

#define vgetc(pt) ((pt)? getc((pt)):xgetc_cur())
#define vungetc(k, pt) ((pt)? ungetc((k),(pt)):xungetc_cur(k))

static void link_job(), fzk_orvt();

#ifndef	NO_FZK
static int	fzkvect_kosuu;			/* 付属語ベクタ数 */
#endif	/* NO_FZK */
static int	kango_vect_kosuu;		/* 幹語ベクタの数 */
static int	kango_length;			/* 幹語数 */

/*
 *	付属語ファイルのフォーマット

	付属語の個数		(ft->fzklength)
	接続ベクタの長さ	(fzkvect_l)
	付属語ベクタの個数	(fzkvect_kosuu)
	幹語ベクタの長さ	(kango_vect_l)		V4.0
	幹語の品詞数		(kango_length)		V4.0
	幹語ベクタの数		(kango_vect_kosuu)	V4.0

	付属語読み文字列	付属語ベクタ数
		No.	接続ベクタ
		…	…	
	付属語読み文字列	付属語ベクタ数
		No.	接続ベクタ
		…	…
		…	…	
	…
		…	…	
	〜

	終端ベクタ
	…
	…
	〜

	幹語接続ベクタNo.	幹語接続ベクタ		V4.0
		幹語接続ベクタ kango_vect_l	付属語接続ベクタ fzk_vect_l
		[31,30,...,1][63,62,....32]...
	…	…
	…	…
	〜

	幹語No.	幹語接続ベクタNo.			V4.0
	…	…
	…	…
	〜

	疑似品詞No. 幹語接続ベクタNo. 疑似品詞ベクタ	(JKTVECT_KOSUU)	V4.0
 */


struct FT *
fzk_read(fp)
register	FILE	*fp;
{
	struct	FT	*fzk_tbl;
	extern	struct	FT	*fzk_ld();

	fzk_tbl = fzk_ld(fp);
	if (fzk_tbl != NULL) {
		if (fp != NULL)
			fclose(fp);
#ifndef NO_FZK
		link_job(fzk_tbl->fzklength, fzk_tbl);
#endif
	}
	return (fzk_tbl);
}

struct FT *
fzk_ld(fp)
register FILE	*fp;
{
	extern	void	fzk_discard();
	struct	FT	*fzk_tbl;
	register int	k, l, m;
	register int	vect_count = 0;
	struct fzkentry	*ptr;
	unsigned char	charyomi[(YOMI_L + 1) * 2];
#ifndef	NO_FZK
	int	fzklength;
#endif	/* NO_FZK */
	int	fzkvect_l;
	int	kango_vect_l;

#ifndef	NO_FZK
	fzklength = get_decimal(fp);		/* 付属語の個数 */
#endif	/* NO_FZK */
	fzkvect_l = get_decimal(fp);		/* 付属語ベクタの長さ */
#ifndef	NO_FZK
	fzkvect_kosuu = get_decimal(fp);	/* 付属語ベクタの個数 */
#endif	/* NO_FZK */
	kango_vect_l = get_decimal(fp);		/* 幹語ベクタの長さ */
	kango_vect_kosuu = get_decimal(fp);	/* 幹語ベクタの個数 */
	kango_length = get_decimal(fp);		/* 幹語の品詞数 */

	if (fzkvect_l > VECT_L) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "Sorry, your fuzokugo bit data has too many id.\n");
		error1( "Please change define VECT_L %d and compile again\n",
			fzkvect_l);
		return(NULL);
	}

	if (kango_vect_l > KANGO_VECT_L) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "Sorry, your kango bit data has too many id.\n");
		error1( "Please change define KANGO_VECT_L %d and compile again\n",
			kango_vect_l);
		return(NULL);
	}
	if (kango_vect_kosuu > KANGO_VECT_KOSUU) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "Sorry, your kango bit vector has too many id.\n");
		error1( "Please change define KANGO_VECT_KOSUU %d and compile again\n",
			kango_vect_kosuu);
		return(NULL);
	}

	if (kango_length > KANGO_HINSI_MX) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "Sorry, your kango bit data has too many id.\n");
		error1( "Please change define KANGO_MX %d and compile again\n",
			kango_length);
		return(NULL);
	}
	if (kango_vect_kosuu > kango_length) {
		wnn_errorno = WNN_FZK_TOO_DEF;
		error1( "KANGO vector kosuu is more than KANGO hinsi suu\n");
		return(NULL);
	}
	if ((fzk_tbl = (struct FT *)malloc(sizeof(struct FT))) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		return(NULL);
	}
	fzk_tbl->kango_hinsi_area = NULL;
	fzk_tbl->kango_vect_area = NULL;
	fzk_tbl->fzkvect_l = fzkvect_l;
	fzk_tbl->kango_vect_l = kango_vect_l;
#ifndef NO_FZK
	fzk_tbl->vect_area = NULL;
	fzk_tbl->tablefuzokugo = NULL;
	fzk_tbl->fzklength = fzklength;

	if ((fzk_tbl->vect_area =
	    (struct fzkvect *) malloc(sizeof (struct fzkvect) * fzkvect_kosuu)) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		fzk_discard(fzk_tbl);
		return(NULL);
	}
#endif	/* NO_FZK */

	if ((fzk_tbl->kango_hinsi_area =
	    (int *) malloc(sizeof (int) * kango_length)) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		fzk_discard(fzk_tbl);
		return(NULL);
	}
	if ((fzk_tbl->kango_vect_area =
	    (struct kangovect *) malloc(sizeof (struct kangovect) * (kango_vect_kosuu + SV_KOSUU))) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		fzk_discard(fzk_tbl);
		return(NULL);
	}

#ifndef	NO_FZK
	if ((fzk_tbl->tablefuzokugo =
	    (struct fzkentry *) malloc(sizeof (struct fzkentry) * fzk_tbl->fzklength)) == NULL) {
		wnn_errorno = WNN_MALLOC_ERR;
		error1( "malloc error in fzk\n");
		fzk_discard(fzk_tbl);
		return(NULL);
	}
	ptr = fzk_tbl->tablefuzokugo;

	for (m = 0; m < fzk_tbl->fzklength; m++, ptr++) {
		get_string(fp, charyomi);
		(void) Sstrcpy(ptr->yomi, charyomi);
		ptr->yomi_su = Strlen(ptr->yomi);
		ptr->kosu = get_decimal(fp);
		ptr->pter = &fzk_tbl->vect_area[vect_count];
		ptr->link = NULL;
		for (l = 0; l < ptr->kosu; l++, vect_count++) {
			fzk_tbl->vect_area[vect_count].no = get_decimal(fp);
			for (k = 0; k < fzkvect_l; k++) {
				fzk_tbl->vect_area[vect_count].vector[k] = get_hexsa(fp);
			}
		}
	}
#endif	/* NO_FZK */
/* 終端 vector を SV_KOSUU 個読み取ります */
	for (l = 0; l < SV_KOSUU; l++) {
		for (k = 0; k < fzkvect_l; k++) {
			fzk_tbl->kango_vect_area[l].vector[k] = get_hexsa(fp);
		}
	}
/* 幹語接続ベクタを読み取る V4.0 */
	/* 幹語前端接続ベクタNo. - 幹語前端接続ベクタ */
	for (m = 0; m < kango_vect_kosuu; m++) {
		vect_count = get_decimal(fp);
		for (k = 0; k < fzkvect_l; k++) {
			fzk_tbl->kango_vect_area[SV_KOSUU+vect_count].vector[k] = get_hexsa(fp);
		}
	}
	/* 幹語品詞No. - 幹語前端接続ベクタNo. */
	for (m = 0; m < kango_length; m++) {
		vect_count = get_decimal(fp);
		if ((k = get_decimal(fp)) != -1) {
			fzk_tbl->kango_hinsi_area[vect_count] = SV_KOSUU + k;
		} else {
			fzk_tbl->kango_hinsi_area[vect_count] = -1;
		}
	}

	if (check_eof(fp) < 0){
	    wnn_errorno = WNN_BAD_FZK_FILE;
	    return(NULL);
	}

	/* 疑似品詞番号を調べる */
#ifdef	nodef
	if ((sentou_no = wnn_find_hinsi_by_name(WNN_SENTOU_MEI)) == -1) {
		giji_hinsi_err(WNN_SENTOU_MEI);
		return(NULL);
	}
	if ((suuji_no = wnn_find_hinsi_by_name(WNN_SUUJI_MEI)) == -1) {
		giji_hinsi_err("SENTOU");
		return(NULL);
	}
	if ((katakanago_no = wnn_find_hinsi_by_name(WNN_KANA_MEI)) == -1) {
		giji_hinsi_err("KANA");
		return(NULL);
	}
	if ((eisuu_no = wnn_find_hinsi_by_name(WNN_EISUU_MEI)) == -1) {
		giji_hinsi_err("EISUU");
		return(NULL);
	}
	if ((kigou_no = wnn_find_hinsi_by_name(WNN_KIGOU_MEI)) == -1) {
		giji_hinsi_err("KIGOU");
		return(NULL);
	}
	if ((toji_kakko_no = wnn_find_hinsi_by_name(WNN_TOJIKAKKO_MEI)) == -1) {
		giji_hinsi_err("TOJI_KKAKO");
		return(NULL);
	}
	if ((fuzokugo_no = wnn_find_hinsi_by_name(WNN_FUZOKUGO_MEI)) == -1) {
		giji_hinsi_err("FUZOKUGO");
		return(NULL);
	}
	if ((kai_kakko_no = wnn_find_hinsi_by_name(WNN_KAIKKAKO_MEI)) == -1) {
		giji_hinsi_err("KAIKAKKO");
		return(NULL);
	}
	if ((giji_no = wnn_find_hinsi_by_name(WNN_GIJI_MEI)) == -1) {
		giji_hinsi_err("GIJI");
		return(NULL);
	}
#endif	/* nodef */
	return (fzk_tbl);
}

#ifdef	nodef
void
giji_hinsi_err(str)
char	*str;
{
	wnn_errorno = WNN_GIJI_HINSI_ERR;
	error1("GIJI hinsi (%s) is not defined in hinsi data file.\n", str);
}
#endif	/* nodef */
void
fzk_discard(fzk_tbl)
struct	FT	*fzk_tbl;
{
	if (fzk_tbl->kango_hinsi_area != NULL)
		free(fzk_tbl->kango_hinsi_area);
	if (fzk_tbl->kango_vect_area != NULL)
		free(fzk_tbl->kango_vect_area);
#ifndef NO_FZK
	if (fzk_tbl->vect_area != NULL)
		free(fzk_tbl->vect_area);
	if (fzk_tbl->tablefuzokugo != NULL)
		free(fzk_tbl->tablefuzokugo);
#endif
	free(fzk_tbl);
}

#ifndef NO_FZK
static void
link_job(x, fzk_tbl)
register int	x;
struct	FT	*fzk_tbl;
{
	register int	n;
	register struct	fzkentry	*pter_a, *pter_b;

	for (pter_a = fzk_tbl->tablefuzokugo, x--, n = 0; n < x; n++, pter_a++) {
		for (pter_b = pter_a + 1; pter_b <= fzk_tbl->tablefuzokugo + x &&
		    Strncmp(pter_a->yomi, pter_b->yomi, pter_a->yomi_su) == 0;
		    pter_b++)
			pter_b->link = pter_a;
	}
}
#endif


/*
 * fzk_kai : fuzokugo kaiseki
 */

int
fzk_kai(start, end, syuutan_vect, syuutan_vect1, ichbnp_p)
w_char	*start;			/* string start pointer */
w_char	*end;			/* string end   pointer */
int	syuutan_vect;		/* 文節終端 vector */
int	syuutan_vect1;		/* 文節終端 vector 1 */
struct	ICHBNP	**ichbnp_p;	/* ich-bunpou area 付属語候補 set pointer
				pointer */
{
#ifndef	NO_FZK
	static struct fzkwork	fzkwk[STRK_L + 1];/* 解析 work area */
	static struct fzkwork	fzkwk1[STRK_L + 1];/* 解析 work area */
	static int	maxpoint = STRK_L;

	struct fzkken	fzkinf[YOMI_L + 1];/* 検索 work area */
	struct fzkken	fzkinf1[YOMI_L + 1];/* 検索 work area */
	struct fzkken	*fzkinfp;
	struct ICHBNP	*getibsp();
	struct ICHBNP	*ichbnptr, *wkptr;

	register int	point;	/* index */
	register int	i, j;	/* work index */
	register int	cnt, n;	/* counter */
struct	kangovect	*endvect;	/* 文節終端 vector pointer */
struct	kangovect	*endvect1;	/* 文節終端 vector 1 */

	int	fzkvect_l = ft->fzkvect_l;
	int	kango_vect_l = ft->kango_vect_l;

	endvect = (struct kangovect *) ft->kango_vect_area + syuutan_vect;
	if (syuutan_vect1 != WNN_VECT_NO)
	    endvect1 = (struct kangovect *) ft->kango_vect_area + syuutan_vect1;
	else
	    endvect1 = 0;

 /*
  * initialize
  */
	for (i = maxpoint; i >= 0; i--) {
	    for (j = VECT_L - 1; j >= 0; j--) {
		fzkwk[i].vector[j] = 0;
		fzkwk1[i].vector[j] = 0;
	    }
	}
	maxpoint = 0;
	for (j = 0; j < fzkvect_l; j++) {
	    fzkwk[0].vector[j] = endvect->vector[j];
	    if (endvect1 != 0)
		fzkwk1[0].vector[j] = endvect1->vector[j];
	}
 /*
  * 付属語解析
  */

	for (point = 0; point <= maxpoint; point++) {
	    if (point > STRK_L) {
		wnn_errorno = WNN_WKAREA_FULL;
		error1("Fuzokugo-kaiseki area is full");
		return (-1);
	    }
	    if (fzk_ck_vector(&fzkwk[point]) ) {
	    /* 付属語検索 vector */
		fzk_ken(start + point, end, &fzkwk[point], &fzkinf[0]);
		for (fzkinfp = &fzkinf[0]; fzkinfp->ent_ptr; fzkinfp++) {
		    n = (fzkinfp->ent_ptr)->yomi_su;
		    fzk_orvt(&fzkwk[point + n], &(fzkinfp->vector[0]));
		    maxpoint = (maxpoint < point + n) ? point + n : maxpoint;
		}
	    }
	    if (fzk_ck_vector(&fzkwk1[point])) {
	    /* 付属語検索 vector 1 */
		fzk_ken(start + point, end, &fzkwk1[point], &fzkinf1[0]);
		for (fzkinfp = &fzkinf1[0]; fzkinfp->ent_ptr; fzkinfp++) {
		    n = (fzkinfp->ent_ptr)->yomi_su;
		    fzk_orvt(&fzkwk1[point + n], &(fzkinfp->vector[0]));
		    maxpoint = (maxpoint < point + n) ? point + n : maxpoint;
		}
	    }
	}


 /* 
  * 付属語候補 set
  */
	for (point = cnt = 0, *ichbnp_p = NULL; point <= maxpoint; point++) {
	    if (kng_ckvt(&fzkwk[point], kango_vect_l) ||
	        kng_ckvt(&fzkwk1[point], kango_vect_l)) {
		if (!(n = cnt % FZKIBNO)) {
		    if ((wkptr = getibsp()) == NULL) {
			if (*ichbnp_p != NULL)
			    (void) freeibsp(*ichbnp_p);
			*ichbnp_p = NULL;
			error1("Error in fzk_kai.");
			return (-1);
		    } else {
			if (*ichbnp_p == NULL)
			    *ichbnp_p = wkptr;
			else
			    ichbnptr->next_p = wkptr;
			ichbnptr = wkptr;
		    }
		}
		ichbnptr->fzkib[n].offset = point;
		for (i = 0; i < kango_vect_l; i++) {
		    ichbnptr->fzkib[n].vector[i] = fzkwk[point].vector[i];
		    ichbnptr->fzkib1[n].vector[i] = fzkwk1[point].vector[i];
		}
		cnt++;
	    }
	}
	return (cnt);
#else	/* NO_FZK */
    struct ICHBNP *getibsp();
    struct ICHBNP *wkptr;

    register int i;	/* work index */
    struct kangovect *endvect;	/* 文節終端 vector pointer */
    struct kangovect *endvect1;	/* 文節終端 vector 1 */

    int	kango_vect_l = ft->kango_vect_l;

    endvect = (struct kangovect *) ft->kango_vect_area + syuutan_vect;
    if (syuutan_vect1 != WNN_VECT_NO)
	endvect1 = (struct kangovect *) ft->kango_vect_area + syuutan_vect1;
    else
	endvect1 = 0;

    *ichbnp_p = NULL; 
    if (kng_ckvt(&endvect, kango_vect_l) || kng_ckvt(&endvect1, kango_vect_l)){
	if ((wkptr = getibsp()) == NULL) {
	    error1("Error in fzk_kai.");
	    return (-1);
	} else {
	    *ichbnp_p = wkptr;
	}
	wkptr->fzkib[0].offset = 0;
	for (i = 0; i < kango_vect_l; i++) {
	    wkptr->fzkib[0].vector[i] = endvect->vector[i];
	    if(endvect1 != 0)
		wkptr->fzkib1[0].vector[i] = endvect1->vector[i];
	}
    }
    return (1);
#endif	/* NO_FZK */
}


/*
 * kng_ckvt	幹語ベクタのチェック	V4.0
 */

int
kng_ckvt(vector, kango_vect_l)
register int	*vector;
register int	kango_vect_l;
{
	register int	i, rts;

	for (rts = 0, i = kango_vect_l; i > 0; i--)
		rts |= *vector++;/* OR cheak */
	return (rts);
}


/*
 * fzk_ckvt
 */
int
fzk_ckvt(vector)
register int	vector;
{
	return (fzk_ck_vector(((struct kangovect *) ft->kango_vect_area + vector)));
}


int
fzk_ck_vector(vector)
register int	*vector;
{
	register int	i, rts;

	rts = *vector++ & ~0x01;	/* 「先頭可」のビットを除く */
	for (i = ft->fzkvect_l - 1; i > 0; i--)
		rts |= *vector++;/* OR cheak */
	return (rts);
}


/*
 * fzk_orvt
 */

static void
fzk_orvt(vector1, vector2)
register int	*vector1;
register int	*vector2;
{
	register int	i;

	for (i = ft->fzkvect_l; i > 0; i--)
		*vector1++ |= *vector2++;/* OR set */
}

#ifndef	NO_FZK
int
fzk_ken(start, end, vector, fzkptr)
w_char	*start;
w_char	*end;
int	vector[];
struct fzkken	*fzkptr;
{
	register int	yomicnt;
	register int	setno;
	struct fzkken	*ansptr;
	register struct	fzkentry	*search_ptr;
	w_char	key[YOMI_L + 1];
	struct fzkentry	*bsrch();
	int	fzkvect_l = ft->fzkvect_l;

	ansptr = fzkptr;
	for (yomicnt = end - start; yomicnt < YOMI_L; key[yomicnt++] = 0)
		;
	for (yomicnt = 0; (yomicnt < YOMI_L) && (start < end);
	    key[yomicnt++] = *start++)
		;
	key[yomicnt] = 0;
	for (; yomicnt > 0; key[--yomicnt] = NULL) {
		search_ptr = (struct fzkentry *) bsrch(key);
		if (search_ptr != NULL) {
			setno = setfzk(search_ptr, &ansptr, vector, fzkvect_l);
			ansptr->ent_ptr = NULL;
			return (setno);
		}
	}
	fzkptr->ent_ptr = NULL;
	return (NO);
}
/******************************************/
int
setfzk(entry_ptr, answer_ptr, vector, fzkvect_l)
struct fzkentry	*entry_ptr;
struct fzkken	**answer_ptr;
int	vector[];
int	fzkvect_l;
{
	register int	setno;
	register int	vectroop;
	register int	wcnt, setflg;
	register struct	fzkvect	*vect_ptr;

	if (entry_ptr != NULL) {
		setno = setfzk(entry_ptr->link, answer_ptr, vector, fzkvect_l);
		for (wcnt = 0; wcnt < fzkvect_l; (*answer_ptr)->vector[wcnt++] = 0);
		setflg = 0;
		for (vectroop = entry_ptr->kosu, vect_ptr = entry_ptr->pter;
		    vectroop > 0; vectroop--, vect_ptr++) {
			if (bittest(vector, vect_ptr->no) > 0) {
				setflg = 1;
				for (wcnt = 0; wcnt < fzkvect_l; wcnt++)
					(*answer_ptr)->vector[wcnt] |= vect_ptr->vector[wcnt];
			}
		}
		if (setflg != 0) {
			(*answer_ptr)->ent_ptr = entry_ptr;
			(*answer_ptr)++;
			return (setno + 1);
		} else
			return (setno);
	} else
		return (0);
}
#endif	/* NO_FZK */

/***************************************************/
int
bittest(vector, no)
int	vector[];
int	no;
{
	register int	wvect;

	wvect = vector[no / (sizeof (int) << 3)];/* << 3 == * 8  */
	wvect >>= (int)(no % (sizeof(int) << 3));
	if ((wvect & 0x00000001) == 1)
		return (1);
	else
		return (-1);
}
/*
int Strncmp(s1,s2,n)
register w_char *s1;
register w_char *s2;
register int n;
{
  if(n == 0)return(0);
  for (;n > 0 && *s1++ == *s2++;n--);
  return (int)(*--s1 - *--s2);
}
*/
#ifndef NO_FZK
struct fzkentry *bsrch(key_yomi)
w_char	*key_yomi;
{
	register int	low, high, j, flg;

	for (low = 0, high = ft->fzklength; low < high;) {
		j = (high + low) >> 1;
		/*
		flg = Strncmp(key_yomi, (ft->tablefuzokugo + j)->yomi, YOMI_L);
		*/
{
		register int n;
		register w_char *s1, *s2;
		s1 = key_yomi;
		s2 = (ft->tablefuzokugo + j)->yomi;
		for (n = YOMI_L;n > 0 && *s1++ == *s2++;n--);
		flg = (int)(*--s1 - *--s2);
}
		if (flg > 0)
			low = j + 1;
		else
			if (flg < 0)
				high = j;
			else
				return (ft->tablefuzokugo + j);
	}
	return (0);
}
#endif

int
error_fzk() {
	wnn_errorno = WNN_NOT_FZK_FILE;
	error1( "Bat format in fzk_file\n");
	return (-1);
}

int
error_eof() {
	wnn_errorno = WNN_NOT_FZK_FILE;
	error1( "Unecpected EOF in reading fzk_file\n");
	return (-1);
}

int
get_decimal(fp)
register FILE	*fp;
{
	unsigned char	buf[24];
	int	k;
	if (get_string(fp, buf) == EOF) {
		return (error_eof());
	}
	if (sscanf((char*)buf, "%d", &k) != 1) {
		return (error_fzk());
	}
	return (k);
}

int
get_hexsa(fp)
register FILE	*fp;
{
	unsigned char	buf[24];
	int	k;
	if (get_string(fp, buf) == EOF) {
		return (error_eof());
	}
	if (sscanf((char*)buf, "%x", &k) != 1) {
		return (error_fzk());
	}
	return (k);
}

int
get_string(fp, buf)
register FILE	*fp;
register unsigned char	*buf;
{
	register unsigned char	*c = buf;
	register int	k;
	for (; (k = vgetc(fp)) == ';' || k == '\n' || k == '\t' || k == ' ';) {
		if (k == ';') {
			for (; (k = vgetc(fp)) != '\n';) {
				if (k == EOF) {
					return (EOF);
				}
			}
		}
	}
	if (k == EOF) {
		return (EOF);
	}
	vungetc(k, fp);
	for (; (k = vgetc(fp)) != ';' && k != '\n' &&
			k != '\t' && k != ' ' && k != EOF;) {
		*c++ = k;
	}
	*c = '\0';
	return (0);		/* not EOF */
}

int
check_eof(fp)
register FILE	*fp;
{
	unsigned char	buf[24];
	if (get_string(fp, buf) != EOF) {
		wnn_errorno = WNN_NOT_FZK_FILE;
		error1( "Not at the end of fzk_file\n");
		return (-1);
	}
	return (0);
}
