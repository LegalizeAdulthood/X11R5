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

typedef struct global {

	JREC		*Jmaxjptr;
	CLREC		*Jmaxclptr;
	JREC		*Jjrt1st;
	CLREC		*Jclt1st;
	JREC		*Jjrt2nd;
	CLREC		*Jclt2nd;
	CLREC		*Jselcl;

	Short		Jselectid;
	Short		Jtrank;
	Short		Jnrank;

	Uchar		*Jinputyomi;
	Uchar		*Jcnvstart;
	Short		Jcnvlen;
	Uchar		*Jystart;
	Uchar		Jhyomi[MaxPhInputLen+1];
	Uchar		Jorgyomi[MaxClInputLen*2+1];

	Uchar		Jkanjibuf[MinPhOutputLen];
	Uchar		*Jkanjipos;
	Uchar		*Jkanjitmp;
	Short		Jkanjilen;

	Uchar		Jdicinl;
	Uchar		Jdicsaml;
	TypeDicSeg	Jprevseg;

	Uchar		Jfzkcount;
	Uchar		Jfzk_ka_flg;
	FREC		Jfzktbl[MAXFREC];

	Uchar		Jheadcode;
	Uchar		Jheadlen;

	Uchar		Jgobilen;

	TypeGram	Jprevclgrm;
	TypeCnct	Jprevclrow;
	Uchar		Jnextcllen;

	Short		Jkhcount;
	Short		Jnkhcount;
	KHREC		Jkouhotbl[MaxKouhoNumber];
	STDYIN		*Jstdytop;

	DICT		*Jcurdict;
	DICTL		*Jdictlist;
	Uchar		*Jdicbuf;
	Uchar		*Jidxbuf;
	TypeIdxOfs	*Jidxofs;
	Uchar		*Jaskknj[MaxKnjAskNumber];
	Uchar		*Jaskknj_k[MaxKnjAskNumber];

	STDY		*Jcurstdy;

	Ushort		Jsuuji_wbuf[NumWordBuf];
	Uchar		Jsuuji_ubuf[NumKetaLength];
	TypeClass	Jsuuji_class;
	Uchar		Jsuuji_comma;
	Uchar		Jsuuji_keta;
	Uchar		*Jsuuji_yptr;
	Uchar		Jsuuji_len;
	Uchar		Jsuuji_exit;
	Ushort		*Jsuuji_wkeep;
	Uchar		*Jsuuji_ukeep;

	Uchar		Jpeepyomi[MaxWdYomiLen * 2 + 1];
	Uchar		Jpeepknj[MaxWdKanjiLen + 1];
	TypeGram	Jpeepgrm;
	Uchar		*Jpeepdptr;
	Uchar		*Jpeephptr;
	Uchar		*Jpeepkptr;
	TypeDicSeg	Jpeepidx;

} Global;

typedef	struct	dictfile {
	DICT		dict;
	int		refcnt;
	fd_set		lock;
	FILE		*fp;
	int		fd;

	Uchar		*buffer;
	long		bufsiz;

	long		idxstrt;
	long		segstrt;

	TypeIdxOfs	*ofsptr;

	struct dictfile	*link;
} DictFile;

typedef struct	stdyfile {
	STDY		stdy;
	int		refcnt;
	ino_t		inode;
	FILE		*fp;
	int		fd;

	Uchar		*header;

	struct stdyfile	*link;
} StdyFile;

typedef	struct workarea {
	int		refcnt;
	int		lock;

	struct workarea	*link;

	Global		global;
} WorkArea;

typedef	struct client {
	int		fd;
	char		host[HostNameLen];
	char		user[UserNameLen];
	char		prog[ProgNameLen];

	WorkArea	*work;
	StdyFile	*stdy;
} Client;
