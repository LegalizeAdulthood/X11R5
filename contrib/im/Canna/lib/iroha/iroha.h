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

/*
 * @(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/iroha.h,v 3.4 91/09/03 18:54:16 kon Exp $
 * $Revision: 3.4 $
 * $Author: kon $
 */
     
#include <stdio.h>
#include "sglobal.h"
#include <iroha/RK.h>
#include <iroha/jrkanji.h>

#include <memory.h>
#define bzero(buf, size) memset((char *)(buf), 0x00, (size))
#define bcopy(src, dst, size) memcpy((char *)(dst), (char *)(src), (size))

/*
 * LIBDIR  -- �����ƥ�Υ������ޥ����ե��������޻������Ѵ��ơ��֥뤬
 *            �֤����ǥ��쥯�ȥꡣ
 */

#ifndef LIBDIR
#define LIBDIR "/usr/lib/iroha"
#endif

#define XKanjiStatus          jrKanjiStatus
#define XKanjiStatusWithValue jrKanjiStatusWithValue

/*
 * glineinfo -- �������ɽ���Τ��������������Ǽ���Ƥ�������ι�¤�Ρ�
 * ���줾��Υ��Фϰʲ��ΰ�̣����ġ�
 *
 * glkosu -- ���ιԤˤ������ο�
 * glhead -- ���ιԤ���Ƭ���䤬��kouhoinfo�β����ܤ�(0���������)
 * gllen  -- ���ιԤ�ɽ�����뤿���ʸ�����Ĺ��
 * gldata -- ���ιԤ�ɽ�����뤿���ʸ����ؤΥݥ���
 */

typedef struct {
  int glkosu;
  int glhead;
  int gllen;
  unsigned char *gldata;
} glineinfo;

/*
 * kouhoinfo -- ��������Τ��������������Ǽ���Ƥ�������ι�¤��
 * ���줾��Υ��Фϰʲ��ΰ�̣����ġ�
 *
 * khretsu -- ���θ��䤬�����
 * khpoint -- ���θ���ιԤΤʤ��Ǥΰ���
 * khdata -- ���θ����ʸ����ؤΥݥ���
 */

typedef struct {
  int khretsu;
  int khpoint;
  unsigned char *khdata;
} kouhoinfo;

#define ROMEBUFSIZE 	1024
#define	BANGOSIZE	4	/* �������γƸ�����ֹ�ΥХ��ȿ� */
#define	REVSIZE		2	/* ���������ֹ�Υ�С����ΥХ��ȿ� */
#define	BANGOMAX   	9	/* ���������κ������� */
#define KIGOSIZE	2	/* �������ΥХ��ȿ� */
#define GL_KUHAKUSIZE	2	/* �����ֹ�ζ���ʸ���ΥХ��ȿ� */
#define KG_KUHAKUSIZE	2	/* ����δ֤ζ���ʸ���ΥХ��ȿ� */
#define KIGOCOL		(KIGOSIZE + KG_KUHAKUSIZE)
					/* bangomax��׻����뤿��ο� */
#define	KIGOBANGOMAX   	16	/* ���������κ������� */
#define HINSHISUU	14
#define HINSHIBUF	256
#define GOBISUU		9

#define	ON		1
#define	OFF		0

#define ZENHIRA		IROHA_MODE_ZenHiraKakuteiMode	/* 5 �������ϥ⡼�� */
#define ZENKATA		IROHA_MODE_ZenKataKakuteiMode	/* 6 �������ϥ⡼�� */
#define HANKATA		IROHA_MODE_HanKataKakuteiMode	/* 7 �������ϥ⡼�� */

#define	NG		-1

#define NO_CALLBACK	0
#define NCALLBACK	4

#define	JISHU_HIRA	0
#define JISHU_ZEN_KATA	1
#define JISHU_HAN_KATA	2
#define JISHU_ZEN_ALPHA	3
#define JISHU_HAN_ALPHA	4
#define MAX_JISHU	5

#define  SENTOU        1
#define  HENKANSUMI    2
#define  WARIKOMIMOJI  4
#define  SHIRIKIRE     8
#define  ATAMAKIRE    16

typedef char *mode_context;

struct callback {
  int (*func[NCALLBACK])();
  mode_context    env;
  struct callback *next;
};

/* identifier for each context structures */
#define CORE_CONTEXT       (char)0
#define YOMI_CONTEXT       (char)1
#define JISHU_CONTEXT      (char)2
#define HENKAN_CONTEXT     (char)3
#define ICHIRAN_CONTEXT    (char)4
#define FORICHIRAN_CONTEXT (char)5
#define MOUNT_CONTEXT      (char)6
#define TOUROKU_CONTEXT    (char)7
#define ADJUST_CONTEXT     (char)8

typedef struct _coreContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode; /* �������Υ⡼�� */
  mode_context    next;
} coreContextRec, *coreContext;

typedef struct  _yomiContextRec {
  /* core �����Ʊ������ */
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* �������Υ⡼�� */
  mode_context    next;

  /* ���޻������Ѵ��ط� */
  unsigned char   romaji_buffer[ROMEBUFSIZE];
  /* ���޻��Хåե��� rStartp, rEndp �Σ��ĤΥ���ǥå����ˤ�äƴ�������
   * �롣rStartp �ϥ��ʤ��Ѵ��Ǥ��ʤ��ä����޻��κǽ��ʸ���ؤΥ���ǥ�
   * �����Ǥ��ꡢrEndp �Ͽ����˥��޻������Ϥ�����ˡ���Ǽ���٤� 
   * romaji_buffer ��Υ���ǥå����Ǥ��롣���������Ϥ������޻��ϡ�
   * romaji_buffer + rEndp �����˳�Ǽ���졢���Υ��޻��򥫥ʤ��Ѵ���
   * ����ϡ�romaji_buffer + rStartp ���� rEndp - rStartp �Х��Ȥ�ʸ����
   * �оݤȤʤ롣 */
  int		  rEndp, rStartp, rCurs; /* ���޻��Хåե��Υݥ��� */
  unsigned char   kana_buffer[ROMEBUFSIZE];
  unsigned char   rAttr[ROMEBUFSIZE], kAttr[ROMEBUFSIZE];
  int		  kEndp; /* ���ʥХåե��κǸ�򲡤���ݥ��� */
  int             kRStartp, kCurs;

  /* ����¾�Υ��ץ���� */
  char		  generalFlags;		/* see below */
  char		  allowedChars;		/* see jrkanji.h */
  char		  henkanInhibition;	/* see below */
  char		  baseChar;		/* see below */
  int		  (*henkanCallback)();	/* �Ѵ���Ԥ����ˤ����ѿ��˥��ɥ쥹
             �����ꤵ��Ƥ���ʤ�Ф��Υ��ɥ쥹�δؿ����̾�δؿ��������
             �Ƥ֡����ꤵ��Ƥ��ʤ������̾�δؿ���Ƥ֡�
               �����Ѵ��ʤɤ��Ѵ������������줿�����ü�ʽ������Ԥ���Τ�
             �б����뤿����դ�����
	       �Ѵ�������Хå��ϰ����Ȥ��Ƥɤ��Ѵ��ؿ����ƤФ줿�Τ��򼨤�
	     �ֹ��ȼ����*/

  /* ������ѿ� */
  char		  *retbuf, *retbufp;
  int		  retbufsize;
} yomiContextRec, *yomiContext;

#define IROHA_YOMI_BREAK_ROMAN		1
#define IROHA_YOMI_CHGMODE_INHIBITTED	2
#define IROHA_YOMI_END_IF_KAKUTEI	4
#define IROHA_YOMI_DELETE_DONT_QUIT	8

#define IROHA_YOMI_INHIBIT_NONE		0
#define IROHA_YOMI_INHIBIT_HENKAN	1
#define IROHA_YOMI_INHIBIT_JISHU	2
#define IROHA_YOMI_INHIBIT_ASHEX	4
#define IROHA_YOMI_INHIBIT_ASBUSHU	8
#define IROHA_YOMI_INHIBIT_ALL		15

/* ��������Τ���Υե饰 */
#define NUMBERING 			1
#define CHARINSERT			2

#define IROHA_JISHU_UPPER		1
#define IROHA_JISHU_CAPITALIZE		2

typedef struct _jishuContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* ���Υ⡼�� */
  mode_context    next;

  unsigned char	 inhibition;
  unsigned char  jishu_buffer[ROMEBUFSIZE];
  int            jishu_len, jishu_kEndp, jishu_rEndp;
  unsigned char  kc, jishu_case;
  yomiContext    ycx;
} jishuContextRec, *jishuContext;

typedef struct _henkanContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* �������Υ⡼�� */
  mode_context    next;

  /* ���ʴ����Ѵ��ط� */
  int            context;
  int		 kouhoCount;	/* ���� henkanNext ��Ϣ³���Ʋ����줿�� */
  unsigned char  yomi_buffer[ROMEBUFSIZE];
  unsigned char  echo_buffer[ROMEBUFSIZE];
  unsigned char  **allkouho; /* RkGetKanjiList��������ʸ���������ˤ���
				�ȤäƤ����Ȥ��� */
  int            yomilen;    /* �ɤߤ�Ĺ�����ɤ߼��Ȥ� kana_buffer ������
			        ���� */
  int            curbun;     /* ������ʸ�� */
  int		 curIkouho;  /* �����ȸ��� */
  int            nbunsetsu;  /* ʸ��ο� */
#define MAXNBUNSETSU	256
  int            kugiri[MAXNBUNSETSU]; /* ʸ��ʬ����Ԥ�����ʸ�᤯��
					  ��ξ��� */
  int		 *kanaKugiri, *romajiKugiri, *jishubun;
  yomiContext    ycx;
/* ifdef MEASURE_TIME */
  long		 proctime;   /* ��������(�Ѵ��Ƿ�¬����) */
  long		 rktime;     /* ��������(RK�ˤ��������) */
/* endif MEASURE_TIME */
} henkanContextRec, *henkanContext;

typedef struct _ichiranContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* �������Υ⡼�� */
  mode_context    next;

  int            svIkouho;   /* �����ȸ�������ȤäƤ���(����ɽ����) */
  int            *curIkouho; /* �����ȸ��� */
  int            nIkouho;    /* ����ο�(����ɽ����) */
  int		 tooSmall;   /* �������������Ƹ���������Ф��ʤ���ե饰 */
  int            curIchar;   /* ̤����ʸ���󤢤��ñ����Ͽ��ñ�����Ϥ�
    							��Ƭʸ���ΰ��� */
  unsigned char  inhibit;
  unsigned char  flags;	     /* ���򸫤Ƥ� */
  unsigned char  **allkouho; /* RkGetKanjiList��������ʸ���������ˤ���
				�ȤäƤ����Ȥ��� */
  unsigned char  *glinebufp; /* ��������Τ����Ԥ�ɽ�����뤿���ʸ��
				��ؤΥݥ��� */
  kouhoinfo      *kouhoifp;  /* ��������ط��ξ�����Ǽ���Ƥ�����¤��
				�ؤΥݥ��� */
  glineinfo      *glineifp;  /* ��������ط��ξ�����Ǽ���Ƥ�����¤��
				�ؤΥݥ��� */
} ichiranContextRec, *ichiranContext;

/* �ե饰�ΰ�̣ */
#define ICHIRAN_ALLOW_CALLBACK 1 /* ������Хå��򤷤Ƥ��ɤ� */


typedef struct _forichiranContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* �������Υ⡼�� */
  mode_context    next;

  int            curIkouho;  /* �����ȸ��� */
  unsigned char  **allkouho; /* RkGetKanjiList��������ʸ���������ˤ���
				�ȤäƤ����Ȥ��� */
} forichiranContextRec, *forichiranContext;

typedef struct _mountContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* �������Υ⡼�� */
  mode_context    next;

  unsigned char   *mountOldStatus; /* �ޥ���Ȥ���Ƥ��뤫���ʤ��� */
  unsigned char   *mountNewStatus; /* �ޥ���Ȥ���Ƥ��뤫���ʤ��� */
  unsigned char  **mountList;   /* �ޥ���Ȳ�ǽ�ʼ���ΰ��� */
  int            curIkouho;     /* �����ȸ��� */
} mountContextRec, *mountContext;

typedef struct _tourokuContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* �������Υ⡼�� */
  mode_context    next;

  unsigned char  genbuf[ROMEBUFSIZE];
  unsigned char  qbuf[ROMEBUFSIZE];
  unsigned char  tango_buffer[ROMEBUFSIZE];
  int            tango_len;  /* ñ����Ͽ��ñ���ʸ�����Ĺ�� */
  unsigned char  yomi_buffer[ROMEBUFSIZE];
  int            yomi_len;   /* ñ����Ͽ���ɤߤ�ʸ�����Ĺ�� */
  int            curHinshi;  /* �ʻ������ */
  char		 hcode[16];   /* ñ����Ͽ���ʻ� */
  int		 katsuyou;   /* ñ����Ͽ��ư��γ��ѷ� */
  unsigned char  **udic;     /* ñ����Ͽ�Ǥ��뼭�� (����̾) */
  int            delContext; /* ñ�����ǣ��Ĥμ����ޥ���Ȥ��� */
} tourokuContextRec, *tourokuContext;

typedef struct _adjustContextRec {
  char id;
  char majorMode, minorMode;
  struct callback c;
  KanjiMode	  prevMode;	/* �������Υ⡼�� */
  mode_context    next;

  yomiContext    ycx;
  henkanContext  hcx;

  unsigned char  genbuf[ROMEBUFSIZE];
  int            kanjilen, strlen, bunlen; /* ������ʬ�����Ρ�ʸ���Ĺ�� */
} adjustContextRec, *adjustContext;

struct moreTodo {
  char          todo; /* ��äȤ���Ρ��򼨤� */
  char          fnum; /* �ؿ��ֹ档���ʤ鼡��ʸ���Ǽ�����뤳�Ȥ򤹤� */
  unsigned char ch;   /* ʸ�� */
};

/* �⡼��̾���Ǽ����ǡ����η���� */

struct ModeNameRecs {
  int           alloc;
  unsigned char *name;
};

/* 

  uiContext �ϥ��޻������Ѵ������ʴ����Ѵ��˻Ȥ��빽¤�ΤǤ��롣
  XLookupKanjiString �ʤɤˤ���Ѵ��ϡ�������ɥ���ʬΥ���줿ʣ������
  �ϥݡ��Ȥ��б����Ƥ���Τǡ�������Υ��޻��ξ���䡢���ʴ����Ѵ�
  ���ͻҤʤɤ򤽤줾��Υ�����ɥ����ʬΥ�����ݻ����Ƥ����ʤ���Ф�
  ��ʤ������ι�¤�ΤϤ��Τ���˻Ȥ��빽¤�ΤǤ��롣
 
  ��¤�ΤΥ��Ф��ɤΤ褦�ʤ�Τ����뤫�ϡ�����򻲾Ȥ��뤳��
 
 */

typedef struct {

  /* XLookupKanjiString�Υѥ�᥿ */
  unsigned char  *buffer_return;
  int            bytes_buffer;
  jrKanjiStatus  *kanji_status_return;

  /* XLookupKanjiString������ͤǤ���ʸ�����Ĺ�� */
  int		 nbytes;

  /* ����饯�� */
  unsigned char  ch;

  /* ���ߥ����Х�ǡ��� */
  int		 contextCache;	 /* �Ѵ�����ƥ����ȥ���å��� */
  int		 bushuContext;	 /* �����Ѵ�����ƥ����� */
  KanjiMode      current_mode;
  char		 majorMode, minorMode;	 /* ľ���Τ�� */

  short		 curkigo;	 /* �����ȵ���(��������) */
  char           currussia;	 /* �����ȵ���(����ʸ��) */
  char           curgreek;	 /* �����ȵ���(���ꥷ��ʸ��) */
  char           curkeisen;	 /* �����ȵ���(����) */
  short          curbushu;       /* ����������̾ */
  int            ncolumns;	 /* ��ԤΥ���������������λ����Ѥ����� */
  unsigned char  genbuf[ROMEBUFSIZE];	/* ���ѥХåե� */

  /* �ꥹ�ȥ�����Хå���Ϣ */
  char           *client_data;   /* ���ץꥱ��������ѥǡ��� */
  void           (*list_func)(); /* �ꥹ�ȥ�����Хå��ؿ� */

  /* ����¾ */
  char		 flags;		 /* ���򸫤Ƥ� */
  char		 status;	 /* �ɤΤ褦�ʾ��֤��֤ä��Τ��򼨤���
				    ���Υ⡼�ɤ�
				     ��������
				     ��������λ
				     ����������
				     ������¾
				    �ʤɤ��ͤ����롣(���򸫤�) */

  /* ������Хå��������� */
  struct callback *cb;

  /* ��äȤ���Ȥ��������Ȥ�����¤�� */
  struct moreTodo more;

  /* ���֥���ƥ����ȤؤΥ�� */
  mode_context   modec;		/* ���������ˤĤʤ�ͽ�� */
} uiContextRec, *uiContext;

/* uiContext �� flags �ΥӥåȤΰ�̣ */
#define PLEASE_CLEAR_GLINE	1	/* GLine ��ä��Ƥ� */
#define PCG_RECOGNIZED		2	/* GLine �򼡤Ͼä��ޤ��� */
#define MULTI_SEQUENCE_EXECUTED	4	/* ���ä��ޥ���������󥹤��Ԥ�줿 */

int defaultContext, defaultBushuContext;

#define EVERYTIME_CALLBACK	0
#define EXIT_CALLBACK		1
#define QUIT_CALLBACK		2
#define AUX_CALLBACK		3

/* ���޻������Ѵ����� */
/*
 * ���޻������Ѵ��ơ��֥�ϣ��Ĥ�����ɤ��Ǥ��礦��ʣ����ɬ�פʤΤ�
 * ����� RomeStruct �Υ��ФȤ�������Ƥ���ɬ�פ⤢��ޤ��礦��...��
 * �λ��Ϥ��λ��ǹͤ��ޤ��礦��
 */
     
extern struct RkRxDic *romajidic, *RkOpenRoma();

/*
 * �����̾��������Ƥ����ѿ�
 */

#define MAX_DICS 16

extern char *kanjidicname[];
extern int  nkanjidics;

extern char *userdicname[];
extern int  nuserdics;
extern char userdicstatus[];

extern char *bushudicname[];
extern int nbushudics;

extern char *localdicname[];
extern int nlocaldics;

/*
 * ���顼�Υ�å�����������Ƥ����ѿ�
 */

extern char *necKanjiError;

/*
 * �ǥХ�ʸ��ɽ�����뤫�ɤ����Υե饰
 */

extern iroha_debug;

/*
 * 16�ʥ��������Ϥ�����Ԥ�ɽ�����뤫�ɤ�����Ĵ�٤��
 */

#define hexGLine(plen) (d->ncolumns >= plen + 4)

/*
 * �����������󥹤�ȯ������褦�ʥ���
 */

#define IrohaFunctionKey(key) \
  ((0x80 <= (int)(unsigned char)(key) &&  \
    (int)(unsigned char)(key) <= 0x8b) || \
   (0x90 <= (int)(unsigned char)(key) &&  \
    (int)(unsigned char)(key) <= 0x9b) || \
   (0xe0 <= (int)(unsigned char)(key) &&  \
    (int)(unsigned char)(key) <= 0xff) )

/* selectOne �ǥ�����Хå���ȼ�����ɤ�����ɽ���ޥ��� */

#define WITHOUT_LIST_CALLBACK 0
#define WITH_LIST_CALLBACK    1

/*
 * Rk �ؿ���ȥ졼�����뤿���̾���ν񤭴�����
 */

#ifdef DEBUG
#include "traceRK.h"
#endif /* DEBUG */

/*
 * �ǥХ���å����������ѤΥޥ���
 */

#ifdef DEBUG
#define debug_message(fmt, x, y, z)	dbg_msg(fmt, x, y, z)
#else /* !DEBUG */
#define debug_message(fmt, x, y, z)
#endif /* !DEBUG */

/*
 * malloc �ΥǥХ�
 */

#ifdef DEBUG_ALLOC
extern int fail_malloc;
#define malloc(n) debug_malloc(n)
#endif /* DEBUG_MALLOC */
