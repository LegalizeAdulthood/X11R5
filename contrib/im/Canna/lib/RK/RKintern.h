/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

/* @(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKintern.h,v 1.9 91/07/12 13:39:45 kon Exp $ */
#ifndef		_RKintern_h
#define		_RKintern_h

/*	kana kanji henkan jisho
 *		MS	870713
 *
 *	+---------------+
 *	|     HEADER	|	jisho header (256 byte)
 *	+---------------+256L
 *	|		|	yomigana no index
 *	|   DIRECTORY	|
 *	+---------------+
 *	|		|	tango jouhou
 *	|		|		yomi, {(row, col), tsuduri, hindo}...
 *	|     WORD	|
 *	|		|
 *	|		|
 *	+---------------+
 *	|		|	setuzoku jouhou(swd nomi)
 *	|   CONJUNCT	|		row...
 *	|		|
 *	+---------------+
 */

/* BASIC TYPE:
 *	subete no data ha MSB first(Motorolla order) de tenkai sareru
 *		unsigned char	w
 *		unsigned short	wx
 *		unsigned long	wxyz
 */	
#define	L4TOL(l4)\
	(((((((unsigned long)(l4)[0]<<8)|(l4)[1])<<8) | (l4)[2])<<8)|(l4)[3])
#define	S2TOS(s2)	(((unsigned short)(s2)[0]<<8)|(s2)[1])
#define	LTOL4(l, l4)	{\
	(l4)[0] = ((l)>>24)&255;\
	(l4)[1] = ((l)>>16)&255;\
	(l4)[2] = ((l)>>8)&255;\
	(l4)[3] = (l)&255;\
}

/* HEADER:
 *	unsigned char	magic[2];
 *	unsigned char	pakeuc;		key ga asshuku sareteiru
 *	unsigned char	name[];		jisho mei
 *					xxx.mwd xxx.swd xxx.pre xxx.suc
 *					\n de shuuryou
 */
#define		ND_HDRSIZ		256		/* header size */
#define		ND_HDRMAG		(('N'<<8)|'D')	/* magic number */
/* jisho class */
#define		ND_MWD		0
#define		ND_SWD		1
#define		ND_PRE		2
#define		ND_SUC		3
#define		ND_EMP		4		/* kuugo */
#define		ND_OPN		5		/* hiraki kakko */
#define		ND_CLS		6		/* toji kakko */
#define		ND_PUN		7		/* kutouten */

/* DIRECTORY:
 *	xxxx NODE....
 *	    xxxx niha, DIRECTORY no ookisa ga byte de hairu.
 *
 *	4 byte no NODE kara kousei 
 *	
 *	+--+--+--+--+
 *	|KY|X1 X2 X3|
 *	+--+--+--+--+
 *	 !  !
 *	 !  +-------------	X1
 *	 !			word record/directory offset
 *	 !			 (X1<<16) | (X2<<8) | (X3)
 *	 !
 *	 +----------------	yomi (ascii/euc hiragara lower byte)
 *				0
 */
#define	ND_NODSIZ	4		/* node size in byte */
#define	ND_DSPENT	256		/* #entry in the dispatch table */
/* mask values */
#define	ND_KEYMASK	0xff000000L	/* key mask    (8 bit) */
#define	ND_BITMASK	0x00e00000L	/* bits mask   (3 bit) */
#define	ND_OFFMASK	0x001fffffL	/* offset mask (21 bit) */
/* X1 bit flags */
#define	ND_LAST		0x00800000L	/* directory no saishuu node (entry) */
#define	ND_WORD		0x00400000L	/* offset ha tango joushou wo sasu */
#define	ND_RFU		0x00200000L	/* Mishiyou (offset ni tuika ?) */

#define ND_NULLKEY	0x00		/* null key */	
#define	ND_NULLOFF	ND_OFFMASK	/* tango ga sakujo sareta */

/* accessors */
#define NOD2KEY(node)	(((node)>>24)&255)
#define NOD2CNT(node)	(((node)>>24)&255)
#define NOD2BIT(node)	((node)&ND_BITMASK)
#define NOD2OFF(node)	((node)&ND_OFFMASK)
/* predicators */
#define	ISLASTNOD(node)	((node)&ND_LAST)
#define	ISWORDNOD(node)	((node)&ND_WORD)

/* WORD:
 *	xxxx {yomi nkouho {flags row col freq kouho}...}...
 *	    xxxx niha, WORD no ookisa ga byte de hairu.
 *
 * tango:
 *	unsigned char	yomi[];			yomigana 
 *	unsigned char	nkouho;			kouho no kazu 
 *		unsigned char	flags;		kouho no nagasa + flags
 *		unsigned char	row;		kouho no row bangou
 *		unsigned char	col;		kouho no col bangou
 *		unsigned char	freq		kouho no shiyou hindo
 *		unsigned char	kouho[];
 *	
 */

#define	NW_MAXWREC      /*256*/1024	/* maximum length of word record */
#define	NW_MAXCAND	255	/* maximum number of candidates in a wrec */

#define	NW_PREFIX	4		/* the length of kouho prefix in byte */
/* flags */
#define	NW_R256		0x80		/* set when row >= 256 */
#define	NW_C256		0x40		/* set when col >= 256 */
#define	NW_LEN		0x3f		/* kouho no nagasa ( zenkaku 31 moji) */
#define	row256(word)	((word)&NW_R256)
#define	col256(word)	((word)&NW_C256)
#define	candlen(word)	((word)&NW_LEN)
#define wordlen(word)	(candlen(word) + NW_PREFIX)

#define MAX_TEXT_LEN    1024            /* TEXT DIC no 1 record no MAXLEN */

#define	readnode(off)		L4TOL(off)
#define	wrtnode(off, node)	LTOL4(node, off)

/* RkWcand
 *	data structure used to handle the word record 
 */
struct RkWcand {
    unsigned char	*addr;		/* houho sentou address */
    short		rnum;		/* row number */
    short		cnum;		/* column number */
    unsigned char	klen;		/* kouho no nagasa */
    unsigned char	freq;		/* kouho hindo jouhou */
};

/* CONJUNCT:
 *	xxxx rrcc
		row_0  row_1  ... row_rr-1
 *		name_0 name_1 ... name_rr-1
 *	    xxxx niha, CONJUNCT no ookisa ga byte de hairu.
 *	    rr, cc ha sorezore, row, col no kosuu wo simesu.
 *	    row_i ha byte kyoukai kara hajimaru cc bit karanaru bit retu
 *	    name_i ha EOS de owaru mojiretu
 *
 */

#define		crpair(row, col)	(((long)(col)<<16)|(long)(row))
#define		pair2row(num)		((int)((num)&0xffff))
#define		pair2col(num)		((int)(((num)>>16)&0xffff))
/* ngram
 *	tango kan no setuzoku wo simesu gyouretu
 *	SWD ga open sareruto douji ni yomikomareru
 */
struct RkKxGram {
/* setuzoku jouhou */
    int			ng_row;		/* row no kazu */
    int			ng_col;		/* col no kazu */
    int			ng_rowbyte;	/* row atari no byte suu */
    unsigned char	*ng_conj;	/* setuzoku gyouretu/code table */
    unsigned char	*ng_strtab;
};

#define	GetGramRow(g, r) 	((g)->ng_conj + (r)*(g)->ng_rowbyte)
#define	TestGram(cnj, col)	((cnj) && ((cnj)[((col)>>3)]&(0x80>>(col&7))))

/* assumption on the lexical code */
#include		"ncommon.h"

#define	BUNSETU		P_BB
#define	MITEIGI		P_BB
#define	KUTOUTEN	P_BB
/* hinshi no bunrui
 *	renbunsetu henkan de siyou sareru
 */
#define	IsShuutan(g, r)	TestGram(GetGramRow(g, r), 0)
#define	IsTaigen(g, r)	TestGram(GetGramRow(g, R_TAIGEN), r)
#define	IsYougen(g, r)	TestGram(GetGramRow(g, R_YOUGEN), r)
#define	IsKakujs(g, r)	TestGram(GetGramRow(g, R_KAKUJS), r)

/* ncache
 *	tango record no caching list 
 *  ncache ha, jisho kara itido yomikomareta tango record wo cache kanri suru
 * tameno jouhou kouzou dearu.
 *  tango record ha key (dicnum, address) de skibetu sareru.
 *  taiou suru tango record ha nc_word ni yori simesareru HEAP ryouiki ni aru.
 *	+------------+
 *	| nc_dicnum  |	key part
 *	| nc_address |
 *	+------------+
 *	|  nc_word   |	value part
 *	+------------+
 *  ncache ha 2tu no souhoukou list hash/free list ni tunagareru.
 *  (* koreha UNIX i/o system no buffer cache kara hint wo eta *)
 *  hash list ha key no kensaku wo kousoku ka suru tame ni shiyou sareru.
 *  free list ha kaihou sareta cache ga tunagareru.
 */
struct ncache	{
    struct ncache	*nc_hnext;	/* hash list */
    struct ncache	*nc_hprev;
    struct ncache	*nc_anext;	/* free list */
    struct ncache	*nc_aprev;
    unsigned char	*nc_word;	/* heap jouno tango record */
    unsigned		nc_utime;	/* tango ga kakutei sareta jikoku */
    struct DM		*nc_dic;	/* yomikomareta jisho */
    unsigned short	nc_flags;
    unsigned short	nc_count;	/* word karano sanshou dosuu */
    long		nc_address;	/* word record no file address */
};
/* flags */
#define	NC_DIRTY	1	/* word no naiyou ga henkou sareta */
#define	NC_ERROR	2	/* cache no naiyou ga tadasiku nai */
#define	NC_NHEAP	4	/* word ha heap wo sasite inai */

/* nword
 *	bunsetsu kaiseki kekka 
 * 	jisho kara no tango yomidasi kekka
 * nword ha, jisho kara search sareta tango record wo kirokusi, 
 * bunsetsu kaiseki ni yori sakusei sareru bunsetsu tree wo hyougen suru.
 */
struct nword {
    struct ncache	*nw_cache;	/* tango record wo fukumu cache */
    short		nw_row, nw_col;	/* setsuzoku  jouhou */
    unsigned char	nw_ylen;	/* word ni taiou suru yomi no nagasa */
    unsigned char	nw_klen;	/* word ni taiou suru kanji no nagasa */
    unsigned char	nw_flags;	/* hinshi flags */
    unsigned char	nw_lit;		/* literal conversion */
    short		nw_prio;	/* kouzou ni yoru priority */
    struct nword	*nw_left;	/* hidari ni tunagaru word */
    struct nword	*nw_next;	/* onaji nw_len wo motu list */
    unsigned char	*nw_kanji;	/* kanji kouho ichi/douteki na kouho */
};
/* flags */
#define	Rk_MWD		0x80
#define	Rk_SWD		0x40
#define	Rk_PRE		0x20
#define	Rk_SUC		0x10
#define	NW_PURGE	0x08		/* kouho kara hazureta */
#define	NW_CLASS	0x07		/* word class */
/* accessor & predicates */
#define	purgeword(f)	((f)&NW_PURGE)
#define	word2class(f)	((f)&NW_CLASS)

#define	MAXPRIO		255

/* conversion number */
#define	LIT2GRP(cn)	((cn)>>4)
#define	LIT2MEM(cn)	((cn)&15)
#define	MAKELIT(g, w)	(((g)<<4)|(w))
#define	MAXLIT		16

#define	LIT_NONE	0
#define	LIT_NUM		1
#define	LIT_ALPHA	2
#define	LIT_HIRA	3
/* nbun
 *	bunsetsu queue
 * renbunsetsu henkan kekka wo kiroku 
 */
struct	nbun {
    unsigned char	*nb_key;	/* henkan maeno yomigana */
    struct nword	*nb_cand;	/* kouho wo kousei suru word
				     * nw_next niyori sort sareru
				     */
    unsigned char	nb_maxlen;	/* yomi no saidai */
    unsigned char	nb_curlen;	/* genzai no nagasa */
    unsigned char	nb_maxcand;	/* kouho no kosuu */
    unsigned char	nb_curcand;	/* sentaku sareteiru kouho */
};

/* DM/DF/DD
 */
struct DM {
/* pointers for the file structure */
    struct DM		*dm_next;
    struct DM		*dm_prev;
    struct DF		*dm_file;	/* ptr to the file containing this */
/* attributes */
    char		*dm_dicname;		
    char		*dm_nickname;
    unsigned		dm_class;	/* MWD/SWD/PRE/SUC */
    unsigned		dm_flags;	/* flags */
    unsigned char	dm_packbyte;	/* shouryaku sareta joui byte */
/* reference  count from MD */
    int			dm_rcount;
/* setuzoku jouhou (SWD nomi) */
    struct RkKxGram	*dm_gram;

    char		*dm_extdata;	
};
#define	DM2TYPE(dm)	((dm)->dm_file->df_type)
/* flag values */
#define	DM_EXIST	1	/* member exists */
#define	DM_WRITABLE	2	/* jisho ga write dekiru */
#define	DM_LOADWORD	4	/* word rec wo memory ni yomi */


struct DF {
/* DF doubly linked list */
    struct DF	*df_next;
    struct DF	*df_prev;
    struct DD	*df_direct;	/* backward pointer to the directory */
    struct DM	df_members;	/* the header of the dictionary */
/* attributes */
    char	*df_link;	/* link name of the file */
    unsigned	df_type;	/* type of file */
    unsigned	df_flags;	/* access mode */
    int		df_rcount;	/* # the mounted members */
    char	*df_extdata;
};
/* jisho no shurui */
#define	DF_PERMDIC	0	/* file ni aru jisho */
#define	DF_TEMPDIC	1	/* zantei jisho */

/* flag values */
#define	DF_EXIST	1	/* file exists */
#define	DF_WRITABLE	2	/* file is writable */

struct DD {
/* DD doubly linked list */
    struct DD	*dd_next;
    struct DD	*dd_prev;
    char	*dd_path;	/* path name of the directory */
    char	*dd_name;	/* unique name of the directory */
    int		dd_rcount;	/* # the opened files + reference from DDP */
    struct DF	dd_files;	/* the header of the file list */
    unsigned	dd_flags;	/* */
};

/* MD
 *  context ni mount sareta jisho/jisho wo mount siteiru jisho list no taiou
 */
struct MD {
    struct MD		*md_next;	/* dictionary list */
    struct MD		*md_prev;
    struct DM		*md_dic;	/* jisho heno pointer */
    int			md_flags;	/* mount flags */
};
#define	MD_WRITE	1		/* gakushuu shitei */
#define	MD_MPEND	2		/* mount pending */
#define	MD_UPEND	4		/* unmount pending */

/* RkParam
 *	common parameter block
 */
struct RkParam {
/* cache */
    struct ncache	*cache;
    int			maxcache;
/* heap */
    unsigned char	*heap;
    int			maxheap;
    unsigned char	*ham;
    int			maxham;
    struct nword	*word;		/* henkyaku sareta word wo tunageru */
/* jisho */
    struct DD		dd;
    char        	*ddhome;	/* dictionary home directory */
/* grammar dictionary */
    struct RkKxGram	*gramdic;
}	SX;

/* td_and_n structure 
 *
 * tupple of td and n for RkGetWordTextDic()
 */

struct td_n_tupple {
  char               *td; /* in fact this type is (struct TD *) */
  int                n;
  struct td_n_tupple *next;
};

/* context
 *	ncontext ha rennbunnsetu hennkann client wo seigyo suru jouhou
 *		jisho kankyou
 *		renbunsetsu henkan sagyou ryouiki 
 */
struct RkContext {
/* jisho kannkyou */
    struct MD		*mount[4];	/* mount list */
/* renbunsetu henkan sagyou ryouiki */
    unsigned char	*yomi;		/* yomigana */
    unsigned char	*y;
    int			maxyomi;	/* yomigana no saidaichou */
    struct nbun		*bunq;		/* bunsetsu kaiseki queue */
    int			maxbunq;	/* saidai bunnsetu suu */
    int			maxbun;		/* saidai bunsetsu suu */
    int			curbun;		/* genzai no bunsetsu */

    struct nword	**parse;	/* used in _RkparseBun() */
    struct nword	**succ;		/* used in _RkRenbun2() */

    int			time;		/* */
    int			flags;		/* context control flags */
    unsigned long	kouhomode;	/* kana kouho mode */
    unsigned long	*litmode;	/* literal conversion table */

/* jishowo 1gyouzutu yomu tokino pointer :used in RkGetWordTextDic */
    int                 gwt_cx;		/* used by RkGetWordTextDic */
    unsigned char       *gwt_dicname;   /* used by RkGetWordTextDic */
    struct td_n_tupple  *tdn;		/* used by RkGetWordTextDic */
      
/* search path */
    struct DD   	**ddpath;	/* dictionary search path */
};
typedef struct RkContext	RkContext;
/* flag values */
#define	CTX_LIVE		1	/* allocate sareta */
#define	CTX_XFER		2	/* bunsetu henkann chuu */

#define                   MAX_CONTEXT    512

#define	IS_LIVECTX(cx)    ((cx)->flags&CTX_LIVE)
#define IS_XFERCTX(cx)    ((cx)->flags&CTX_XFER)

/* RkRxDic
 *	romaji/kana henkan jisho 
 */
struct RkRxDic	{
    unsigned char	*nr_string;	/* romaji/kana taiou hyou */
    int			nr_strsz;	/* taiou hyou no size */
    unsigned char	**nr_keyaddr;	/* romaji key no kaishi iti */
    int			nr_nkey;	/* romaji/kana taiou suu */
};

/* RkDST
 *	dictionary switch table
 */
struct RkDST {
    int		(*d_open)();	/* jisho ga open sareta toki */
    int		(*d_close)();	/* jisho ga close sareta toki */
    int		(*d_search)();	/* jisho kara tango wo search suru */
    int		(*d_io)();	/* jisho he tango to cache no io */
    int		(*d_ctl)();	/* jisho koyuuno sousa */
};
extern struct RkDST	_RkDST[];

#define	DST_PROC(dm)		_RkDST[DM2TYPE(dm)]
#define	DST_OPEN(dm, file, mode)\
	(DST_PROC(dm).d_open)(dm, file, mode)
#define	DST_CLOSE(dm, file)	(DST_PROC(dm).d_close)(dm, file)
#define	DST_SEARCH(dm, k, n, cp, max)\
	(DST_PROC(dm).d_search)(dm, k, n, cp, max)
#define	DST_IO(dm, cp, io)	(DST_PROC(dm).d_io)(dm, cp, io)
#define	DST_READ(dm, cp)	DST_IO(dm, cp, 0)
#define	DST_WRITE(dm, cp)	DST_IO(dm, cp, 1)
#define	DST_CTL(dm, what,arg)	(DST_PROC(dm).d_ctl)(dm, what, arg)

/* ctl command */
#define DST_DoDefine		0	/* define words */
#define DST_DoDelete		1	/* delete words */
/* more commands (not implemented) */
#define DST_DoQuery		2	/* query on the dictionary info */

#define DST_DoPrint		9999	/* print debug information */


/* Internal Functions */
struct RkParam		*RkGetSystem();
struct RkContext	*RkGetContext(), *RkGetXContext();
struct RkKxGram		*RkReadGram(), *RkOpenGram();

/* HEAP */
int			_RkInitializeHeap();
void			_RkFinalizeHeap();
unsigned char 		*_RkNewHeap();
void			_RkFreeHeap();
/* CACHE */
int			_RkInitializeCache();
void			_RkFinalizeCache();
struct ncache		*_RkFindCache();
struct ncache		*_RkReadCache();
void			_RkFreeCache();
void		 	_RkKillCache();
void		 	_RkPurgeCache();
int			_RkEnrefCache(), _RkDerefCache();
/* WORD */
int			_RkInitializeWord();
void			_RkFinalizeWord();
int			_RkRenbun2();
void			_RkLearnBun();
/* WREC */
int			RkGetWrecSize();
unsigned char		*RkCopyWrec();
unsigned char		*RkUnionWrec();
unsigned char		*RkSubtractWrec();

int			RkScanWcand();
int			RkUniqWcand();
int			RkUnionWcand();
int			RkSubtractWcand();
unsigned char		*RkWcand2Wrec();

void 			_Rkdebugwrec();
unsigned char		*_Rkuparsewrec();
/* Clib functions */
char 			*malloc(), *calloc(), *realloc();
char 			*strchr();

/* Context */
int			_RkInitialize();
void			_RkBgnBun(), _RkEndBun();

extern unsigned char	*_Rkparsewrec(), *_Rkuparsewrec();

/* DEBUG stuff */
#ifdef RK_DEBUG
#define	RkDebug(fmt, p, q, r)	_Rkdebug(fmt, p, q, r)

#else

#define	RkDebug(fmt, p, q, r)

#endif


#endif	/* _RKintern_h */
/* don't add stuff after this line */
