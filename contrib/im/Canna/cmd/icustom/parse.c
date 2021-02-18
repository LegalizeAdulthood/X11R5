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
 *
 */

static	char	rcs_id[] = "@(#) 102.1 $Header: /work/nk.R3.1/cmd/icustom/RCS/parse.c,v 3.1 91/06/19 15:52:39 satoko Exp $";

#include <stdio.h>
#include "iroha.h"

#define KANJI

/* char *RomkanaTable = (char *)NULL, RengoGakushu = (char *)NULL;
KatakanaGakushu = (char *)NULL;*/
int InhibitHankakuKana = 0;
char *Dictionary;
int IROHA_ParseError;
static int ptr, len;
extern yylineno;
extern char err_mess[];

static FILE *rcfile;
FILE *fopen();

#define BUF_LEN 1024
#define BACK_BUF 16

static char rbuff[BACK_BUF + BUF_LEN];
static char *buff = rbuff + BACK_BUF;
char IROHA_rcfilename[BUF_LEN] = "";

extern char *allKey[], *alphaKey[], *yomiganaiKey[];
extern char *yomiKey[], *jishuKey[], *tankouhoKey[];
extern char *ichiranKey[], *zenHiraKey[], *zenKataKey[];
extern char *zenAlphaKey[], *hanKataKey[], *hanAlphaKey[];

extern char *allFunc[], *alphaFunc[], *yomiganaiFunc[];
extern char *yomiFunc[], *jishuFunc[], *tankouhoFunc[];
extern char *ichiranFunc[], *zenHiraFunc[], *zenKataFunc[];
extern char *zenAlphaFunc[], *hanKataFunc[], *hanAlphaFunc[];

extern int NallKeyFunc, NalphaKeyFunc, NyomiganaiKeyFunc, NyomiKeyFunc;
extern int NjishuKeyFunc, NtankouhoKeyFunc,  NichiranKeyFunc;
extern int NzenHiraKeyFunc, NzenKataKeyFunc, NzenAlphaKeyFunc;
extern int NhanKataKeyFunc, NhanAlphaKeyFunc;
extern char *RomkanaTable, *RengoGakushu;

before_parse()
{
  int i;

  free_mode_mei();
  init_mode_mei();

  if (RomkanaTable) {
    free(RomkanaTable);
    RomkanaTable = NULL;
  }

  if (nkanjidics) {
    for (i = 0;i < nkanjidics && *kanjidicname[i];i++) {
      if (kanjidicname[i])
	free(kanjidicname[i]);
    }
  }
  if (nuserdics) {
    for (i = 0;i < nuserdics && *userdicname[i];i++) {
      if (userdicname[i])
	free(userdicname[i]);
    }
  }
  if (nbushudics) {
    for (i = 0;i < nbushudics && *bushudicname[i];i++) {
      if (bushudicname[i])
	free(bushudicname[i]);
    }
  }

  if (NallKeyFunc) {
    for (i = 0;i < NallKeyFunc && allKey[i];i++) {
      if (allKey[i])
	free(allKey[i]);
    }
    for (i = 0;i < NallKeyFunc && allFunc[i];i++) {
      if (allFunc[i])
	free(allFunc[i]);
    }
  }
  if (NalphaKeyFunc) {
    for (i = 0;i < NalphaKeyFunc && alphaKey[i];i++) {
      if (alphaKey[i])
	free(alphaKey[i]);
    }
    for (i = 0;i < NalphaKeyFunc && alphaFunc[i];i++) {
      if (alphaFunc[i])
	free(alphaFunc[i]);
    }
  }
  if (NyomiganaiKeyFunc) {
    for (i = 0;i < NyomiganaiKeyFunc && yomiganaiKey[i];i++) {
      if (yomiganaiKey[i])
	free(yomiganaiKey[i]);
    }
    for (i = 0;i < NyomiganaiKeyFunc && yomiganaiFunc[i];i++) {
      if (yomiganaiFunc[i])
	free(yomiganaiFunc[i]);
    }
  }
  if (NyomiKeyFunc) {
    for (i = 0;i < NyomiKeyFunc && yomiKey[i];i++) {
      if (yomiKey[i])
	free(yomiKey[i]);
    }
    for (i = 0;i < NyomiKeyFunc && yomiFunc[i];i++) {
      if (yomiFunc[i])
	free(yomiFunc[i]);
    }
  }
  if (NjishuKeyFunc) {
    for (i = 0;i < NjishuKeyFunc && jishuKey[i];i++) {
      if (jishuKey[i])
	free(jishuKey[i]);
    }
    for (i = 0;i < NjishuKeyFunc && jishuFunc[i];i++) {
      if (jishuFunc[i])
	free(jishuFunc[i]);
    }
  }
  if (NtankouhoKeyFunc) {
    for (i = 0;i < NtankouhoKeyFunc && tankouhoKey[i];i++) {
      if (tankouhoKey[i])
	free(tankouhoKey[i]);
    }
    for (i = 0;i < NtankouhoKeyFunc && tankouhoFunc[i];i++) {
      if (tankouhoFunc[i])
	free(tankouhoFunc[i]);
    }
  }
  if (NichiranKeyFunc) {
    for (i = 0;i < NichiranKeyFunc && ichiranKey[i];i++) {
      if (ichiranKey[i])
	free(ichiranKey[i]);
    }
    for (i = 0;i < NichiranKeyFunc && ichiranFunc[i];i++) {
      if (ichiranFunc[i])
	free(ichiranFunc[i]);
    }
  }
  if (NzenHiraKeyFunc) {
    for (i = 0;i < NzenHiraKeyFunc && zenHiraKey[i];i++) {
      if (zenHiraKey[i])
	free(zenHiraKey[i]);
    }
    for (i = 0;i < NzenHiraKeyFunc && zenHiraFunc[i];i++) {
      if (zenHiraFunc[i])
	free(zenHiraFunc[i]);
    }
  }
  if (NzenKataKeyFunc) {
    for (i = 0;i < NzenKataKeyFunc && zenKataKey[i];i++) {
      if (zenKataKey[i])
	free(zenKataKey[i]);
    }
    for (i = 0;i < NzenKataKeyFunc && zenKataFunc[i];i++) {
      if (zenKataFunc[i])
	free(zenKataFunc[i]);
    }
  }
  if (NzenAlphaKeyFunc) {
    for (i = 0;i < NzenAlphaKeyFunc && zenAlphaKey[i];i++) {
      if (zenAlphaKey[i])
	free(zenAlphaKey[i]);
    }
    for (i = 0;i < NzenAlphaKeyFunc && zenAlphaFunc[i];i++) {
      if (zenAlphaFunc[i])
	free(zenAlphaFunc[i]);
    }
  }
  if (NhanKataKeyFunc) {
    for (i = 0;i < NhanKataKeyFunc && hanKataKey[i];i++) {
      if (hanKataKey[i])
	free(hanKataKey[i]);
    }
    for (i = 0;i < NhanKataKeyFunc && hanKataFunc[i];i++) {
      if (hanKataFunc[i])
	free(hanKataFunc[i]);
    }
  }
  if (NhanAlphaKeyFunc) {
    for (i = 0;i < NhanAlphaKeyFunc && hanAlphaKey[i];i++) {
      if (hanAlphaKey[i])
	free(hanAlphaKey[i]);
    }
    for (i = 0;i < NhanAlphaKeyFunc && hanAlphaFunc[i];i++) {
      if (hanAlphaFunc[i])
	free(hanAlphaFunc[i]);
    }
  }

  nkanjidics = nuserdics = nbushudics = 0;
  NallKeyFunc = NalphaKeyFunc = NyomiganaiKeyFunc = NyomiKeyFunc = 0;
  NjishuKeyFunc = NtankouhoKeyFunc = NichiranKeyFunc = NzenHiraKeyFunc = 0;
  NzenKataKeyFunc = NzenAlphaKeyFunc = NhanKataKeyFunc = NhanAlphaKeyFunc = 0;

}

/* cfuncdef

   parse_string -- ʸ�����ѡ������롣

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

   YYparse -- �������ޥ����ե�������ɤࡣ

   �ե�����ǥ�������ץ��ǻ��ꤵ�줿�ե�������ɤ߹��ࡣ

*/

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

  YYparse_by_rcfilename -- �������ޥ����ե�������ɤ߹��ࡣ

  IROHA_rcfilename �ˤƻ��ꤵ��Ƥ��륫�����ޥ����ե�����򥪡��ץ�
  �ɤ߹��ࡣ

  �����	�� �ɤ߹��᤿���� �ɤ߹���ʤ��ä�

*/

static 
YYparse_by_rcfilename()
{
  FILE *f;

  if (f = fopen(IROHA_rcfilename, "r")) {
    before_parse();

#ifdef DEBUG
    if (iroha_debug)
      printf("cookrc(%s)\n",IROHA_rcfilename);
#endif /* DEBUG */

    YYparse(f);
    fclose(f);
    return 1;
  }
  return 0;
}

/* cfuncdef

  parse -- .iroha �ե������õ���Ƥ����ɤ߹��ࡣ

  parse �ϥ������ޥ����ե������õ�������Υե�����򥪡��ץ󤷥ѡ�����
  �롣

  �ѡ�����Υե������̾���� IROHA_rcfilename ������Ƥ�����

  */

#define NAMEBUFSIZE 1024
#define RCFILENAME  ".iroha"
#define SYSRCDIR    "/usr/lib/iroha/"
#define SYSRCFILE   "/usr/lib/iroha/default.iroha"
#define FILEENVNAME "IROHAFILE"

parse()
{
  char *p, *getenv();
  int n;
  FILE *f;
  int home_iroha_exist = 0;
  extern char *initFileSpecified;

  if (initFileSpecified) {
    strcpy(IROHA_rcfilename, initFileSpecified);
    if (YYparse_by_rcfilename()) {
      return;
    }
    else {
      strcpy(err_mess, IROHA_rcfilename);
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
    /* �Ǹ�ϥ����ƥ�ǥե���ȤΥե�������ɤ� */
    strcpy(IROHA_rcfilename, SYSRCDIR);
    n = strlen(IROHA_rcfilename);
  
    strcpy(IROHA_rcfilename + n, "default");
    strcat(IROHA_rcfilename + n, RCFILENAME);
    if (YYparse_by_rcfilename()) {
      if (p = getenv("DISPLAY")) {
	char display[NAMEBUFSIZE];
	
	DISPLAY_to_hostname(p, display, NAMEBUFSIZE);

	strcpy(IROHA_rcfilename + n, display);
	strcat(IROHA_rcfilename, RCFILENAME);
	YYparse_by_rcfilename();
      }

      if (p = getenv("TERM")) {
	strcpy(IROHA_rcfilename + n, p);
	strcat(IROHA_rcfilename, RCFILENAME);
	YYparse_by_rcfilename();
      }
    }
    else {
      char buf[256];

      sprintf(buf, "�����ƥ�ν�����ե����� %s ��¸�ߤ��ޤ���",
	      IROHA_rcfilename);
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
