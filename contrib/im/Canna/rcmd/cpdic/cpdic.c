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
static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/rcmd/rutil/RCS/cpuserdic.c,v 1.16 91/05/21 19:51:14 mako Exp $";
#endif

/*
 * CPUSERDIC
 */

#include <stdio.h>
#include <signal.h>
#include <ctype.h>
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

static int cx_num;
static SIGVAL	RefreshAll(), StopAll();
/* ���ԡ����μ���̾, ���ԡ���μ���̾ */
char dicname1[RECSZ], dicname2[RECSZ];

int mode;

void
usage(Progname)
char *Progname;
{
  (void)fprintf(stderr,"Usage: %s [options]  from-dic to-dic\n", Progname);
  (void)fprintf(stderr," options:\n");
  (void)fprintf(stderr,"        {-is | -irohaserver} iroha-server\n");
  (void)fprintf(stderr,"        -s\n");
  (void)fprintf(stderr,"        -i | -u user-name\n");
  (void)fflush(stderr);
  exit(ERR_VALUE);
}

static SIGVAL
StopAll()
{
#ifdef DEBUG
  Message("Caught signal, Shutting down and remove dic");
#endif
  (void) signal(SIGQUIT, SIG_IGN);
  (void) signal(SIGTERM, SIG_IGN);
  (void) signal(SIGINT, SIG_IGN);

  RkFinalize();

  Message("���ԡ������Ǥ��ޤ���");
  exit(ERR_VALUE);
}

static SIGVAL
RefreshAll()
{
#ifdef DEBUG
  Message("Caught signal, Shutting down and remove dic");
#endif
  (void) signal(SIGQUIT, SIG_IGN);
  (void) signal(SIGTERM, SIG_IGN);
  (void) signal(SIGINT, SIG_IGN);

  (void) RkUnmountDic(cx_num, dicname2);
  (void) RkFinalize();

  cx_num = RkInitialize(init);
  if (cx_num != 0) {
#ifdef DEBUG
      Message("RefreshAll: cannot make new connection with server \"%s\"",
	      init);
#endif
  } else {
      if ((mode & KYOUSEI) == KYOUSEI) {
#ifdef DEBUG
	  Message("RefreshAll: Restore dictionary \"%s\".", dicname2);
#endif
	  (void) RkCreateDic(cx_num, dicname2, mode);
      } else {
#ifdef DEBUG
	  Message("RefreshAll: Remove dictionary \"%s\"", dicname2);
#endif
	  (void) rmDictionary(cx_num, dicname2);
      }
      (void) RkFinalize();
  }
  Message("���ԡ������Ǥ��ޤ���");
  exit(ERR_VALUE);
}

main(argc, argv)
int argc;
char **argv;
{
  unsigned char dirname[RECSZ*2];      /* �桼��̾�ޤ���"iroha"�ޤ���NULL*/
  char *Progname = *argv;              /* "cpuserdic" */
  
  mode = Rk_MWD;              /* ����μ��� */

  if(argc < 3 || argc > 8) usage(Progname);
  
  (void)strcpy((char *)dirname, "");
  (void)strcpy((char *)dicname1, "");
  (void)strcpy((char *)dicname2, "");
  (void)strcpy((char *)init,"/usr/lib/iroha/dic");
  
  while(--argc) {
    argv++;
    if (!strcmp(*argv, "-is") ||
	!strcmp(*argv, "-irohaserver")) {
      if (--argc) {
	argv++;
	(void)strcpy(init,*argv);
      }
    }
    else if (!strcmp(*argv, "-i")) { /* �����ƥ༭����� */
      if(dirname[0] != '\0' ) usage(Progname); /* �桼��̾��������ꤵ�줿 */
      (void)strcpy((char *)dirname,"iroha");
    }
    else if (!strcmp(*argv, "-u")) { /* �桼��̾���� */ 
      if(dirname[0] != '\0' ) usage(Progname); /* �桼��̾��������ꤵ�줿 */
      if (--argc) {
	argv++;
	(void)strcpy((char *)dirname,*argv);
      }
    }
    /* swd�����ꤵ�줿��� */
    else if (!strcmp(*argv,"-s")) { /* swd���� */
	mode = Rk_SWD;
    }
    else { /* ����̾�����ꤵ�줿 */
      if(dicname1[0] == '\0' ) {
	(void)strcpy((char *)dicname1, *argv);
      }
      else {
	if(dicname2[0] == '\0' ) {
	  (void)strcpy((char *)dicname2, *argv);
	}
	else usage(Progname);
      }
      if(strchr((char *)dicname2,'-')) {
	(void)fprintf(stderr,
		      "\"%s\": ����̾�ˤ�\"-\"�ϻ��ѤǤ��ޤ���\n",
		      dicname2);
	exit(ERR_VALUE);
      }
      if (strlen((char *)dicname2) > (RECSZ -1)) {
	  fprintf(stderr,"����̾\"%s\"��Ĺ�����ޤ���\n", dicname2);
	  exit(ERR_VALUE);
      }
    }
  }
  /* ����̾�����ꤵ��ʤ���Х��顼 */
  if( dicname1[0] == '\0' || dicname2[0] == '\0') {
    (void)fprintf(stderr,"���񤬻��ꤵ��Ƥ��ޤ���\n");
    usage(Progname);
  }
  
  /* �ޤ�Initialize���� */
  if ((cx_num = RkInitialize(init)) < 0) {
    if (init[0] == '/') {
      (void)fprintf(stderr,"���ʴ����Ѵ������Ф���³�Ǥ��ޤ���Ǥ�����\n");
      exit(ERR_VALUE);
    }
    else {
      (void)fprintf(stderr,
		    "���ʴ����Ѵ�������\"%s\"����³�Ǥ��ޤ���Ǥ�����\n",
		    init);
      exit(ERR_VALUE);
    }
  }
  
  /* ������� */
  (void) signal(SIGINT, StopAll);
  (void) signal(SIGQUIT, StopAll);
  (void) signal(SIGTERM, StopAll);
  if (makeDictionary(cx_num, dicname2, mode) < 0) {
    RkFinalize();
    exit(ERR_VALUE);
  }

  (void) signal(SIGINT,  RefreshAll);
  (void) signal(SIGQUIT, RefreshAll);
  (void) signal(SIGTERM, RefreshAll);

  if(CopyDic(cx_num, dirname, dicname1, dicname2, mode)) {

    RkFinalize();
    exit(ERR_VALUE);
  }

  /* finalize���� */
  RkFinalize();
  (void)fprintf(stderr,
	"\n����\"%s\"�򼭽�\"%s\"�˥��ԡ����ޤ�����\n", dicname1, dicname2);
  exit(0);
}
