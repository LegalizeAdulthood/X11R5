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
 * マウントされている指定された辞書(dm)の中身をからに(初期化)する。
 * 指定された辞書がテキストファイルでなかった場合は-1を返す。
 */
  
ClearMountDic(dm)
struct DM             *dm;
{
  struct TD             *xdm = (struct TD *)dm->dm_extdata;
  
  if (!dm || !xdm) return -1; /* テキストファイルでなかった */
  
  _RkKillCache(dm);
  freeTD((struct TD *)xdm); /* TDをすべてfreeする。 */  
  /*
   * TDを初期化する。
   */
  if (!(dm->dm_extdata = (char *)newTD())) {
    return -1;
  }
  return 0;
}

/* ClearUnmountDic(dm)
 * マウントされていない指定された辞書(ファイル)の中身をクリアする。
 * 指定された辞書ファイルのオープンに失敗したら-1を返す。
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
  
  /* ファイルの中身をクリアする。 */
  (void)sprintf(file_path ,"%s/%s",dd->dd_path, df->df_link);
  
  if((fp = fopen(file_path, "w")) == NULL) {
    return -1; /* ファイルのオープンに失敗した。 */
  }
  (void)fclose(fp);
  return 0;
}


/* SearchMountDic(dm)
 * すべてのコンテクストで指定された辞書がマウント中か調べる。
 * マウント中であるコンテクストが１つでも存在すれば"1"を返し、
 * マウント中であるコンテクストが１つも存在しなければ"0"を返す。
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
 * すべてのコンテクストで指定された辞書が使用中か調べる。
 * 使用中であるコンテクストが１つでも存在すれば"1"を返し、
 * 使用中であるコンテクストが１つも存在しなければ"0"を返す。
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
      if(IS_XFERCTX(cx)) { /* コンテクストが、今、変換中か？ */
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
 * すべての変換中でないコンテクストを検索して、
 * マウントしているコンテクストが１つでもあれば、ClearMountDicを呼ぶ。
 * マウントしているコンテクストが１つも無い時は、ClearUnmountDicを呼ぶ。
 * 指定された辞書がシステム辞書ならクリアしないで-1を返す。
 * ClearMountDicかClearUnmountDicに失敗したら-1を返す。
 * 
 * 注)この関数では、変換中でないコンテクストしか検索しないため、
 * 変換中のコンテクストでマウントしている辞書は削除します。
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
 * 指定されたddpの中のシステム辞書ディレクトリでないディレクトリに、
 * 指定された辞書が存在するかどうか調べる。
 * 存在したらその辞書(dm)へのポインタを返す。
 * 存在しなかったらNULLポインタを返す。
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
      if(strcmp(dd->dd_name, "iroha")) { /* システム辞書で無い時だけ */
	fh = &dd->dd_files; /* ディレクトリの全てのファイルを調べる。 */
	for ( f = fh->df_next; f != fh; f = f->df_next ) {
	  mh = &f->df_members; /* ファイルの全ての辞書を調べる。 */
	  for ( m = mh->dm_next; m != mh; m = m->dm_next ) {
	    if (!strcmp((char *)(m->dm_nickname), (char *)name) ) {
	      /* 辞書名が一致した。 */
	      return m;
	    }
	  }
	}
      }
    }
  }
  /* 指定されたディレクトリには指定された辞書は存在しませんね。 */
  return (struct DM *)0;
}


/* int
 * RkCreateDic(cx_num, dicname, mode)
 *
 * 引き数
 *         int            cx_num    コンテクストナンバー
 *	   unsigned char  *dicname  辞書へのポインタ
 *	   int            mode      辞書の種類と強制モードのOR
 *             辞書の種類 
 *                 #define	Rk_MWD		0x80
 *                 #define	Rk_SWD		0x40
 *                 #define	Rk_PRE		0x20
 *                 #define	Rk_SUC		0x10
 *             強制モード
 *                 #define KYOUSEI	        0x01
 *                 強制しない場合           0x00
 *
 * リターン値
 *             成功した場合                                 0
 *             成功した場合(上書きした場合)                 1
 *             アロケーションに失敗した場合                -6  NOTALC
 *             辞書がバイナリ辞書であった場合              -9    BADF
 *             dics.dirに異常があった場合                 -10   BADDR
 *             GetDicFilenameの返り値が-1の場合           -13   ACCES
 *             MakeDicFileに失敗した場合                  -13   ACCES
 *             CleatDicに失敗した場合                     -13   ACCES
 *             辞書がマウント中であった場合               -16   MOUNT
 *             辞書がすでにある場合(強制でない場合)       -17   EXIST
 *             辞書が使用中であった場合                   -26  TXTBSY
 *             modeが異常値であった場合                   -99  BADARG
 *             コンテクスト構造体が存在しない場合        -100 BADCONT
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
  
  /* 同じ辞書名の辞書が存在するか調べます。 */
  if (!(dm = SearchUDDP(ddp, dicname))) {
    /* 新しく辞書を作成する */
      return (CreateNewDic(ddp, mode, dicname));
  }
  /* 既存の辞書に上書きする */
  return (RefreshOldDic(dm, mode));
}

/*
 * RefreshOldDic(dm, mode)
 * dmが存在する場合の処理
 */
RefreshOldDic(dm, mode)
struct DM  *dm;
int        mode;
{
  unsigned char	*check_file;
  unsigned char	file_path[512];

  /* textdicどうか調べる */
  if (DM2TYPE(dm) != DF_TEMPDIC ) {  
    return BADF;
  }
  /* 強制モードなら辞書をからにする */
  if ((mode & KYOUSEI) != KYOUSEI) {
    return EXIST; /* 強制モードでないので辞書を作らないで返す */
  }
  /* 辞書の種類が一緒かどうか */
  if (!(ND2RK(dm->dm_class) & mode)) {
    return INVAL;
  }
  /* マウント中か調べる */
  if (SearchMountDic(dm)) {
    return TXTBSY;
  }

  /*
   * dmと比べてdics.dirが正しいかどうかチェックします。
   */
  if (!(check_file = CheckDicFile(dm))) {
    return BADDR; /* dics.dirに無かった！ */
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
    return ACCES; /* ファイルを作成できません */
  }
  
#ifdef RK_TEST
  /*
   * 作成した辞書をマウントしてチェックします。
   */
  if (RkMountDic(cx_num,dicname,0) == -1){
    (void)fprintf(stderr,"辞書\"%s\"のマウントに失敗しました\n",
		  dicname);
  } else {
    (void)fprintf(stderr,"辞書\"%s\"のマウントに成功しました\n",
		  dicname);
    RkUnmountDic(cx_num,dicname);
  }
#endif
  return 0; /* 新しく辞書を作成しました。 */
}

/*
 * RkListDic(cx_num, dirname, buf, size)
 * int  cx_num;             コンテクストナンバー
 * unsigned char *dirname;  辞書リストを出力したいディレクトリ名
 * unsigned char *buf;      辞書リストが返ってくるバッファ
 * int  size;               バッファのサイズ
 *
 * リターン値               
 *             成功した場合                      辞書の数
 *             コンテクストナンバーが負の場合          BADCONT
 *             RkCreateContextに失敗した場合           BADCONT
 *             RkSetDicPathに失敗した場合              NOTALC
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
 * 引き数で指定された辞書がどのコンテクストでもマウントされていなければ、
 * その辞書を削除する。
 *
 * 引き数
 *             dm 削除したい辞書のdm構造体へのポインタ
 *
 * リターン値
 *             成功した場合                             0
 *             マウントしていた場合                    -1
 *             RemoveDicFileの返り値が-1の場合         -1
 */

RemoveDic( dm )
struct DM	        *dm;
{
  /* どこでもマウントしていなかったのでファイルを破壊します。 */
  if (RemoveDicFile( dm ) == -1) {
    return -1;
  }
  (void)freeDF( dm->dm_file );
  return 0;
}

/* int
 * RkRemoveDic(cx_num, dicname)
 *
 * 指定されたコンテクストに指定された辞書が存在すれば
 * その辞書を削除する。
 *
 * 引き数
 *             int            cx_num     コンテクストナンバー
 *             unsigned char  *dicname   辞書名
 *
 * リターン値
 *             成功した場合                             0
 *             辞書が存在しない場合                    -2   NOENT
 *             辞書がバイナリ辞書であった場合          -9    BADF
 *             RemoveDicの返り値が-1の場合            -13   ACCES
 *             マウントしていた場合                   -26  TXTBSY
 *             コンテクスト構造体が存在しない場合    -100 BADCONT
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
    return BADCONT; /* 指定されたコンテクストNOが変 */
  
  /* 同じ辞書名のユーザ辞書が存在するか調べる */
  if((dm = SearchUDDP(ddp, dicname)) == (struct DM *)NULL )
    return NOENT; /* 指定された辞書は存在しません。 */
  
  /* 辞書が存在したらtextdicか調べる */
  if (DM2TYPE(dm) != DF_TEMPDIC ) {
    return BADF; /* バイナリ辞書は消せません */
  }
  /* マウント中なら削除しない */
  if (SearchMountDic(dm)) {
    return TXTBSY;
  }

  /* 辞書削除に失敗したら return ACCES */
  if (RemoveDic(dm) == -1) {
    return ACCES;
  }
  return 0; /* 辞書を削除した */
}

/* int
 * RkRenameDic(cx_num, dicname1, dicname2, mode)
 *
 * 指定されたコンテクストに指定された辞書が存在すれば
 * その辞書を削除する。
 *
 * 引き数
 *          int            cx_num           コンテクストナンバー
 *          unsigned char  *dicname1        変更元辞書名
 *          unsigned char  *dicname2        変更先辞書名
 *          int            mode             強制モード
 *
 * 返り値 (RKdic.h参照)
 *          成功した場合                             0
 *          dicname1が存在しない場合                -2     NOENT
 *          RemoveDicの返り値が-1の場合             -2     NOENT
 *          辞書がバイナリ辞書であった場合          -9      BADF
 *          RenameDicFileの返り値が-1の場合        -13     ACCES
 *          dicname2が存在する場合                 -17     EXIST
 *          dicname1をマウントしていた場合         -26    TXTBSY
 *          dicname2をマウントしていた場合         -26    TXTBSY
 *          コンテクスト構造体が存在しない場合    -100   BADCONT
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

    /* RENAME元の辞書がなければ return NOENT */
    if ((dm1 = SearchUDDP(ddp, dicname1)) == (struct DM *)NULL ) {
	return NOENT;
    }

    /* 辞書が存在したらtextdicか調べる */
    if (DM2TYPE(dm1) != DF_TEMPDIC ) {
      return BADF; /* バイナリ辞書はrenameできません */
    }
    /* RENAME元の辞書がマウント中なら return TXTBSY */
    if (SearchMountDic(dm1)) {
      return TXTBSY;
    }

    /* RENAME先の辞書があれば削除する */
    if ((dm2 = SearchUDDP(ddp, dicname2)) != (struct DM *)NULL ) {
      
      /* 辞書が存在したらtextdicか調べる */
      if (DM2TYPE(dm2) != DF_TEMPDIC ) {
	return BADF; /* バイナリ辞書は消せません */
      }
      return (rk_del_rename(dm1, dm2, (char *)dicname2, mode));
    }
    else {
      /* RENAME先の辞書が無い時の処理 */
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

/* マウント中ならエラー */
    if (SearchMountDic(dm_obj))
	return TXTBSY;

/* 強制モードでなかったら返す */
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
     *	※どちらか失敗した場合、もう一方を元に戻せません。
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
 * int            cx_num      コンテクストNO
 * unsigned char  *dirname    ディレクトリ名
 * unsigned char  *dicname    辞書名
 * unsigned char  *info       バッファ
 * int            infolen     バッファの長さ
 *
 * 返り値 : 実際にinfoに入った長さ
 *          最後まで読んでいたら          ０を返す
 *          RkCreateContextに失敗した     BADCONT
 *          RkDuplicateContextに失敗した  BADCONT
 *          RkGetContextに失敗した        BADCONT
 *          RkSetDicPathに失敗した        NOTALC
 *          RkMountDicに失敗した          NOENT
 *          SearchUDDPに失敗した          NOENT
 *          バイナリ辞書だった                          -9   BADF
 *          dics.dirに異常があった場合                 -10   BADDR
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

  /* １回目の処理 */
  if(dicname[0] != '\0') {

/* area for cx->gwt_dicname */
    size = strlen((char *)dicname) + 1;
    if (!(buff = (unsigned char *)malloc(size))) {
	return (NOTALC);
    }
    (void)strcpy((char *)buff, (char *)dicname);

    /* 他人の辞書を見たい時はSetDicPathする */
    if(dirname[0] != '\0') {
      /* 新しいコンテクスト作成 */
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
  
    /* 辞書をマウントします */
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
      return BADF; /* バイナリ辞書だった */
    }
    if(RkMountDic(new_cx_num, (char *)dicname,0) == -1) {
      RkCloseContext(new_cx_num);
      (void)free((unsigned char *)buff);
      return NOMOUNT; /* 辞書がマウントできませんでした */
    }
    if (!(CheckDicFile(dm))) {
      RkCloseContext(new_cx_num);
      (void)free((unsigned char *)buff);
      return BADDR; /* dics.dirに無かった！ */
    }

    cx->gwt_cx = new_cx_num;

    if (cx->gwt_dicname) {
      (void)free((unsigned char *)cx->gwt_dicname);
      cx->gwt_dicname = (unsigned char *)NULL;
    }
    cx->gwt_dicname = buff;
    initial_td = (struct TD *)dm->dm_extdata;
  }

  /* ２回目以降の処理 */
  else {
    if ((new_cx_num = cx->gwt_cx) < 0) {
      (void)free((unsigned char *)buff); /* ここを通るのはとても異常です */
      return BADCONT;
    }
    if (!(new_cx = RkGetContext(new_cx_num))) {
      (void)free((unsigned char *)buff); /* ここを通るのはとても異常です */
      return BADCONT;
    }
    initial_td = (struct TD *)0;
  }
  /* 共通の処理 */

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


freeTdn(cx)  /* tdn を next をたどりながら free する */
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
 * 返り値 成功  0
 *        失敗 -1
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
      return NOTALC; /* 新しい構造体のallocに失敗 */
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
  while ( !IsWordNode(vtn) ) { /* ツリーなら */
    cx->tdn->n++;
    if(!pushTdn(cx, vtn->tn_tree)) {
      return NOTALC; /* 新しい構造体のallocに失敗 */
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
