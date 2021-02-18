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
static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/rcmd/catdic/RCS/catdic.c,v 1.19 91/08/19 13:28:36 kubota Exp $";
#endif
/*
 * DOWNLOADDIC
 * マウントされているかもしくはマウント可能な辞書をdownloadする。
 */

#include <stdio.h>
#include <signal.h>
#include "RK.h"
#include "RKintern.h"
#include "RKdic.h"

#ifdef nec_ews_svr2
#define SIGVAL int
#else
#ifdef nec_ews_svr4
#define SIGVAL void
#else
#define SIGVAL void
#endif /* nec_ews_svr4 */
#endif /* nec_ews_svr2 */

char init[RECSZ];
#ifndef lint
int mode;	/* dummy */
#endif
unsigned char dicname[RECSZ];

static int cx_num;
static char *r_file;
static SIGVAL RefreshAll();

static SIGVAL
RefreshAll()
{
#ifdef DEBUG
  Message("Caught signal, Shutting download and remove dic");
#endif
  (void) signal(SIGQUIT, SIG_IGN);
  (void) signal(SIGTERM, SIG_IGN);
  (void) signal(SIGINT, SIG_IGN);

  if(STRCMP(r_file,"")) {
    unlink(r_file);
  }
  Message("ダウンロードを中断します。");
  RkUnmountDic(cx_num, dicname);
  RkFinalize();
  exit(ERR_VALUE);
}

void
usage(Progname)
char *Progname;
{
  (void)fprintf(stderr,"Usage: %s [options]  remote-dic\n", Progname);
  (void)fprintf(stderr," options:\n");
  (void)fprintf(stderr,"        {-is | -irohaserver} iroha-server\n");
  (void)fprintf(stderr,"        -i | -u user-name\n");
  (void)fprintf(stderr,"        -l local-file\n");
  (void)fflush(stderr);
  exit(ERR_VALUE);
}

main(argc, argv)
int argc;
char **argv;
{
  FILE *fopen(), *fp = stdout;
  unsigned char dirname[RECSZ*2];          /* ユーザ名または"iroha"またはNULL*/
  unsigned char filename[RECSZ*2];        /* ファイル名またはNULL */
  char *Progname = *argv;                  /* "downloaddic" */

  if (argc < 2) usage(Progname);

  (void)strcpy((char *)dirname, "");
  (void)strcpy((char *)dicname, "");
  (void)strcpy((char *)filename, "");
  (void)strcpy(init,"/usr/lib/iroha/dic");

  while(--argc) {
      argv++;
      if (!strcmp(*argv, "-h")) {
	  (void)usage(Progname);
      } else if (!strcmp(*argv, "-is") ||
		 !strcmp(*argv, "-irohaserver")) {
	  if (--argc) {
	      argv++;
	      (void)strcpy(init, *argv);
	      continue;
	  }
      } else if (!strcmp(*argv, "-i")) { /* システム辞書指定 */
	  if ((char *)dirname[0] == '\0') {
	      (void)strcpy((char *)dirname,"iroha");
	      continue;
	  }
      } else if (!strcmp(*argv, "-u")) { /* ユーザ名指定 */ 
	  if ((char *)dirname[0] == '\0') {
	      if (--argc) {
		  argv++;
		  (void)strcpy((char *)dirname,*argv);
		  continue;
	      }
	  }
      } else if (!strcmp(*argv, "-l")) { /* ファイル指定 */
	  if ((char *)filename[0] == '\0') {
	      if (--argc) {
		  argv++;
		  (void)strcpy((char *)filename,*argv);
		  continue;
	      }
	  }
      } else if ((char *)dicname[0] == '\0') {  /* 辞書名 */
	  (void)strcpy((char *)dicname,*argv);
	  continue;
      }
      (void)usage(Progname);
  }

  if( dicname[0] == '\0' ) { /* 辞書名が指定されなければエラー */
      (void)fprintf(stderr,"辞書が指定されていません\n");
      usage(Progname);
  }
  if( filename[0] != '\0' ) { /* ファイル名が指定されなければ標準出力 */
      if((fp = fopen((char *)filename,"w")) == NULL) {
	  (void)fprintf(stderr,
			"指定されたファイル\"%s\"がオープンできません。\n",
			filename);
	  exit(ERR_VALUE);
      }
  }

  r_file = (char *)filename;

  /* まずInitializeして */
  if ((cx_num = RkInitialize(init)) < 0) {
      if (init[0] == '/') {
	  (void)fprintf(stderr,
			"かな漢字変換サーバと接続できませんでした。\n");
	  exit(ERR_VALUE);
      } else {
	  (void)fprintf(stderr,
			"かな漢字変換サーバ\"%s\"と接続できませんでした。\n",
			init);
	  exit(ERR_VALUE);
      }
  }
  
  (void) signal(SIGTERM, RefreshAll);
  (void) signal(SIGINT, RefreshAll);

  /* 辞書に書いて */
  if(DownLoadDic(fp, dirname) < 0) {
    RkUnmountDic(cx_num, dicname);
    RkFinalize();
    exit(ERR_VALUE);
  }
  RkUnmountDic(cx_num, dicname);
  RkFinalize();
  exit(0);
}

DownLoadDic(fp, dirname)
FILE          *fp;
unsigned char *dirname;
{
  int           ret;
  unsigned char buf[BUFLEN];
  unsigned char dicname_bk[RECSZ];

  strcpy((char *)dicname_bk, (char *)dicname);

  do {
      if ((ret = RkGetWordTextDic(cx_num,
				  dirname,
				  dicname_bk,
				  buf,
				  BUFLEN)) >= 0) {
	  if (!ret) {
	      (void)fclose(fp);
	      break;
	  }
	  if (fprintf(fp,"%s\n",buf) <= 0) {
	      (void)fclose(fp);
	      (void)fprintf(stderr,
			    "辞書\"%s\"に書き込むことができません。\n",
			    dicname_bk);
	      return -1;
	  } else {
	      (void)strcpy((char *)buf,"");
	      (void)strcpy((char *)dicname_bk, "");
	  }
      } else {
	  (void)fclose(fp);
	  PrintMessage(ret, dicname);
	  return -1;
      }
  } while(ret >= 0);
  return (0);
}
