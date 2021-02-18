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
static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/rcmd/rutil/RCS/lsuserdic.c,v 1.21 91/06/21 13:47:25 satoko Exp $";
#endif
#include "RK.h"
#include "RKintern.h"
#include "RKdic.h"
#include <stdio.h>
#include <string.h>
#include <pwd.h>
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
void usage();
int makelist();
char *searchuname();

char init[RECSZ], user[RECSZ*2];
extern void Message();
unsigned char buf[BUFLEN];
int bufcnt;

int is_include_name();

#ifndef lint
int mode; /* dummy */
#endif

void
usage(Progname)
char *Progname;
{
    (void)fprintf(stderr,"Usage: %s [options]\n", Progname);
    (void)fprintf(stderr," options:\n");
    (void)fprintf(stderr,"        {-is | -irohaserver} iroha-server\n");
    (void)fprintf(stderr,"        -i\n");
    (void)fprintf(stderr,"        -u user-name\n");
    (void)fflush(stderr);
    exit(ERR_VALUE);
}

static SIGVAL
StopAll()
{
#ifdef DEBUG
  Message("Caught signal, Shutdown.\n");
#endif
  (void) signal(SIGQUIT, SIG_IGN);
  (void) signal(SIGTERM, SIG_IGN);
  (void) signal(SIGINT, SIG_IGN);
  RkFinalize();
  exit(ERR_VALUE);
}

/*
 * �ºݤ�RkLib��Ƥ֤Ȥ���
 */
int
makelist()
{
    int cx_num;
    if ( cx_num = RkInitialize(init) < 0) {
	if (init[0] == '/') {
	    (void)fprintf(stderr,
			  "���ʴ����Ѵ������Ф���³�Ǥ��ޤ���Ǥ�����\n");
	} else {
	    (void)fprintf(stderr,
			  "���ʴ����Ѵ�������\"%s\"����³�Ǥ��ޤ���Ǥ�����\n",
			  init);
	}
	exit(ERR_VALUE);
    }
    /*
     * �ºݤ˼���ꥹ�Ȥ���
     */
    (void) signal(SIGINT, StopAll);
    (void) signal(SIGQUIT, StopAll);
    (void) signal(SIGTERM, StopAll);
    bufcnt = RkListDic(cx_num, user, buf, BUFLEN );
#ifdef DEBUG
    if ((bufcnt == BADCONT) ||
	(bufcnt < 0)) {
	Message("�����Ф��֤��ͤ��۾�Ǥ���");
    }
#endif
    RkFinalize();
    return 0;
}

/*
 * �桼��̾���� ������=̾���ؤΥݥ���
 */
char *
searchuname()
{
    char *username = NULL ;
    
    if ( (username = (char *)getenv( "LOGNAME" )) == NULL ) {
	if( (username = (char *)getenv( "USER" )) == NULL ) {
	    if( (username = (char *)getlogin()) == NULL ) {
		struct passwd *pass = getpwuid( (int)getuid() ) ;
		if( pass )
		  username = pass->pw_name ;
	    }
	}
    }
    return( username ) ;
}

/* ����Ʊ���桼�������ꤵ��Ƥ��뤫�ɤ���Ĵ�٤� */
is_include_name(buf, user)
char *buf;
char *user;
{
  char *b = buf;

  if(!b || !user) {

    return (0);
  }

  do {
    if(!strncmp(b, user, strlen(user)) &&
       (b[strlen(user)] == ':' || !b[strlen(user)])) {
      return (1); /* atta! */
    }
  } while (b = strchr(b, ':'), b++);

  return (0); /* nakatta! */
}

/* ����ꥹ�Ȥ�������ޤ��� */
main(argc, argv)
int argc;
char *argv[];
{
    unsigned char *p;
    int i, arg;
    char *Progname = *argv;
    int iflag = 0;
    int uflag = 0;
    int isflag = 0;

    /*
     * �桼��̾�����ꤵ��Ƥ��ʤ�����
     * ��ʬ��̾����õ���Ƥ���̾����RK��ƤӤޤ���
     */
    
    (void)strcpy(init, "/usr/lib/iroha/dic");
    (void)strcpy(user, "\0");
    

    for(arg = 1; arg  < argc; ++arg) {
        /* �����л��� */
	if ((!strcmp(argv[arg], "-is")) ||
	    (!strcmp(argv[arg], "-irohaserver")) ) {
	    if(isflag) {
	        usage(Progname); /* ������ꤵ�줿�饨�顼 */
	    }
	    else {
	      isflag =1;
	        if(arg + 2 > argc) { /* ���ΰ�����̵���Τǥ��顼 */
		  usage(Progname); /* �����л���θ�˰�����̵����Х��顼 */
	        }
		(void)strcpy(init, argv[++arg]);
	    }

	}
	/* �����ƥ༭��ǥ��쥯�ȥ���� */
	else if (!strcmp(argv[arg], "-i")) {
	  if(iflag) { /* ������ꤵ�줿�饨�顼 */
	    usage(Progname);
	  }
	  else {
	    iflag =1;
	    if(*user == (char)NULL) {
		(void)strcpy(user, "iroha");
	    } else {
		(void)strcat(user, ":iroha");
	    }
	  }
	}
	/* �桼������ǥ��쥯�ȥ���� */
	else if (!strcmp(argv[arg], "-u")) {
	  if(uflag) { /* ������ꤵ�줿�饨�顼 */
	    usage(Progname);
	  }
	  else {
	    uflag = 1;
	    if(arg +2 > argc) { /* ���ΰ�����̵���Τǥ��顼 */
		usage(Progname);
	    }
	    if(*user == (char)NULL) {
		(void)strcpy(user, argv[++arg]);
	    } else {
		(void)strcat(user, ":");
		(void)strcat(user, argv[++arg]);
	    }
	  }
	} else { /* -is -irohaserver -i -u �ʳ��λ���ʤ�usage��Ф� */
	    usage(Progname);
	}
    }

    /* �����ǥ桼�������ꤵ��ʤ���м�ʬ�μ����ץ��ȥ����Ȥ��ޤ� */
    if (*user == (char)NULL ) {
      if (uflag) { /* �桼��̾��NULL���ϤäƤ����饨�顼 */
	usage(Progname);
      }
      (void)strcpy(user, searchuname());
    }
    
    (void)makelist(); /* ����ꥹ�Ⱥ��� */    
    
    /*
     *    ���������ץ��ȥ����Ȥ��ƽ����Ǥ���
     */
    
    if (bufcnt >= 0) {
      for (p = buf, i = 0 ; i < bufcnt ; i++) {
        (void)fprintf(stdout, "%s\n", p);
        (unsigned char *)p += (strlen((char *)p) + 1);
      }
      exit(0);
    }
    switch ( bufcnt ) {
      case NOTALC :
	(void)fprintf(stderr, "�������ݤǤ��ޤ���\n");
	break;
      case BADCONT :
	(void)fprintf(stderr,"����ƥ����Ȥ��ͤ��۾�Ǥ���\n");
	break;
      default:
	if (init[0] == '/') {
	    (void)fprintf(stderr, 
"���ʴ����Ѵ������Ф�������ƥʥ󥹤򥵥ݡ��Ȥ��Ƥ��ޤ���\n");
	} else {
	    (void)fprintf(stderr,
"���ʴ����Ѵ�������\" %s\"��������ƥʥ󥹤򥵥ݡ��Ȥ��Ƥ��ޤ���\n",
			  init);
	}
	break;
    }
    exit(ERR_VALUE);
}





