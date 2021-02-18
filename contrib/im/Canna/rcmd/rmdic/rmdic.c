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
static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/rcmd/rmdic/RCS/rmdic.c,v 1.22 91/09/17 20:16:54 mako Exp $";
#endif
#include "RK.h"
#include "RKintern.h"
#include "RKdic.h"
#include <stdio.h>
#include <string.h>
#include <signal.h>

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
int mode;	/* dummy */
#endif

usage(Progname)
char *Progname;
{
  (void)fprintf(stderr,"Usage: %s [options]  dicname1 [dicname2...]\n", Progname);
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

/* RMUSERDIC */
main(argc, argv)
int argc;
char *argv[];
{
  int  i, j, cx_num;
  char *Progname = *argv;
  int isflag = 0;
  int rmdone = 0; /* rm ���ޤ��ʤ���Ƥ��ʤ� */

  if(argc < 2) usage(Progname);

  (void)strcpy(init, "/usr/lib/iroha/dic");

  /* Usage�򸫤���ʬ */
  for(i = 1; i < argc ; i++) {
    if ((!strcmp(argv[i], "-is")) ||
	(!strcmp(argv[i], "-irohaserver")) ) {
      /* ���ȣ��İʾ���������ʤ���Х��顼 */
      if (argc - i < 2) {
	(void)fprintf(stderr,"������̾�����ꤵ��Ƥ��ޤ���\n");
	usage(Progname);
      }
      /* �����Ф����٤���ꤵ��Ƥ����饨�顼 */
      if (isflag) {
	(void)fprintf(stderr,"�����Ф�ʣ������ꤵ��Ƥ��ޤ���\n");
	usage(Progname);
      }
      else {
	isflag = 1;
	(void)strcpy(init, argv[++i]);
      }
    }
  }
  /* ����� */
  if ((cx_num = RkInitialize(init)) < 0) {
    if (init[0] == '/') {
      (void)fprintf(stderr,
		    "���ʴ����Ѵ������Ф���³�Ǥ��ޤ���Ǥ�����\n");
      exit(ERR_VALUE);
    }
    else {
      (void)fprintf(stderr,
		    "���ʴ����Ѵ�������\"%s\"����³�Ǥ��ޤ���Ǥ�����\n",
		    init);
      exit(ERR_VALUE);
    }
  }

  (void) signal(SIGINT, StopAll);
  (void) signal(SIGQUIT, StopAll);
  (void) signal(SIGTERM, StopAll);

  /* ������ */
  for(j = 1; j < argc; j++) {

    /* �����Ф����ꤵ�줿���ϰ����򣲤����Ф� */
    if ((!strcmp("-is", argv[j])) ||
	(!strcmp("-irohaserver", argv[j])) ) {
      j++;
    }
    else {
      if (rmDictionary(cx_num, argv[j])) {
	RkFinalize();
	exit(ERR_VALUE);
      }
      rmdone = 1; /* �ºݤ� rm ���Ԥ�줿 */
    }
  }
  RkFinalize();
  if (rmdone == 0) { /* ���� rm ���ʤ���ʤ��ä��ʤ�� */
    (void)fprintf(stderr, "���񤬻��ꤵ��Ƥ��ޤ���\n");
  }
  exit(0);    
}



