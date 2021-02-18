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
 * 実際にRkLibを呼ぶところ
 */
int
makelist()
{
    int cx_num;
    if ( cx_num = RkInitialize(init) < 0) {
	if (init[0] == '/') {
	    (void)fprintf(stderr,
			  "かな漢字変換サーバと接続できませんでした。\n");
	} else {
	    (void)fprintf(stderr,
			  "かな漢字変換サーバ\"%s\"と接続できませんでした。\n",
			  init);
	}
	exit(ERR_VALUE);
    }
    /*
     * 実際に辞書リストを作る
     */
    (void) signal(SIGINT, StopAll);
    (void) signal(SIGQUIT, StopAll);
    (void) signal(SIGTERM, StopAll);
    bufcnt = RkListDic(cx_num, user, buf, BUFLEN );
#ifdef DEBUG
    if ((bufcnt == BADCONT) ||
	(bufcnt < 0)) {
	Message("サーバの返り値が異常です。");
    }
#endif
    RkFinalize();
    return 0;
}

/*
 * ユーザ名検索 帰り値=名前へのポインタ
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

/* 既に同じユーザが指定されているかどうか調べる */
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

/* 辞書リストを作成します。 */
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
     * ユーザ名が指定されていない時は
     * 自分の名前を探してその名前でRKを呼びます。
     */
    
    (void)strcpy(init, "/usr/lib/iroha/dic");
    (void)strcpy(user, "\0");
    

    for(arg = 1; arg  < argc; ++arg) {
        /* サーバ指定 */
	if ((!strcmp(argv[arg], "-is")) ||
	    (!strcmp(argv[arg], "-irohaserver")) ) {
	    if(isflag) {
	        usage(Progname); /* ２回指定されたらエラー */
	    }
	    else {
	      isflag =1;
	        if(arg + 2 > argc) { /* 次の引数が無いのでエラー */
		  usage(Progname); /* サーバ指定の後に引数が無ければエラー */
	        }
		(void)strcpy(init, argv[++arg]);
	    }

	}
	/* システム辞書ディレクトリ指定 */
	else if (!strcmp(argv[arg], "-i")) {
	  if(iflag) { /* ２回指定されたらエラー */
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
	/* ユーザ辞書ディレクトリ指定 */
	else if (!strcmp(argv[arg], "-u")) {
	  if(uflag) { /* ２回指定されたらエラー */
	    usage(Progname);
	  }
	  else {
	    uflag = 1;
	    if(arg +2 > argc) { /* 次の引数が無いのでエラー */
		usage(Progname);
	    }
	    if(*user == (char)NULL) {
		(void)strcpy(user, argv[++arg]);
	    } else {
		(void)strcat(user, ":");
		(void)strcat(user, argv[++arg]);
	    }
	  }
	} else { /* -is -irohaserver -i -u 以外の指定ならusageを出す */
	    usage(Progname);
	}
    }

    /* 引数でユーザが指定されなければ自分の辞書をプリントアウトします */
    if (*user == (char)NULL ) {
      if (uflag) { /* ユーザ名にNULLが渡ってきたらエラー */
	usage(Progname);
      }
      (void)strcpy(user, searchuname());
    }
    
    (void)makelist(); /* 辞書リスト作成 */    
    
    /*
     *    辞書一覧をプリントアウトして終わりです。
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
	(void)fprintf(stderr, "メモリを確保できません。\n");
	break;
      case BADCONT :
	(void)fprintf(stderr,"コンテクストの値が異常です。\n");
	break;
      default:
	if (init[0] == '/') {
	    (void)fprintf(stderr, 
"かな漢字変換サーバが辞書メンテナンスをサポートしていません。\n");
	} else {
	    (void)fprintf(stderr,
"かな漢字変換サーバ\" %s\"が辞書メンテナンスをサポートしていません。\n",
			  init);
	}
	break;
    }
    exit(ERR_VALUE);
}





