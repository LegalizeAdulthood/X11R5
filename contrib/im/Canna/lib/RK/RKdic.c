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

#ifndef lint
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKdic.c,v 1.10 91/08/09 16:56:00 mako Exp $";
#endif

#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <ctype.h>
  
#include	"RK.h"
#include	"RKintern.h"
#include	"RKdic.h"


/* ClearMountDic(dm)
 * �ޥ���Ȥ���Ƥ�����ꤵ�줿����(dm)����Ȥ򤫤��(�����)���롣
 * ���ꤵ�줿���񤬥ƥ����ȥե�����Ǥʤ��ä�����-1���֤���
 */
  
ClearMountDic(dm)
struct DM             *dm;
{
  struct TD             *xdm = (struct TD *)dm->dm_extdata;
  
  if (!dm || !xdm) return -1; /* �ƥ����ȥե�����Ǥʤ��ä� */
  
  _RkKillCache(dm);
  freeTD((struct TD *)xdm); /* TD�򤹤٤�free���롣 */  
  /*
   * TD���������롣
   */
  if (!(dm->dm_extdata = (char *)newTD())) {
    return -1;
  }
  return 0;
}

/* ClearUnmountDic(dm)
 * �ޥ���Ȥ���Ƥ��ʤ����ꤵ�줿����(�ե�����)����Ȥ򥯥ꥢ���롣
 * ���ꤵ�줿����ե�����Υ����ץ�˼��Ԥ�����-1���֤���
 */

ClearUnmountDic(dm)
struct DM             *dm;
{
  struct DF   	        *df;
  struct DD   	        *dd;
  FILE                  *fp;
  char                  file_path[512];
  
  df = dm->dm_file;
  dd = df->df_direct;
  
  /* �ե��������Ȥ򥯥ꥢ���롣 */
  (void)sprintf(file_path ,"%s/%s",dd->dd_path, df->df_link);
  
  if((fp = fopen(file_path, "w")) == NULL) {
    return -1; /* �ե�����Υ����ץ�˼��Ԥ����� */
  }
  (void)fclose(fp);
  return 0;
}


/* SearchMountDic(dm)
 * ���٤ƤΥ���ƥ����Ȥǻ��ꤵ�줿���񤬥ޥ�����椫Ĵ�٤롣
 * �ޥ������Ǥ��륳��ƥ����Ȥ����ĤǤ�¸�ߤ����"1"���֤���
 * �ޥ������Ǥ��륳��ƥ����Ȥ����Ĥ�¸�ߤ��ʤ����"0"���֤���
 */

SearchMountDic(dm)
struct DM *dm;
{
  register struct RkContext *cx;
  register int i;
  register int p;
  struct MD		*mh, *md;
  
  if(!strcmp(dm->dm_file->df_direct->dd_name, "iroha")) return 0;
  
  for ( i = 0; i < MAX_CONTEXT; i++) {
    if(cx = RkGetContext(i)) {
      for ( p = 0; p < 4; p++ ) {
	mh = cx->mount[p];
	for ( md = mh->md_next; md != mh; md = md->md_next ) {
	  if(dm == md->md_dic) {
	    return 1;
	  }
	}
      }
    }
  }
  return 0;
}

/* SearchConvertDic(dm)
 * ���٤ƤΥ���ƥ����Ȥǻ��ꤵ�줿���񤬻����椫Ĵ�٤롣
 * ������Ǥ��륳��ƥ����Ȥ����ĤǤ�¸�ߤ����"1"���֤���
 * ������Ǥ��륳��ƥ����Ȥ����Ĥ�¸�ߤ��ʤ����"0"���֤���
 */

SearchConvertDic(dm)
struct DM *dm;
{
  register struct RkContext	*cx;
  register int i;
  register int p;
  struct MD		*mh, *md;
  
  if(!strcmp(dm->dm_file->df_direct->dd_name, "iroha")) {
    return 0;
  }
  
  for ( i = 0; i < MAX_CONTEXT; i++) {
    if(cx = RkGetContext(i)) {
      if(IS_XFERCTX(cx)) { /* ����ƥ����Ȥ��������Ѵ��椫�� */
	for ( p = 0; p < 4; p++ ) {
	  mh = cx->mount[p];
	  for ( md = mh->md_next; md != mh; md = md->md_next ) {
	    if(md->md_dic == dm) {
              return 1;
            }
	  }
	}
      }
    }
  }
  return 0;
}

/* ClearDic( dm )
 * ���٤Ƥ��Ѵ���Ǥʤ�����ƥ����Ȥ򸡺����ơ�
 * �ޥ���Ȥ��Ƥ��륳��ƥ����Ȥ����ĤǤ⤢��С�ClearMountDic��Ƥ֡�
 * �ޥ���Ȥ��Ƥ��륳��ƥ����Ȥ����Ĥ�̵�����ϡ�ClearUnmountDic��Ƥ֡�
 * ���ꤵ�줿���񤬥����ƥ༭��ʤ饯�ꥢ���ʤ���-1���֤���
 * ClearMountDic��ClearUnmountDic�˼��Ԥ�����-1���֤���
 * 
 * ��)���δؿ��Ǥϡ��Ѵ���Ǥʤ�����ƥ����Ȥ����������ʤ����ᡢ
 * �Ѵ���Υ���ƥ����Ȥǥޥ���Ȥ��Ƥ��뼭��Ϻ�����ޤ���
 */

ClearDic( dm )
struct DM	        *dm;
{
  register struct RkContext *cx;
  register int i;
  register p;
  struct MD		*mh, *md;  
  
  if(!strcmp(dm->dm_file->df_direct->dd_name, "iroha")) return -1;
  for ( i = 0; i < MAX_CONTEXT; i++) {
    if(cx = RkGetContext(i)) {
      if(!IS_XFERCTX(cx)) {
	for ( p = 0; p < 4; p++ ) {
	  mh = cx->mount[p];
	  for ( md = mh->md_next; md != mh; md = md->md_next ) {
	    if(md->md_dic == dm) {
	      if( ClearMountDic(dm) ) { 
		return -1;
	      }
	      return 0;
	    }
	  }
	}
      }
    }
  }
  if( ClearUnmountDic(dm))  {
    return -1;
  }
  return 0;
}

/*
 * struct DM	*
 * SearchUDDP(ddp, name)
 * ���ꤵ�줿ddp����Υ����ƥ༭��ǥ��쥯�ȥ�Ǥʤ��ǥ��쥯�ȥ�ˡ�
 * ���ꤵ�줿����¸�ߤ��뤫�ɤ���Ĵ�٤롣
 * ¸�ߤ����餽�μ���(dm)�ؤΥݥ��󥿤��֤���
 * ¸�ߤ��ʤ��ä���NULL�ݥ��󥿤��֤���
 */

struct DM	*
SearchUDDP(ddp, name)
struct DD	**ddp;
unsigned char	*name;
{
  struct DD	*dd;
  struct DF	*f, *fh;
  struct DM	*m, *mh;
  int		i;
  
  if ( ddp ) {
    for ( i = 0; dd = ddp[i]; i++ ) {
      if(strcmp(dd->dd_name, "iroha")) { /* �����ƥ༭���̵�������� */
	fh = &dd->dd_files; /* �ǥ��쥯�ȥ�����ƤΥե������Ĵ�٤롣 */
	for ( f = fh->df_next; f != fh; f = f->df_next ) {
	  mh = &f->df_members; /* �ե���������Ƥμ����Ĵ�٤롣 */
	  for ( m = mh->dm_next; m != mh; m = m->dm_next ) {
	    if (!strcmp((char *)(m->dm_nickname), (char *)name) ) {
	      /* ����̾�����פ����� */
	      return m;
	    }
	  }
	}
      }
    }
  }
  /* ���ꤵ�줿�ǥ��쥯�ȥ�ˤϻ��ꤵ�줿�����¸�ߤ��ޤ���͡� */
  return (struct DM *)0;
}


/* int
 * RkCreateDic(cx_num, dicname, mode)
 *
 * ������
 *         int            cx_num    ����ƥ����ȥʥ�С�
 *	   unsigned char  *dicname  ����ؤΥݥ���
 *	   int            mode      ����μ���ȶ����⡼�ɤ�OR
 *             ����μ��� 
 *                 #define	Rk_MWD		0x80
 *                 #define	Rk_SWD		0x40
 *                 #define	Rk_PRE		0x20
 *                 #define	Rk_SUC		0x10
 *             �����⡼��
 *                 #define KYOUSEI	        0x01
 *                 �������ʤ����           0x00
 *
 * �꥿������
 *             �����������                                 0
 *             �����������(��񤭤������)                 1
 *             �����������˼��Ԥ������                -6  NOTALC
 *             ���񤬥Х��ʥ꼭��Ǥ��ä����              -9    BADF
 *             dics.dir�˰۾郎���ä����                 -10   BADDR
 *             GetDicFilename���֤��ͤ�-1�ξ��           -13   ACCES
 *             MakeDicFile�˼��Ԥ������                  -13   ACCES
 *             CleatDic�˼��Ԥ������                     -13   ACCES
 *             ���񤬥ޥ������Ǥ��ä����               -16   MOUNT
 *             ���񤬤��Ǥˤ�����(�����Ǥʤ����)       -17   EXIST
 *             ���񤬻�����Ǥ��ä����                   -26  TXTBSY
 *             mode���۾��ͤǤ��ä����                   -99  BADARG
 *             ����ƥ����ȹ�¤�Τ�¸�ߤ��ʤ����        -100 BADCONT
 */

int
RkCreateDic(cx_num, dicname, mode)
int cx_num;
unsigned char *dicname;
int     mode;
{
  struct RkContext	*cx = RkGetContext(cx_num);
  struct DM             *dm;
  struct DD   	        **ddp;

  if (!cx ||
      !(ddp = cx->ddpath)) {
      return BADCONT;
  }
  if (!dicname || *dicname == NULL) return ACCES;
  
  /* Ʊ������̾�μ���¸�ߤ��뤫Ĵ�٤ޤ��� */
  if (!(dm = SearchUDDP(ddp, dicname))) {
    /* ������������������ */
      return (CreateNewDic(ddp, mode, dicname));
  }
  /* ��¸�μ���˾�񤭤��� */
  return (RefreshOldDic(dm, mode));
}

/*
 * RefreshOldDic(dm, mode)
 * dm��¸�ߤ�����ν���
 */
RefreshOldDic(dm, mode)
struct DM  *dm;
int        mode;
{
  unsigned char	*check_file;
  unsigned char	file_path[512];

  /* textdic�ɤ���Ĵ�٤� */
  if (DM2TYPE(dm) != DF_TEMPDIC ) {  
    return BADF;
  }
  /* �����⡼�ɤʤ鼭��򤫤�ˤ��� */
  if ((mode & KYOUSEI) != KYOUSEI) {
    return EXIST; /* �����⡼�ɤǤʤ��ΤǼ������ʤ����֤� */
  }
  /* ����μ��ब��狼�ɤ��� */
  if (!(ND2RK(dm->dm_class) & mode)) {
    return INVAL;
  }
  /* �ޥ�����椫Ĵ�٤� */
  if (SearchMountDic(dm)) {
    return TXTBSY;
  }

  /*
   * dm����٤�dics.dir�����������ɤ��������å����ޤ���
   */
  if (!(check_file = CheckDicFile(dm))) {
    return BADDR; /* dics.dir��̵���ä��� */
  }
  (void)sprintf((char *)file_path, "%s/%s",
		dm->dm_file->df_direct->dd_path,
		check_file);
  if (access((char *)file_path, F_OK) == -1) {
    if (MakeDicFile(dm,
		    dm->dm_nickname,
		    (mode & (~KYOUSEI)),
		    (char *)check_file) == -1) {
	return ACCES;
      }
  }
  if (ClearUnmountDic(dm) == -1) {
    return ACCES;
  }
  return 1;
}

int
CreateNewDic(ddp, mode, dicname)
struct DD **ddp;
int	mode;
unsigned char	*dicname;
{
  struct DD		*dd = (struct DD *)0;
  struct DF		*df;
  struct DM             *dm;
  unsigned char         *filename;
  int                   i;

  for (i = 0 ; ddp[i] ; i++) {
    if (STRCMP(ddp[i]->dd_name, "iroha")) {
      dd = ddp[i];
      break;
    }
  }
  if (dd == (struct DD *)0) {
    return BADCONT;
  }

  filename = GetDicFilename(dd);
  if(!filename) {
    return ACCES;
  }
  if(!(df = allocDF(dd, filename))) {
#ifdef DEBUG
    Debug ("RkCreateDic: err allocDF");
#endif
    return NOTALC;
  }

  df->df_type = DF_TEMPDIC;

  if ((mode & ~KYOUSEI) == Rk_SWD) {
    if (!(dm = allocDM(df, ".swd" , dicname))) {
      (void)freeDF(df);
#ifdef DEBUG
      Debug ("RkCreateDic: err alloDM");
#endif
      return NOTALC;
    }
    dm->dm_class = ND_SWD;
  } else if ((mode & ~KYOUSEI) == Rk_MWD) {

    if(!(dm = allocDM(df, ".mwd" , dicname))) {
#ifdef DEBUG
      Debug ("RkCreateDic: err alloDM");
#endif
      (void)freeDF(df);
      return NOTALC;
    }
    dm->dm_class = ND_MWD;
  } else {
#ifdef DEBUG
    Debug ("RkCreateDic: err mode %x", mode);
#endif
    return BADARG;
  }
  
  if (MakeDicFile(dm,
		  (char *)dicname,
		  (mode & (~KYOUSEI)),
		  filename) == -1) {
#ifdef DEBUG
    Debug ("error: cannot MakeDicFile");
#endif
    freeDF(df);
    return ACCES; /* �ե����������Ǥ��ޤ��� */
  }
  
#ifdef RK_TEST
  /*
   * �������������ޥ���Ȥ��ƥ����å����ޤ���
   */
  if (RkMountDic(cx_num,dicname,0) == -1){
    (void)fprintf(stderr,"����\"%s\"�Υޥ���Ȥ˼��Ԥ��ޤ���\n",
		  dicname);
  } else {
    (void)fprintf(stderr,"����\"%s\"�Υޥ���Ȥ��������ޤ���\n",
		  dicname);
    RkUnmountDic(cx_num,dicname);
  }
#endif
  return 0; /* �����������������ޤ����� */
}

/*
 * RkListDic(cx_num, dirname, buf, size)
 * int  cx_num;             ����ƥ����ȥʥ�С�
 * unsigned char *dirname;  ����ꥹ�Ȥ���Ϥ������ǥ��쥯�ȥ�̾
 * unsigned char *buf;      ����ꥹ�Ȥ��֤äƤ���Хåե�
 * int  size;               �Хåե��Υ�����
 *
 * �꥿������               
 *             �����������                      ����ο�
 *             ����ƥ����ȥʥ�С�����ξ��          BADCONT
 *             RkCreateContext�˼��Ԥ������           BADCONT
 *             RkSetDicPath�˼��Ԥ������              NOTALC
 */

RkListDic( cx_num, dirname, buf, size )
int  cx_num;
unsigned char *dirname;
unsigned char *buf;
int  size;
{
  int dicscnt;
  int new_cx_num;
  
  if (cx_num < 0 ||
      !dirname ||
      !buf) {
    return BADCONT;
  }
  if((new_cx_num = RkCreateContext(cx_num)) < 0)
    return BADCONT;
  
  if (RkSetDicPath(new_cx_num, (char *)dirname) == -1) {
    RkCloseContext(new_cx_num);
    return NOTALC;
  }

  dicscnt = RkGetDicList(new_cx_num, (char *)buf, size);

#ifdef DEBUG
  if (dicscnt < 0)
    return -1;
#endif
  
#ifdef DEBUG
  if (RkCloseContext(new_cx_num) < 0)
    return -1;
#else
  RkCloseContext(new_cx_num);
#endif
  
  return (dicscnt);
}

/* RemoveDic( dm )
 *
 * �������ǻ��ꤵ�줿���񤬤ɤΥ���ƥ����ȤǤ�ޥ���Ȥ���Ƥ��ʤ���С�
 * ���μ���������롣
 *
 * ������
 *             dm ��������������dm��¤�ΤؤΥݥ���
 *
 * �꥿������
 *             �����������                             0
 *             �ޥ���Ȥ��Ƥ������                    -1
 *             RemoveDicFile���֤��ͤ�-1�ξ��         -1
 */

RemoveDic( dm )
struct DM	        *dm;
{
  /* �ɤ��Ǥ�ޥ���Ȥ��Ƥ��ʤ��ä��Τǥե�������˲����ޤ��� */
  if (RemoveDicFile( dm ) == -1) {
    return -1;
  }
  (void)freeDF( dm->dm_file );
  return 0;
}

/* int
 * RkRemoveDic(cx_num, dicname)
 *
 * ���ꤵ�줿����ƥ����Ȥ˻��ꤵ�줿����¸�ߤ����
 * ���μ���������롣
 *
 * ������
 *             int            cx_num     ����ƥ����ȥʥ�С�
 *             unsigned char  *dicname   ����̾
 *
 * �꥿������
 *             �����������                             0
 *             ����¸�ߤ��ʤ����                    -2   NOENT
 *             ���񤬥Х��ʥ꼭��Ǥ��ä����          -9    BADF
 *             RemoveDic���֤��ͤ�-1�ξ��            -13   ACCES
 *             �ޥ���Ȥ��Ƥ������                   -26  TXTBSY
 *             ����ƥ����ȹ�¤�Τ�¸�ߤ��ʤ����    -100 BADCONT
 */

int
RkRemoveDic(cx_num, dicname)
int cx_num;
unsigned char *dicname;
{
  struct RkContext	*cx = RkGetContext(cx_num);
  struct DM             *dm;
  struct DD   	        **ddp;
  
  if ( !cx ||
      !dicname ||
      !(ddp = cx->ddpath) )
    return BADCONT; /* ���ꤵ�줿����ƥ�����NO���� */
  
  /* Ʊ������̾�Υ桼������¸�ߤ��뤫Ĵ�٤� */
  if((dm = SearchUDDP(ddp, dicname)) == (struct DM *)NULL )
    return NOENT; /* ���ꤵ�줿�����¸�ߤ��ޤ��� */
  
  /* ����¸�ߤ�����textdic��Ĵ�٤� */
  if (DM2TYPE(dm) != DF_TEMPDIC ) {
    return BADF; /* �Х��ʥ꼭��Ͼä��ޤ��� */
  }
  /* �ޥ������ʤ������ʤ� */
  if (SearchMountDic(dm)) {
    return TXTBSY;
  }

  /* �������˼��Ԥ����� return ACCES */
  if (RemoveDic(dm) == -1) {
    return ACCES;
  }
  return 0; /* ����������� */
}

/* int
 * RkRenameDic(cx_num, dicname1, dicname2, mode)
 *
 * ���ꤵ�줿����ƥ����Ȥ˻��ꤵ�줿����¸�ߤ����
 * ���μ���������롣
 *
 * ������
 *          int            cx_num           ����ƥ����ȥʥ�С�
 *          unsigned char  *dicname1        �ѹ�������̾
 *          unsigned char  *dicname2        �ѹ��輭��̾
 *          int            mode             �����⡼��
 *
 * �֤��� (RKdic.h����)
 *          �����������                             0
 *          dicname1��¸�ߤ��ʤ����                -2     NOENT
 *          RemoveDic���֤��ͤ�-1�ξ��             -2     NOENT
 *          ���񤬥Х��ʥ꼭��Ǥ��ä����          -9      BADF
 *          RenameDicFile���֤��ͤ�-1�ξ��        -13     ACCES
 *          dicname2��¸�ߤ�����                 -17     EXIST
 *          dicname1��ޥ���Ȥ��Ƥ������         -26    TXTBSY
 *          dicname2��ޥ���Ȥ��Ƥ������         -26    TXTBSY
 *          ����ƥ����ȹ�¤�Τ�¸�ߤ��ʤ����    -100   BADCONT
 */

int
RkRenameDic(cx_num, dicname1, dicname2, mode)
  int cx_num;
  unsigned char *dicname1;
  unsigned char *dicname2;
  int mode;
{
    struct RkContext	*cx = RkGetContext(cx_num);
    struct DM             *dm1, *dm2;
    struct DD   	        **ddp;
    
    if (!cx ||
 	!(ddp = cx->ddpath) ) {
	return BADCONT;
    }
    if (!dicname1 || !dicname2) return ACCES;

    /* RENAME���μ��񤬤ʤ���� return NOENT */
    if ((dm1 = SearchUDDP(ddp, dicname1)) == (struct DM *)NULL ) {
	return NOENT;
    }

    /* ����¸�ߤ�����textdic��Ĵ�٤� */
    if (DM2TYPE(dm1) != DF_TEMPDIC ) {
      return BADF; /* �Х��ʥ꼭���rename�Ǥ��ޤ��� */
    }
    /* RENAME���μ��񤬥ޥ������ʤ� return TXTBSY */
    if (SearchMountDic(dm1)) {
      return TXTBSY;
    }

    /* RENAME��μ��񤬤���к������ */
    if ((dm2 = SearchUDDP(ddp, dicname2)) != (struct DM *)NULL ) {
      
      /* ����¸�ߤ�����textdic��Ĵ�٤� */
      if (DM2TYPE(dm2) != DF_TEMPDIC ) {
	return BADF; /* �Х��ʥ꼭��Ͼä��ޤ��� */
      }
      return (rk_del_rename(dm1, dm2, (char *)dicname2, mode));
    }
    else {
      /* RENAME��μ���̵�����ν��� */
      return (rk_rename(dm1, (char *)dicname2));
    }
}

int rk_rename(dm, dicname)
  struct  DM *dm;
  char *dicname;
{
    char *buff;
    unsigned size;
    
    size = (unsigned)(strlen((char *)dicname) + 1);
    if (!(buff = (char *)malloc(size)))
	return NOTALC;
    (void)strcpy(buff, (char *)dicname);
    
    if (RenameDicFile(dm, dicname)) {
	(void)free((char *)buff);
	return ACCES;
    }

    (void)free((char *)dm->dm_nickname);
    dm->dm_nickname = buff;
    return (0);
}

int rk_del_rename(dm_src, dm_obj, dicname_obj, mode)
  struct DM *dm_src;
  struct DM *dm_obj;
  char *dicname_obj;
  int	mode;
{
    char *buff;
    unsigned size;

/* �ޥ������ʤ饨�顼 */
    if (SearchMountDic(dm_obj))
	return TXTBSY;

/* �����⡼�ɤǤʤ��ä����֤� */
    if ((mode & KYOUSEI) != KYOUSEI)
	return EXIST;

/* allocate new area */
    size = strlen((char *)dicname_obj) + 1;
    if (!(buff = (char *)malloc(size)))
	return NOTALC;
    (void)strcpy((char *)buff, (char *)dicname_obj);

    /* handle file of dictionary
     *
     * BUG:
     *	���ɤ��餫���Ԥ�����硢�⤦�����򸵤��᤻�ޤ���
     */

    if (RemoveDicFile(dm_obj) ||
	RenameDicFile(dm_src, dicname_obj)) {
	(void)free((char *)buff);
	return ACCES;
    }
    
/* free */
    (void)freeDF(dm_obj->dm_file);
    (void)free((char *)dm_src->dm_nickname);
    dm_src->dm_nickname = buff;

    return (0);
}

/*
 * RkGetWordTextDic(cx_num, dirname, dicname, info, infolen)
 *
 * int            cx_num      ����ƥ�����NO
 * unsigned char  *dirname    �ǥ��쥯�ȥ�̾
 * unsigned char  *dicname    ����̾
 * unsigned char  *info       �Хåե�
 * int            infolen     �Хåե���Ĺ��
 *
 * �֤��� : �ºݤ�info�����ä�Ĺ��
 *          �Ǹ�ޤ��ɤ�Ǥ�����          �����֤�
 *          RkCreateContext�˼��Ԥ���     BADCONT
 *          RkDuplicateContext�˼��Ԥ���  BADCONT
 *          RkGetContext�˼��Ԥ���        BADCONT
 *          RkSetDicPath�˼��Ԥ���        NOTALC
 *          RkMountDic�˼��Ԥ���          NOENT
 *          SearchUDDP�˼��Ԥ���          NOENT
 *          �Х��ʥ꼭����ä�                          -9   BADF
 *          dics.dir�˰۾郎���ä����                 -10   BADDR
 */

int
RkGetWordTextDic(cx_num, dirname, dicname, info, infolen)
int cx_num;
unsigned char *dirname;
unsigned char *dicname;
unsigned char *info ;
int infolen ;
{
  struct RkContext *new_cx, *cx;
  struct DM *dm;
  int new_cx_num;
  struct TD *initial_td;
  unsigned size;
  unsigned char *buff = NULL;

  if (!dirname ||
      !dicname ||
      !info ||
      !(cx = RkGetContext(cx_num))) {
    return BADCONT;
  }

  /* �����ܤν��� */
  if(dicname[0] != '\0') {

/* area for cx->gwt_dicname */
    size = strlen((char *)dicname) + 1;
    if (!(buff = (unsigned char *)malloc(size))) {
	return (NOTALC);
    }
    (void)strcpy((char *)buff, (char *)dicname);

    /* ¾�ͤμ���򸫤�������SetDicPath���� */
    if(dirname[0] != '\0') {
      /* ����������ƥ����Ⱥ��� */
      if((new_cx_num = RkCreateContext()) < 0) {
	  (void)free((unsigned char *)buff);
	  return BADCONT;
      }
      if(RkSetDicPath(new_cx_num, (char *)dirname) < 0) {
	RkCloseContext(new_cx_num);
	(void)free((unsigned char *)buff);
	return NOTALC;
      }
    }
    else {
      if ((new_cx_num = RkDuplicateContext(cx_num)) < 0) {
#ifdef DEBUG
	Debug ("irregal context number, %d\n", cx_num);
#endif
	(void)free((unsigned char *)buff);
	return BADCONT;
      }
    }

    if (!(new_cx = RkGetContext(new_cx_num))) {
      RkCloseContext(new_cx_num);
      (void)free((unsigned char *)buff);
      return BADCONT;
    }
    if (cx->gwt_cx >= 0) {
      RkCloseContext(cx->gwt_cx);
      cx->gwt_cx = -1;
    }
  
    /* �����ޥ���Ȥ��ޤ� */
    if(!STRCMP(dirname,"iroha")) {
      if (!(dm = searchDDP(new_cx->ddpath, (char *)dicname))) {
	RkCloseContext(new_cx_num);
	(void)free((unsigned char *)buff);
	return NOENT;
      }
    }
    else {
      if (!(dm = SearchUDDP(new_cx->ddpath, dicname))) {
	RkCloseContext(new_cx_num);
	(void)free((unsigned char *)buff);
	return NOENT;
      }
    }
    if (DM2TYPE(dm) != DF_TEMPDIC ) {
      RkCloseContext(new_cx_num);
      (void)free((unsigned char *)buff);
      return BADF; /* �Х��ʥ꼭����ä� */
    }
    if(RkMountDic(new_cx_num, (char *)dicname,0) == -1) {
      RkCloseContext(new_cx_num);
      (void)free((unsigned char *)buff);
      return NOMOUNT; /* ���񤬥ޥ���ȤǤ��ޤ���Ǥ��� */
    }
    if (!(CheckDicFile(dm))) {
      RkCloseContext(new_cx_num);
      (void)free((unsigned char *)buff);
      return BADDR; /* dics.dir��̵���ä��� */
    }

    cx->gwt_cx = new_cx_num;

    if (cx->gwt_dicname) {
      (void)free((unsigned char *)cx->gwt_dicname);
      cx->gwt_dicname = (unsigned char *)NULL;
    }
    cx->gwt_dicname = buff;
    initial_td = (struct TD *)dm->dm_extdata;
  }

  /* �����ܰʹߤν��� */
  else {
    if ((new_cx_num = cx->gwt_cx) < 0) {
      (void)free((unsigned char *)buff); /* �������̤�ΤϤȤƤ�۾�Ǥ� */
      return BADCONT;
    }
    if (!(new_cx = RkGetContext(new_cx_num))) {
      (void)free((unsigned char *)buff); /* �������̤�ΤϤȤƤ�۾�Ǥ� */
      return BADCONT;
    }
    initial_td = (struct TD *)0;
  }
  /* ���̤ν��� */

  if (GetLine(new_cx,SX.gramdic,(struct TD *)initial_td,info,infolen) < 0) {
    RkUnmountDic(new_cx_num, (char *)cx->gwt_dicname);
    RkCloseContext(cx->gwt_cx);
    cx->gwt_cx = -1;
    if (cx->gwt_dicname) {
      free(cx->gwt_dicname);
      cx->gwt_dicname = (unsigned char *)NULL;
    }
    return 0;
  }
  infolen = strlen ((char *)info);
  return infolen;
}

struct td_n_tupple *
pushTdn(cx, tdp)
RkContext *cx;
struct TD *tdp;
{
  struct td_n_tupple *new;

  new = (struct td_n_tupple *)malloc(sizeof(struct td_n_tupple));
  if (new == (struct td_n_tupple *)0) {
    return (struct td_n_tupple *)0;
  }
  new->td = (char *)tdp;
  new->n = 0;
  new->next = cx->tdn;
  cx->tdn = new;
  return new;
}


freeTdn(cx)  /* tdn �� next �򤿤ɤ�ʤ��� free ���� */
RkContext *cx;
{
  struct td_n_tupple *work;
  
  while(work = cx->tdn) {
    cx->tdn = work->next;
    (void)free(work);
  }
}

void
popTdn(cx)
RkContext *cx;
{
  struct td_n_tupple *work;

  work = cx->tdn;
  if (work) {
    cx->tdn = work->next;
    (void)free(work);
  }
}

/*
 * GetLine(cx, gram, tdp, line, linelen)
 * RkContext            *cx
 * struct RkKxGram	*gram
 * struct TD            *tdp
 * unsigned char	*line
 * int                  linelen
 *
 * �֤��� ����  0
 *        ���� -1
 */

GetLine(cx, gram, tdp, line, linelen)
RkContext        *cx;
struct RkKxGram	 *gram;
struct TD        *tdp;
unsigned char	 *line;
int              linelen;
{
  struct TD *vtd;
  struct TN *vtn;

  if (tdp) {
    if (cx->tdn) {
      freeTdn(cx);
    }

    if(!pushTdn(cx, tdp)) {
      return NOTALC; /* ��������¤�Τ�alloc�˼��� */
    }
  }
  while (cx->tdn && cx->tdn->n >= ((struct TD *)cx->tdn->td)->td_n) {
    popTdn(cx);
  }
  if (cx->tdn == (struct td_n_tupple *)0) {
    return -1;
  }
  vtd = (struct TD *)cx->tdn->td;
  vtn = vtd->td_node + cx->tdn->n;
  while ( !IsWordNode(vtn) ) { /* �ĥ꡼�ʤ� */
    cx->tdn->n++;
    if(!pushTdn(cx, vtn->tn_tree)) {
      return NOTALC; /* ��������¤�Τ�alloc�˼��� */
    }
    vtd = (struct TD *)cx->tdn->td;
    vtn = vtd->td_node;
  }
  if (RkUparseWrec(gram, vtn->tn_word, line, 0)) {
    cx->tdn->n++;
    return 0;
  }
  else {
    return -1;
  }
}
