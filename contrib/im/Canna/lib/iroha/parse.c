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

static char rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/lib/iroha/RCS/parse.c,v 3.5 91/08/30 21:48:15 kon Exp $";

#include <stdio.h>
#include "iroha.h"

#define KANJI

int InitialMode, CursorWrap = 1, SelectDirect;
int HexkeySelect = 1, Gakushu = 1, BreakIntoRoman = 0;
int InhibitHankakuKana = 0, QuitIchiranIfEnd = 0;
char *RomkanaTable, *Dictionary;
char *RengoGakushu = (char *)NULL, *KatakanaGakushu = (char *)NULL;
int IROHA_ParseError;
static int ptr, len;
extern yylineno;

static FILE *rcfile;
FILE *fopen();

#define BUF_LEN 1024
#define BACK_BUF 16

static char rbuff[BACK_BUF + BUF_LEN];
static char *buff = rbuff + BACK_BUF;
char IROHA_rcfilename[BUF_LEN] = "";

static int DISPLAY_to_hostname();

/* cfuncdef

   parse_string -- 文字列をパースする。

*/

parse_string(str)
char *str;
{
  rcfile = (FILE *)0;
  ptr = yylineno = 0;
  strcpy(buff, str);
  len = strlen(buff);
  IROHA_ParseError = 0;
  yyparse();
}

/* cfuncdef

   YYparse -- カスタマイズファイルを読む。

   ファイルディスクリプタで指定されたファイルを読み込む。

*/

extern ckverbose;

static
YYparse(f)
FILE *f;
{
  rcfile = f;
  ptr = len = yylineno = 0;
  IROHA_ParseError = 0;
  yyparse();
}

/* cfuncdef

  YYparse_by_rcfilename -- カスタマイズファイルを読み込む。

  IROHA_rcfilename にて指定されているカスタマイズファイルをオープンし
  読み込む。

  戻り値	１ 読み込めた、０ 読み込めなかった

*/

static 
YYparse_by_rcfilename()
{
  FILE *f;

  if (f = fopen(IROHA_rcfilename, "r")) {

#ifdef DEBUG
    if (iroha_debug)
      printf("cookrc(%s)\n",IROHA_rcfilename);
#endif /* DEBUG */

    if (ckverbose == CANNA_FULL_VERBOSE) {
      printf("カスタマイズファイルとして \"%s\" を用います。\n",
	     IROHA_rcfilename);
    }

    YYparse(f);
    fclose(f);
    return 1;
  }
  return 0;
}

/* cfuncdef

  parse -- .iroha ファイルを探してきて読み込む。

  parse はカスタマイズファイルを探し、そのファイルをオープンしパースす
  る。

  パース中のファイルの名前を IROHA_rcfilename に入れておく。

  */

#define NAMEBUFSIZE 1024
#define RCFILENAME  ".iroha"
#define FILEENVNAME "IROHAFILE"

parse()
{
  char *p, *getenv();
  int n;
  FILE *f;
  extern iroha_debug;
  int home_iroha_exist = 0;
  extern initFileSpecified;

  if (initFileSpecified) {
    strcpy(IROHA_rcfilename, initFileSpecified);
    if (YYparse_by_rcfilename()) {
      return;
    }
    else {
      char buf[256];

      if (ckverbose) {
	printf("カスタマイズファイルは読み込みません。\n");
      }

      sprintf(buf, "指定された初期化ファイル %s が存在しません。",
	      IROHA_rcfilename);
      addWarningMesg(buf);
      return;
    }
  }
  if (p = getenv(FILEENVNAME)) {
    strcpy(IROHA_rcfilename, p);
    if (YYparse_by_rcfilename()) 
      return;
  }
  if (p = getenv("HOME")) {
    strcpy(IROHA_rcfilename, p);
    strcat(IROHA_rcfilename, "/");
    strcat(IROHA_rcfilename, RCFILENAME);
    n = strlen(IROHA_rcfilename);

    /* $HOME/.iroha */

    home_iroha_exist = YYparse_by_rcfilename();
    

    if (home_iroha_exist) {

      /* $HOME/.iroha-DISPLAY */

      if (p = getenv("DISPLAY")) {
	char display[NAMEBUFSIZE];
	
	DISPLAY_to_hostname(p, display, NAMEBUFSIZE);
	
	IROHA_rcfilename[n] = '-';
	strcpy(IROHA_rcfilename + n + 1, display);
	
	YYparse_by_rcfilename();
      }
      
      /* $HOME/.iroha-TERM */
      
      if (p = getenv("TERM")) {
	IROHA_rcfilename[n] = '-';
	strcpy(IROHA_rcfilename + n + 1, p);
	YYparse_by_rcfilename();
      }
    }
  }

  if ( !home_iroha_exist ) {
    /* 最後はシステムデフォルトのファイルを読む */
    strcpy(IROHA_rcfilename, LIBDIR);
    n = strlen(IROHA_rcfilename);
  
    strcpy(IROHA_rcfilename + n, "/default");
    strcat(IROHA_rcfilename + n, RCFILENAME);
    if (YYparse_by_rcfilename()) {
      if (p = getenv("DISPLAY")) {
	char display[NAMEBUFSIZE];
	
	DISPLAY_to_hostname(p, display, NAMEBUFSIZE);

	IROHA_rcfilename[n] = '/';
	strcpy(IROHA_rcfilename + n + 1, display);
	strcat(IROHA_rcfilename, RCFILENAME);
	YYparse_by_rcfilename();
      }

      if (p = getenv("TERM")) {
	IROHA_rcfilename[n] = '/';
	strcpy(IROHA_rcfilename + n + 1, p);
	strcat(IROHA_rcfilename, RCFILENAME);
	YYparse_by_rcfilename();
      }
    }
    else {
      char buf[256];

      if (ckverbose) {
	printf("カスタマイズファイルは読み込みません。\n");
      }
      sprintf(buf, "システムの初期化ファイル %s が存在しません。",
	      IROHA_rcfilename);
      addWarningMesg(buf);
    }
  }
}


static
DISPLAY_to_hostname(name, buf, bufsize)
char *name, *buf;
int bufsize;
{
  if (name[0] == ':' || !strncmp(name, "unix", 4)) {
    gethostname(buf, bufsize);
  }
  else {
    int i, len = strlen(name);
    for (i = 0 ; i < len && i < bufsize ; i++) {
      if (name[i] == ':') {
	break;
      }
      else {
	buf[i] = name[i];
      }
    }
    if (i < bufsize) {
      buf[i] = '\0';
    }
  }
}


int
  IROHA_input()
{
  while (ptr == len)
    {
      if (rcfile == (FILE *)NULL
	  || fgets(buff, BUF_LEN, rcfile) == (char *)NULL)
	return (int)NULL;
      
      yylineno++;
      
      ptr = 0;
      len = strlen(buff);
    }
#ifdef KANJI
  return ((buff[ptr] == -1) ? 
	  (ptr++, -1) : (int)(unsigned char)buff[ptr++]);
#else /* not KANJI */
  return ((int)buff[ptr++]);
#endif /* not KANJI */
}

void
  IROHA_unput(c)
{
  buff[--ptr] = c;
}

void
  IROHA_output(c)
{
  putchar(c);
}
