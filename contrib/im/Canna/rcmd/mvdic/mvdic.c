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
static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/rcmd/mvdic/RCS/mvdic.c,v 1.25 91/09/17 20:16:05 mako Exp $";
#endif
/*
 * MVUSERDIC
 */

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include "RK.h"
#include "RKintern.h"
#include "RKdic.h"

#define NOT_OVER_WRITE 0
#define OVER_WRITE 1

#ifdef nec_ews_svr2
#define SIGVAL int
#else
#ifdef nec_ews_svr4
#define SIGVAL void
#else
#define SIGVAL void
#endif /* nec_ews_svr4 */
#endif /* nec_ews_svr2 */

static SIGVAL StopAll();
char init[RECSZ];
#ifndef lint
int mode;
#endif

usage(Progname)
char *Progname;
{
  (void)fprintf(stderr,"Usage: %s [options]  from-dic to-dic\n", Progname);
  (void)fprintf(stderr," options:\n");
  (void)fprintf(stderr,"        {-is | -irohaserver} iroha-server\n");
  (void)fflush(stderr);
  exit(ERR_VALUE);
}

static SIGVAL
StopAll()
{
#ifdef DEBUG
  Message("Caught signal, Shutting down and remove dic\n");
#endif
  (void) signal(SIGQUIT, SIG_IGN);
  (void) signal(SIGTERM, SIG_IGN);
  (void) signal(SIGINT, SIG_IGN);

  RkFinalize();
  exit(ERR_VALUE);
}

main(argc, argv)
int argc;
char *argv[];
{
  int  cx_num;
  int  ret;
  char *dicname1;
  char *dicname2;
  char *Progname = *argv;
  ret = 0;

  (void)strcpy(init,"/usr/lib/iroha/dic");

  /* ������������� */
  if(argc == 5) {
    if((!strcmp("-is", argv[1])) ||
       (!strcmp("-irohaserver", argv[1])) ) {
      (void)strcpy(init,argv[2]);
      dicname1=argv[3];
      dicname2=argv[4];
    }
    else if((!strcmp("-is", argv[2])) ||
	    (!strcmp("-irohaserver", argv[2])) ) {
      (void)strcpy(init,argv[3]);
      dicname1=argv[1];
      dicname2=argv[4];
    }    
    else if((!strcmp("-is", argv[3])) ||
	    (!strcmp("-irohaserver", argv[3])) ) {
      (void)strcpy(init,argv[4]);
      dicname1=argv[1];
      dicname2=argv[2];
    }    
    else usage(Progname);
  }
  else if(argc == 3) {
    if(strcmp("-is", argv[1]) &&
       strcmp("-irohaserver", argv[1]) &&
       strcmp("-is", argv[2]) &&
       strcmp("-irohaserver", argv[2])) {
      dicname1=argv[1];
      dicname2=argv[2];
    }
    else
      usage (Progname);
  }
  else
    usage (Progname);


  /* ����̾�ˡ�-�ɤ��ޤޤ�Ƥ����饨�顼 */
  if (strchr(dicname1,'-') ||
      strchr(dicname2,'-')) {
    fprintf(stderr,"����̾��'-'�ϻ��ѤǤ��ޤ���\n");
    exit(ERR_VALUE);
  }

  if (strlen((char *)dicname1) > (RECSZ -1)) {
      fprintf(stderr,"����̾\"%s\"��Ĺ�����ޤ���\n", dicname1);
      exit(ERR_VALUE);
  }
  if (strlen((char *)dicname2) > (RECSZ -1)) {
      fprintf(stderr,"����̾\"%s\"��Ĺ�����ޤ���\n", dicname2);
      exit(ERR_VALUE);
  }

  /* ����̾��Ʊ���ʤ饨�顼 */
  if(!strcmp(dicname1,dicname2)) {
    fprintf(stderr, "%s: %s, ����̾��Ʊ���Ǥ���\n",
	    Progname, dicname1);
    exit(ERR_VALUE);
  }

  (void) signal(SIGINT, StopAll);
  (void) signal(SIGQUIT, StopAll);
  (void) signal(SIGTERM, StopAll);

  /* initialize���� */
  if ((cx_num = RkInitialize(init)) < 0) {
    if (init[0] == '/') {
      fprintf(stderr,
	      "���ʴ����Ѵ������Ф���³�Ǥ��ޤ���Ǥ�����\n");
      exit(ERR_VALUE);
    }
    else {
      fprintf(stderr,
	      "���ʴ����Ѵ�������\"%s\"����³�Ǥ��ޤ���Ǥ�����\n",
	      init);
      exit(ERR_VALUE);
    }
  }

  ret = renameDictionary(cx_num, dicname1, dicname2, NOT_OVER_WRITE);
  RkFinalize();
  exit(ret);    
}

renameDictionary(cn, dicname1, dicname2, force)
int cn;
char *dicname1;
char *dicname2;
int force;
{
  char ans[20];
  int ret = 0;

  switch ( RkRenameDic( cn, dicname1, dicname2, force) ) {
  case 0 :
    fprintf(stderr,"����\"%s\"�򼭽�\"%s\"���ѹ����ޤ���\n",
	   dicname1, dicname2);
    ret = 0;
    break;
  case 1 :
    fprintf(stderr,"����\"%s\"�򼭽�\"%s\"�˾�񤭤��ޤ���\n",
	   dicname1, dicname2);
    ret = 0;
    break;
  case NOTALC :
    (void)fprintf(stderr, "�ΰ褬���ݤǤ��ޤ���\n");
    break;
  case NOENT :
    fprintf(stderr,"����\"%s\"��¸�ߤ��ޤ���\n", dicname1);
    ret = 1;
    break;
  case BADF :
    fprintf(stderr,
	    "����\"%s\"�ޤ���\"%s\"���Х��ʥ꼭��Ǥ���\n", dicname1,dicname2);
    ret = 1;
    break;
  case ACCES :
    fprintf(stderr,"����˥�����������ޤ���\n");
    ret = 1;
    break;
  case BADDR :
    (void)fprintf(stderr,
		  "dics.dir ���۾�Ǥ�������ե����뤬�����Ǥ��ޤ���\n");
    ret = -1;
    break;
  case MOUNT :
  case EXIST :
    fprintf(stderr,
	    "���ꤵ�줿����\"%s\"�Ϥ��Ǥ�¸�ߤ��ޤ���񤭤��ޤ�����(y/n)",
	    dicname2);
    (void)gets(ans);
    if(!strcmp(ans,"y")) {
      ret = renameDictionary(cn, dicname1, dicname2, OVER_WRITE);
    }
    else {
      fprintf(stderr, "���ꤵ�줿����\"%s\"�Ͼ�񤭤��ޤ���\n",
	      dicname2);
      ret = 1;
    }
    break;
  case TXTBSY :
    fprintf(stderr,"����\"%s\"�ޤ���\"%s\"��������ʤΤǾ�񤭤Ǥ��ޤ���\n", dicname1, dicname2);
    ret = 1;
    break;
  case BADCONT :
    fprintf(stderr,"����ƥ����Ȥ��ͤ��ѤǤ���\n");
    ret = 1;
    break;
  default:
    if(init[0] == '/') {
      fprintf(stderr, 
	"���ʴ����Ѵ������Ф�������ƥʥ󥹤򥵥ݡ��Ȥ��Ƥ��ޤ���\n");
    }
    else {
      fprintf(stderr,
	"���ʴ����Ѵ�������\"%s\"��������ƥʥ󥹤򥵥ݡ��Ȥ��Ƥ��ޤ���\n",
	      init);
    }
    ret = 1;
    break;
  }
  return ret;
}
