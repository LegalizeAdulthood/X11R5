#ifndef lint
static char *rcs_id = "$Id: dtoa.c,v 1.2 1991/09/16 21:32:21 ohm Exp $";
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
/*
  辞書を ASCII 形式の辞書に変換するプログラム。
  */

#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

#ifdef CHINESE
#include "cplib.h"
extern int pzy_flag;	/* Pinyin or Zhuyin */
#endif

extern void parse_options(), usage(), input_dic(), udtoujis(), kset(),
  sdtoujis(), ghindo();

int which_dict;

char *com_name;
char infile[LINE_SIZE];
int esc_expand = 0;

int serial_out = 0;
int need_sort = 1;
struct JT jt;
struct wnn_file_head file_head;

char *hinsi_file_name = NULL;

void
main(argc, argv)
int argc;
char **argv;
{
    FILE *ifpter;

    com_name = argv[0];

    parse_options(&argc, &argv);

    if((ifpter = fopen(infile , "r")) == NULL){
	fprintf(stderr , "Can't open the input file %s.\n" , infile);
	perror("");
	exit(1);
    }
    if(input_header(ifpter, &jt, &file_head) == -1)exit(1);
    which_dict = jt.syurui;
    input_comment(ifpter);
    input_hinsi_list(ifpter);
    input_dic(ifpter);
    if(little_endian()){ 
	revdic(&jt, 0);
    }
    set_hinsi();
    ghindo(argc - 2 , argv + 2);	/* Gather Hindo */
    if(which_dict == WNN_STATIC_DICT){
	kset();
	sdtoujis(jt.hontai, 0);
#ifdef CHINESE
    }else if((which_dict & 0x00ff) == WNN_REV_DICT){
#else
    }else if(which_dict == WNN_REV_DICT){
#endif
	rdtoujis();
    }else{
	udtoujis();
    }
    if(need_sort){
	sort();
    }
    output_ujis(stdout, serial_out, esc_expand);
    exit (0);
}


void
parse_options(argc, argv)
int *argc;
char ***argv;
{
    int c;
    extern int optind;
    extern char *optarg;

#ifdef CHINESE
    while ((c = getopt(*argc,*argv,"nsezh:")) != EOF) {
#else
    while ((c = getopt(*argc,*argv,"nseh:")) != EOF) {
#endif
	switch(c){
	case 'n': 
	    need_sort = 0;
	    break;
	case 's':
	    serial_out = 1;
	    break;
	case 'e':
	    esc_expand = 1;
	    break;
	case 'h':
	    hinsi_file_name = optarg;
	    break;
#ifdef CHINESE
	case 'z':
	    pzy_flag = CWNN_ZHUYIN;
	    break;
#endif
	default:
	    usage();
	}
    }
    if (optind) {
	optind--;
	*argc -= optind;
	*argv += optind;
    }
    if(*argc != 2){
	usage();
    }
    strcpy(infile, (*argv)[1]);
    if(wnn_loadhinsi(hinsi_file_name) != 0){
	fprintf(stderr, "Can't Open hinsi_file.\n");
	exit(1);
    }
}


void
usage()
{
  fprintf(stderr , "Usage: %s [-nse -h hinsi_file_name] <dictionary file> <hindo file>*\n",com_name);
  fprintf(stderr , "Output the Text dictionary to stdout\n" );
#ifdef CHINESE
  fprintf(stderr , "-z : Output Zhuyin (Default is Pinyin)\n");
#endif
  exit1();
}

void
input_dic(ifpter)
FILE *ifpter;
{

    init_heap(0,			/* jt.maxkanji,*/
	    jt.maxserial * YOMI_PER_LINE * 5, /* may be enough */
	    jt.maxserial, jt.maxserial, stdin);
    jt.table = (struct uind1 *)malloc(jt.maxtable * sizeof(struct uind1));
    jt.hontai = (UCHAR *)malloc(jt.maxhontai);
    jt.ri1[D_YOMI] = (struct rind1 *)malloc(jt.maxri1[D_YOMI] * sizeof(struct rind1));
    jt.ri1[D_KANJI] = (struct rind1 *)malloc(jt.maxri1[D_KANJI] * sizeof(struct rind1));
    jt.kanji = (UCHAR *)malloc(jt.maxkanji + 1);
    jt.ri2 = (struct rind2 *)malloc(jt.maxserial * sizeof(struct rind2));
    jt.hinsi = (w_char *)malloc(jt.maxserial * sizeof(w_char));
    if(!jt.table || !jt.hontai || !jt.ri1[D_YOMI] || !jt.ri1[D_KANJI] || !jt.kanji || !jt.ri2 || !jt.hinsi){
	fprintf(stderr, "Malloc Failed.\n");
	exit(1);
    }
    init_jeary();

    input_hindo(ifpter);
    fread(jt.hinsi, 2, jt.maxserial, ifpter);
#ifdef	CHINESE
    if (which_dict == CWNN_REV_DICT)
	input_sisheng(ifpter);
#endif
    fread(jt.kanji, 1, jt.maxkanji, ifpter);
/*  input_kanji(ifpter); */
    fread(jt.table , sizeof(struct uind1), jt.maxtable , ifpter);
    fread(jt.ri1[D_YOMI], sizeof(struct rind1), jt.maxri1[D_YOMI], ifpter);
    fread(jt.ri1[D_KANJI], sizeof(struct rind1), jt.maxri1[D_KANJI], ifpter);
    fread(jt.hontai , 1 , jt.maxhontai , ifpter);
    fread(jt.ri2, sizeof(struct rind2), jt.maxserial, ifpter);
}

void
udtoujis()
{
    
    w_char  yomi[LENGTHYOMI]; 	/* 2 byte yomi */
    w_char  kan[LENGTHYOMI]; 	/* 2 byte kanji */
    w_char  comm[LENGTHYOMI];
    w_char ryomi[LENGTHYOMI];
    register struct uind2 *hop;
    register int i, k;
    register int serial;
    int len;
    UCHAR *c;
    
    for(i = 0 ; i < jt.maxtable; i++){
	for(hop = (struct uind2 *)(jt.table[i].pter + (char *)jt.hontai);
	    hop != (struct uind2 *)jt.hontai;
	    hop = (struct uind2 *)(hop->next  + (char *)jt.hontai)){
	    /* Though each hop has the same yomi, but calculate yomi each time. */
	    yomi[0] = jt.table[i].yomi1 >> 16;
	    yomi[1] = jt.table[i].yomi1 & 0xffff;
	    if(yomi[1]){
		yomi[2] = jt.table[i].yomi2 >> 16;
		if(yomi[2]){
		    yomi[3] = jt.table[i].yomi2 & 0xffff;
		}
	    }
	    len = hop->yomi[0];
	    Strncpy(yomi + 4, (hop->yomi) + 1, len - 4);
	    yomi[len] = 0;
	    serial = hop->serial;
	    for(k = hop->kosuu, 
		c = hop->kanjipter + jt.kanji
		;k > 0;
		k--, serial++){
		get_kanji_str(c, kan, NULL, comm);
		jeary[serial]->kanji = c;
		c += *c;
		jeary[serial]->serial = serial;
#ifdef	CHINESE
		Strcpy(ryomi, yomi);
#else
		Sreverse(ryomi, yomi);
#endif
		w_stradd(ryomi, &jeary[serial]->yomi);
		w_stradd(kan, &jeary[serial]->kan);
		w_stradd(comm, &jeary[serial]->comm);
	    }
	}
    }
}
/* SD routine */

w_char  yomi[LENGTHYOMI];
int je = 0;

void
kset()
{
    int i;
    UCHAR *c;
    w_char  kan[LENGTHYOMI]; 	/* 2 byte kanji */
    w_char  comm[LENGTHYOMI]; 	/* 2 byte kanji */
    

    c = jt.kanji;
    for(i = 0 ; i < jt.maxserial; i++){
	get_kanji_str(c, kan, NULL, comm);
	w_stradd(kan, &jeary[i]->kan);
	w_stradd(comm, &jeary[i]->comm);
	jeary[i]->kanji = c;
	c += *c;
    }
}

void
sdtoujis(hopter,level)
char *hopter;
register int level;
{
  register int k, j;
  register int tsize;
  register w_char *charst;
  register w_char *sumst;
  int *ptrst;
  static w_char ry[LENGTHYOMI];

  yomi[level + 1] = 0;
  switch(*(w_char *)hopter){
  case ST_NORMAL:
    tsize = *(w_char *)(hopter + 2);
    charst = (w_char *)(hopter + 12);
    sumst = ((w_char *)charst + tsize + 2); /* + 2 keeps two zero words */
    ptrst = (int *)((w_char *)sumst + tsize);
    for(k = 0 ; k < tsize ; k++){
      j = sumst[k] - sumst[k - 1];
      if(j){
	yomi[level] = charst[k];
	for(;j > 0;j--){
	    jeary[je]->serial = je;
#ifdef	CHINESE
            Strcpy(ry, yomi);
#else
	    Sreverse(ry, yomi);
#endif
	    w_stradd(ry, &jeary[je++]->yomi);
	}
      }
    }
    for(k = 0 ; k < tsize ; k++){
      if(ptrst[k] != ENDPTR){
	yomi[level] = charst[k];
	sdtoujis((char *)jt.hontai + ptrst[k],level + 1);
      }
    }
    break;
  case ST_NOENT:
    tsize = *(w_char *)(hopter + 2);
    charst = (w_char *)(hopter + 4);
    ptrst = (int *)AL_INT((w_char *)charst + tsize);
    for(k = 0 ; k < tsize ; k++){
      if(ptrst[k] != ENDPTR){
	yomi[level] = charst[k];
	sdtoujis((char *)jt.hontai + ptrst[k],level + 1);
      }
    }
    break;
  case ST_NOPTER:
    tsize = *(w_char *)(hopter + 2);
    charst = (w_char *)(hopter + 12);
    sumst = ((w_char *)charst + tsize + 2); /* + 2 keeps two zero words */
    for(k = 0 ; k < tsize ; k++){
      j = sumst[k] - sumst[k - 1];
      if(j){
	yomi[level] = charst[k];
	for(;j > 0;j--){
	    jeary[je]->serial = je;
#ifdef	CHINESE
            Strcpy(ry, yomi);
#else
	    Sreverse(ry, yomi);
#endif
	    w_stradd(ry, &jeary[je++]->yomi);
	}
      }
    }
    break;
  case ST_SMALL:
    yomi[level] = *(w_char *)(hopter + 2);
    sdtoujis(hopter + 4,level + 1);
    break;
  }
}

int
rdtoujis()
{
    w_char  yomi[LENGTHYOMI]; 	/* 2 byte yomi */
    w_char  kan[LENGTHYOMI]; 	/* 2 byte yomi */
    w_char  comm[LENGTHYOMI]; 	/* 2 byte yomi */
    register struct rind2 *hop;
    register int hopi;
    register int i;
    register int serial;
#ifdef CHINESE
    char *un_pinyin;
    char *pinyin;
    char *sishengtmp;
    w_char  ryomi[LENGTHYOMI];   /* 2 byte yomi in normal order */
#endif
    
    for(i = 0 ; i < jt.maxri1[D_YOMI]; i++){
	for(hopi = jt.ri1[D_YOMI][i].pter; hopi != RD_ENDPTR; hopi = hop->next[D_YOMI]){
	    hop = hopi + jt.ri2;
	/* Though each hop has the same yomi, but calculate yomi each time. */
	    get_kanji_str(hop->kanjipter + jt.kanji, kan, yomi, comm);
#ifdef CHINESE
	    cwnn_yincod_pzy_str(yomi, yomi, Strlen(yomi), pzy_flag);
#endif
	    serial = hopi;
	    jeary[serial]->kanji = hop->kanjipter + jt.kanji;
	    jeary[serial]->serial = serial;

	    w_stradd(yomi, &jeary[serial]->yomi);
	    w_stradd(kan, &jeary[serial]->kan);
	    w_stradd(comm, &jeary[serial++]->comm);
	}
    }
    return(0);
}

void
ghindo(argc , argv)
int argc;
char **argv;
{
    FILE *fp;
    struct wnn_file_uniq uniq;
    struct wnn_file_head file_head;
    struct HJT hjt;


    while(argc-- > 0){
	if((fp = fopen(*argv++ , "r"))== NULL){
	    fprintf(stderr , "can't open %s\n" , *(argv - 1));
	    perror("error in ghindo");
	    exit(1);
	}
	if(input_hindo_header(fp, &hjt, &file_head) == -1){
	    perror("error in ghindo 1");
	    exit(1);
	}
	if(bcmp(&uniq,&file_head.file_uniq,sizeof(struct wnn_file_uniq))){
	    fprintf(stderr, "%s(%s)Dic File and Hindo File mismatch.\n",
		    infile, *(argv - 1));
	    exit(1);
	}
	add_hindo(fp, hjt.maxserial);
    }
}

int
add_hindo(ifpter, num)
register FILE *ifpter;
int num;
{
    register int i;
    register int k;
    for(i = 0 ; i < num ; i++){
	jeary[i]->hindo += motoni2((k = getc(ifpter)) & 0x7f);
	if(k == -1)return(-1);
    }
  return(0);
}

/* input dict routines */

int
input_comment(fp)
register FILE *fp;
{
    return(get_n_EU_str(fp, jt.maxcomment, file_comment));
}

int
input_hinsi_list(fp)
register FILE *fp;
{
    return(get_n_EU_str(fp, jt.maxhinsi_list, hinsi_list));
}


int
input_hindo(ifpter)
register FILE *ifpter;
{
    register int i;
    register int k;
    for(i = 0 ; i < jt.maxserial ; i++){
	jeary[i]->hindo = motoni2((k = getc(ifpter)) & 0x7f);
	if(k == -1)return(-1);
    }
    return(0);
}

int
set_hinsi()
{
    register int i;

    for(i = 0 ; i < jt.maxserial ; i++){
	jeary[i]->hinsi = jt.hinsi[i];
    }
    return(0);
}

#ifdef 	CONVERT_with_SiSheng
int
input_sisheng(ifpter)
register FILE *ifpter;
{
    register int i;
    unsigned short s;

    for(i = 0 ; i < jt.maxserial ; i++){
        if(get_short(&s, ifpter) == -1)return(-1);
        jeary[i]->ss = s;
    }
  return(0);
}

#endif
