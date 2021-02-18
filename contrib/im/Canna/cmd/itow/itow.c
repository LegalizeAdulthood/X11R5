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
 * Author : Osamu Hata, NEC Corporation  (hata@d1.bs2.mt.nec.co.jp)
 *
 */

#ifndef lint
static char rcs[]="@(#) 102.1 $Header: /work/nk.R3.1/cmd/itow/RCS/itow.c,v 1.9 91/09/17 18:58:59 hata Exp $";
#endif
/* itow.c  �ƥ����ȷ����μ����ؤ���ʡ٤���ף��Τ�Τ��Ѵ����롣
 *	itow [-f hinshifile] [irohadic] [wnndic]
 */
#include	<stdio.h>
#include        <ctype.h>
#define		MAXTANGO	256
#define         MAXHINSHI       100

static char *default_hinshi[] = {
  "̾��","����","#T35", "��̾","#JN", "��̾","#CN",
  "��ͭ̾��","#KK", "����","#NN",
  "���Ը���","#K5", "���Ը���","#G5", "���Ը���","#S5",
  "���Ը���","#T5", "�ʹԸ���","#N5", "�йԸ���","#B5",
  "�޹Ը���","#M5", "��Ը���","#R5", "��Ը���","#W5",
  "��ư��","����","#KS", "����&̾��","#KSr", "����(�Ԥ�)","#C5r",
  "����(����)","#SX", "����(����)","#ZX","����(����)&̾��","#T30",
  "��(��)","#kxo","��(��)","#kxi","��(��)","#kxuru",
  "��(��)","#sxe","��(��)","#sxi","��(��)","#sxuru",
  "���(������)","#L5",
  "���Ը���","#K5r", "���Ը���","#G5r", "���Ը���","#S5r",
  "���Ը���","#T5r", "�йԸ���","#B5r",
  "�޹Ը���","#M5r", "��Ը���","#R5r", "��Ը���","#W5r",
  "���ƻ�","#KY", "���ƻ�","#KYT", "���ƻ�","#KYna",
  "����(����)&̾��","#T00", "����ư��","#T04", "����ư��","#T06",
  "����","#T07", "����ư��","#T08", "����ư��","#T09",
  "����ư��","#T10", "����ư��","����","#T15", "����","#T16",
  "����","#T31", "����","#T36","����","#T37",
  "����ư��","����ư��&̾��","#T05", "����","����ư��(����)","#F00",
  "����(����)","����ư��&̾��","#F01", "����","����ư��&̾��","#F02",
  "����ư��(����)","#F03", "����","#F04", "����","#F06",
  "����","#F12", "����","#F14", "Ϣ�λ�","#RT", "��³��,��ư��","#CJ",
  "ñ����","#KJ",
  "��Ƭ��","#PRE", "������","#SUC",
  "��Ƭ����","#NNPRE", "������","��Ƭ������","#JS", "����������","#JSSUC",
  "������̾","#JNSUC", "��Ƭ��̾","#CNPRE", "������̾","#CNSUC1",
  "����ư�첽������","#N2T17", "����(����)&̾�첽������","#N2T30",
  "����ư��","#D2T35", "���ƻ첽����ư��","#D2KY",
  "��Ƭ��","#SNPRE", "������̾","#CNSUC2", "������","#N2T35",
  "������","#K2T15", "������","#K2T35", "���Ʋ�������","#ND2KY",
  "���Ʋ�������","#N2KYT", "����ư�첽������","#N2T10",
  "����ư�첽������","#N2T15","����ư�첽������","#N2T18",
  "������","#N2R5","������","#N2K5",
  ""
};

ask_default_hinshi_size()
{
  int i;
  
  for (i = 0; strcmp(default_hinshi[i],""); i++);
  return i;
}

char *salloc(s)
char *s;
{
  char *new;
  
  if (new = (char *)malloc(strlen(s) +1))
    strcpy(new, s);
  else{
    fprintf(stderr, "No more memory\n");
    exit(1);
  }
  return(new);
}

/* �ʻ��б� */
char *
chghinshi(hinshi, size, taiou, fsize)
char   *hinshi, **taiou;
int    size, fsize;
{
  int   i;
  char wnn_hinshi[MAXTANGO];

  wnn_hinshi[0] = '\0';
  for( i = (fsize -1); i >= 0; i -= 2) {
    if(!strcmp(hinshi ,taiou[i])) {
      strcat(wnn_hinshi, taiou[i-1]);
      strcat(wnn_hinshi," ");
    }
  }
  if (wnn_hinshi[0] != '\0') {
    wnn_hinshi[strlen(wnn_hinshi)-1] = '\0';
    return salloc(wnn_hinshi);
  }
  for (i = (size-1); i >= 0; i--) {
    if(*default_hinshi[i] == '#') {
      if(wnn_hinshi[0] != '\0')
	return salloc(wnn_hinshi);
      if(!strcmp(hinshi, default_hinshi[i])) {
	strcpy(wnn_hinshi, default_hinshi[--i]);
}
    }
    else {
      if(wnn_hinshi[0] != '\0') {
	strcat(wnn_hinshi," ");
	strcat(wnn_hinshi, default_hinshi[i]);
      }
    }
  }
  if (wnn_hinshi[0] != '\0')
    return salloc(wnn_hinshi);
  return salloc("");
}

/* �ʻ�ե������ɤ߹��� */
read_hinshi(fp, taiou)
FILE  *fp;
char *taiou[MAXHINSHI];
{
  int  size;
  char H[MAXTANGO], wnn[MAXTANGO], iroha[MAXTANGO];

  size = 0;
  while(fgets(H, MAXTANGO, fp)){
    if(2 != sscanf(H, "%s %s", wnn, iroha))
      continue;
    else {
      taiou[size++] = salloc(wnn);
      taiou[size++] = salloc(iroha);
    }
  }
  taiou[size] = '\0';
  return size;
}
 
char *
get_hindo(iroha_hinshi)
char *iroha_hinshi;
{
  char *p;
  char *hindo;

  for(p = iroha_hinshi; *p; p++)
    if (*p == '*') {
      *p = '\0';
      hindo = ++p;
      return hindo;
    }
  return "0";
}

/* ���� */
itow_write(fp, yomi, hinshi, kouho, hindo)
FILE  *fp;
char *yomi, *hinshi, *kouho, *hindo;
{
  fprintf( fp, "%s %s %s %s \n", yomi, kouho, hinshi, hindo);
}

main(argc, argv)
int  argc;
char *argv[]; 
{
  char *taiou[MAXHINSHI];
  char *nd, *hinshis, hinshi[MAXTANGO], *p;
  char S[MAXTANGO], y[MAXTANGO], h[MAXTANGO], k[MAXTANGO];
  int  hinshiSize,option,fsize = 0;
  FILE *fph,*fpi,*fpo;
  
  hinshiSize = ask_default_hinshi_size();

  option = 0;
  if( argc <= 5 ) { /* �������������� */ 
    if( argc > 2 && !strcmp(argv[1],"-f") ) { /* �ʻ�ե�������ɤ߹��फ�� */
      if( (fph = fopen( argv[2], "r" ) ) == NULL) { /* �ʻ�ե���������� */
	fprintf(stderr,"%s: cannot open %s\n", argv[0], argv[2] );
	exit(2);
      }
      fsize = read_hinshi(fph, taiou);
      close( fph );
      option = 1;
    }
    fpi = stdin;
    fpo = stdout;
  }
  else { /* ���������� */
    fprintf(stderr,"Usage: wtoi [-f hinshitaiou] [wnndic] [irohadic]\n",argv[0]);
    exit(2);
  }
  if( argc >= (2 + option*2) ) { /* ����ʼ���򥪡��ץ� */
    if( (fpi = fopen( argv[1 + option*2], "r" ) ) == NULL) {
      fprintf(stderr,"%s: cannot open %s\n", argv[0], argv[1 + option*2] );
      exit(2);
    }
    if( argc == (3 + option*2) ) { /* �ף���򥪡��ץ� */
      if( (fpo = fopen( argv[2 + option*2], "w" ) ) == NULL) {
	fprintf(stderr,"%s: cannot create %s\n", argv[0], argv[2 + option*2] );
	exit(2);
      }
    }
  }
  
  /* ����� */
  while(fgets(S, sizeof(S), fpi)) {
    if( 3 !=  sscanf(S, "%s %s %s ", y, h, k))
      continue;
    else {
      nd = get_hindo(h);
      if (k[0] == '@') /* ��ά����@�򸵤��᤹ */
	strcpy(k,y);
      if (!strcmp(h,"#kxuru") || !strcmp(h,"#sxuru")) {
	y[strlen(y)-2] = '\0'; /* �ؤ���١ؤ���٤� �ؤ��١ؤ��٤ˤ��� */
	k[strlen(k)-2] = '\0';
      }
      p = hinshi;
      hinshis = chghinshi(h, hinshiSize, taiou, fsize);
      if (!strcmp(hinshis,"")) {
	fprintf(stderr,"�ɤ�:%s  ����:%s  �ʻ�:%s  ��\n",y,k,h);
	fprintf(stderr,"̤������ʻ�Ǥ��Τ��Ѵ����ޤ���\n");
      }
      else {
	for (hinshis; *hinshis; hinshis++, p++) {
	  *p = *hinshis;
	  if (*p == ' ') {
	    *p = '\0';
	    itow_write( fpo, y, hinshi, k, nd );
	    p = hinshi;
	    p--;
	  }
	}
	*p = '\0';
	itow_write(fpo, y, hinshi, k, nd);
      }
    }
  }
  fclose( fpi );
  fclose( fpo );
  exit( 0 );
}
