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

#ifndef	_SJ_STRUCT

#define	_SJ_STRUCT

typedef	struct	jiritu {
	struct jiritu	*jsort;
	TypeDicSeg	jseg;
	TypeDicOfs	jofsst;
	TypeDicOfs	jofsed;
	Ushort		flags;
	TypeClass	class;
	TypeDicID	dicid;
	Uchar		jlen;
	TypeGram	hinsi;
	Uchar		sttofs;
	Uchar		stbofs;
	Uchar		count;
	Uchar		numlen;
} JREC;


typedef struct bunsetu {
	JREC	*jnode;
	struct	bunsetu	*clsort;
	Uchar		gobiln;
	Uchar		cllen;
	TypeCnct	right;
	Uchar		cl2len;
	Uchar		kubun;
	Uchar		fzk_ka : 1;
} CLREC;


typedef	struct	kouho {
	CLREC		*clrec;
	TypeDicOfs	offs;
	TypeStyNum	styno;
	Uchar		rank;
	Uchar		sttfg  :  1;
	Uchar		sttkj  :  1;
	Uchar		ka_fg  :  1;
	Uchar		ka_kj  :  1;
	Uchar		mode   :  4;
} KHREC;


typedef	struct	conj {
	Uchar		len;
	TypeCnct	right;
} CREC;


typedef	struct	fuzoku {
	Uchar		*yomip;
	Uchar	TFar	*fzkp;
} FREC;


typedef struct study_in {
	TypeDicOfs	offset;
	TypeDicSeg	seg;
	TypeStyNum	styno;
	TypeDicID	dicid;
	Uchar		sttkj  :  1;
	Uchar		ka_kj  :  1;
	Uchar		nmflg  :  1;
} STDYIN;


typedef struct study_out {
	STDYIN		stdy1;
	TypeGram	hinshi;
	Uchar		len;
	Uchar		sttlen :  2;
	Uchar		sttfg  :  1;
	Uchar		ka_fg  :  1;
} STDYOUT;


typedef	struct	dict {
	TypeDicID	dicid;

	TypeIdxOfs	idxlen;

	TypeDicOfs	seglen;
	TypeDicSeg	segunit;
	TypeDicSeg	maxunit;

	IFunc		getofs;
	IFunc		getidx;
	IFunc		getdic;
	IFunc		putidx;
	IFunc		putdic;
	IFunc		rszdic;
} DICT;


typedef	struct	dictl {
	DICT		*dict;
	struct dictl	*next;
} DICTL;


typedef struct	stdy {
	Short	stdycnt;
	Short	stdymax;
	STDYIN	*stdydic;

	Short	clstdystep;
	Ushort	*clstdyidx;

	Short	clstdylen;
	Uchar	*clstdydic;
} STDY;

#endif /* !SJ_STRUCT */

