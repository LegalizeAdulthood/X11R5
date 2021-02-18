#ifndef lint
static char *rcs_id = "$Id: oldatonewa.c,v 1.2 1991/09/16 21:32:25 ohm Exp $";
#endif /* lint */
/*
 * Copyright Kyoto University Research Institute for Mathematical Sciences
 *                 1987, 1988, 1989, 1990, 1991
 * Copyright OMRON Corporation. 1987, 1988, 1989, 1990, 1991
 * Copyright ASTEC, Inc. 1987, 1988, 1989, 1990, 1991
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that all of the following conditions are satisfied:
 *
 * 1) The above copyright notices appear in all copies
 * 2) Both those copyright notices and this permission notice appear
 *    in supporting documentation
 * 3) The name of "Wnn" isn't changed unless substantial modifications
 *    are made, or
 * 3') Following words followed by the above copyright notices appear
 *    in all supporting documentation of software based on "Wnn":
 *
 *   "This software is based on the original version of Wnn developed by
 *    Kyoto University Research Institute for Mathematical Sciences (KURIMS),
 *    OMRON Corporation and ASTEC Inc."
 *
 * 4) The names KURIMS, OMRON and ASTEC not be used in advertising or
 *    publicity pertaining to distribution of the software without
 *    specific, written prior permission
 *
 * KURIMS, OMRON and ASTEC make no representations about the suitability
 * of this software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * KURIMS, OMRON, ASTEC AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KURIMS, OMRON, ASTEC OR
 * WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/*	Version 4.0
 */
#include <stdio.h>
#include "commonhd.h"

static char *ohinsi[] = {
    "カ行五段",
    "カ行(行く)",
    "ガ行五段",
    "サ行五段",
    "タ行五段",
    "ナ行五段",
    "バ行五段",
    "マ行五段",
    "ラ行五段",
    "ワ行五段",
    "一段&名詞",
    "一段",
    "サ行(する)&名詞",
    "サ行(する)",
    "ザ行(ずる)",
    "来(こ)",
    "形容詞",
    "形容動詞",
    "名詞",
    "連体詞",
    "副詞",
    "接続詞,感動詞",
    "来(き)",
    "来(く)",
    "接頭語",
    "接尾語",
    "助数詞",
    "数詞",
    "ラ行(下さい)",
    "固有名詞",
    "形容動詞(たる)",
    "単漢字"
  };

#define LINE_SIZE 1024

static int get_line1();
static char *get_string();

void
main()
{
  char buffer[LINE_SIZE];

  fgets(buffer, LINE_SIZE, stdin);
  printf("\\total %s",buffer);
  for(;fgets(buffer, LINE_SIZE, stdin) != NULL;){
    get_line1(buffer);
  }
  exit (0);
}

static int
get_line1(buffer)
char *buffer;
{
  char *get_string();
  char *c = buffer;
  char tmp[LINE_SIZE];
  char kanji[LINE_SIZE];
  char yomi[LINE_SIZE];
  /*
  char comment[LINE_SIZE];
  */
  w_char wyomi[LINE_SIZE];
  w_char wyomi1[LINE_SIZE];
  int hinsi;
  int hindo;
  int k;
  int	len;

  if((c = get_string(yomi, c)) == NULL ) return(1);
  Sstrcpy(wyomi,yomi);
  Sreverse(wyomi1,wyomi);
  sStrcpy(yomi,wyomi1);
  if((c = get_string(kanji, c)) == NULL ) return(-1);
  if((c = get_string(tmp, c)) == NULL ) return(-1);
  sscanf(tmp,"%x",&hinsi);
  if((c = get_string(tmp,c)) == NULL ) return(-1);
  sscanf(tmp,"%d",&hindo);
  hindo = motoni2(hindo);
  for(k = 0 ; k < 32 ; k++){
    if(hinsi & (1 << k)){
/*
      printf("%s\t\t%s\t\t%s\t\t%d\n",yomi,kanji,ohinsi[k],hindo);
*/
      printf("%s\t", yomi);
      len = strlen(yomi);
	if(len < 8)	printf("\t");
	if(len < 16)	printf("\t");
      printf("%s\t", kanji);
      len = strlen(kanji);
	if(len < 8)	printf("\t");
	if(len < 16)	printf("\t");
      printf("%s\t", ohinsi[k]);
      len = strlen(ohinsi[k]);
	if(len < 8)	printf("\t");
      printf("%d\n", hindo);
    }
  }
  return(0);
}

static char *
get_string(str,buf)
char *str;
char *buf;
{
  char *c = buf;
  /*
  int k;
  */
  for(;*c == '\t' || *c == ' ';c++);
  if(*c == '\0' || *c == '\n'){
    return(NULL);
  }
  for(;*c != '\t' && *c != ' ' && *c != '\n' && *c != '\0';c++){
    *str++ = *c;
  }
  *str = 0;
  return(c);
}

