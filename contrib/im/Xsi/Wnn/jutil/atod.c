#ifndef lint
static char *rcs_id = "$Id: atod.c,v 1.2 1991/09/16 21:32:17 ohm Exp $";
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
UJIS 形式を、辞書登録可能形式, 及び固定形式辞書に変換するプログラム。
*/

#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"
#ifdef CHINESE
#include "cplib.h"
#endif

static void ujistoud(), output_dic_index(), usage(), sdic_sort(),
  ujistosd(), not_enough_area(), get_pwd(), output_hindo(),
  output_hinsi(), output_kanji(), rev_dic_data(), set_pter1();
void upd_kanjicount(), output_dic_data();

/* Switcher variable between UD and SD */

int which_dict = WNN_REV_DICT;
#ifdef CHINESE
extern int pzy_flag;
static void output_sisheng();
#endif

/* Variables both for UD and SD */
int reverse_dict = NORMAL;

char *com_name;
int maxserial = MAX_ENTRIES;
int kanjicount = 0 ;
char outfile[LINE_SIZE];
struct wnn_file_head file_head;
struct JT jt;
UCHAR *hostart,*hoend;	/* index 2 */
int to_esc = 0;

/* For SD */
#define HONTAI_PER_ENTRY 20
int node_count = 0;
UCHAR *hopter;


/* For UD */
struct uind1 *tary;		/* index 1 */
int tnum = 0;
struct uind2 *uhopter;

char *malloc();
char *hinsi_file_name = NULL;


void
init(argc,argv)
     int argc;
     char **argv;
{
  extern int optind;
  extern char *optarg;
  int c;

  maxserial = MAX_ENTRIES;
  while ((c = getopt(argc,argv,"SURrs:P:p:Nneh:")) != EOF) {
    switch(c){
    case 'S': 
	which_dict = WNN_STATIC_DICT;
	break;
    case 'U': 
	which_dict = WNN_UD_DICT;
	break;
    case 'R':
	which_dict = WNN_REV_DICT;
	break;
    case 'r':
	reverse_dict = REVERSE;
	break;
    case 'P':
	get_pwd(optarg, file_head.file_passwd);
	break;
    case 'p':
	get_pwd(optarg, jt.hpasswd);
	break;
    case 'N':
	strcpy(file_head.file_passwd, "*");
	break;
    case 'n':
	strcpy(jt.hpasswd, "*");
	break;
    case 'e':
	to_esc = 1;
	break;
    case 'h':
	hinsi_file_name = optarg;
	break;
    case 's':
      if(sscanf(optarg , "%d" , &maxserial) == 0){
	usage();
	exit(1);
      }
      break;
    }
  }
  if(to_esc && which_dict == WNN_REV_DICT){
      fprintf(stderr, "You can't make the kanji component of reverse dictionary compact.\n");
      exit(1);
  }
  if (optind) {
    optind--;
    argc -= optind;
    argv += optind;
  }
  if(argc != 2){
    usage();
    exit(1);
  }
  strcpy(outfile , argv[1]);
  if(wnn_loadhinsi(hinsi_file_name) != 0){
      fprintf(stderr, "Can't Open hinsi_file.\n");
      exit(1);
  }
  if(init_heap(DEF_ENTRIES * HEAP_PER_LINE,
	       DEF_ENTRIES * YOMI_PER_LINE,
	       maxserial, DEF_ENTRIES, stdin) == -1)
      exit(1);
}

void
alloc_area()
{
  if(which_dict == WNN_STATIC_DICT){
      if((hostart = (UCHAR *)malloc(maxserial * HONTAI_PER_ENTRY)) == NULL){
	  fprintf(stderr, "Malloc Failed\n");
	  exit(1);
      }
      hopter = hostart;
      hoend = (UCHAR *)hostart + maxserial * HONTAI_PER_ENTRY;
  }else {
      if((tary = (struct uind1 *)malloc(jt.maxserial * sizeof(struct uind1))) == NULL ||
                                /* Too large? */
	 (hostart =(UCHAR *)malloc(jt.maxserial *
		      (sizeof(struct uind2) + sizeof(w_char) * 2))) == NULL){
                                /* 2 must be enough? */
	  fprintf(stderr, "Malloc Failed\n");
	  exit(1);
      }
      hoend = (UCHAR *)((char *)hostart +
	       jt.maxserial * (sizeof(struct uind2) + sizeof(w_char) * 2));
  }
}

extern int sort_func_sdic();
extern int sort_func_je();
FILE *ofpter , *fopen();

void
main(argc,argv)
     int argc ;
     char **argv;
{
  com_name = argv[0];
  init(argc , argv);

#ifdef CHINESE
  ujis_header(&which_dict);	/* read header of UJIS dic  */
#else
  ujis_header();	/* read header of UJIS dic  */
#endif
			/* like comment,total,hinsi  */
#ifdef CHINESE
  read_ujis(reverse_dict, to_esc, which_dict); 

  if (which_dict != CWNN_REV_DICT && which_dict != BWNN_REV_DICT)
      reverse_yomi();
#else
  read_ujis(reverse_dict, to_esc, (which_dict == WNN_REV_DICT)? 1:0);
  reverse_yomi();
#endif

  if((ofpter = fopen(outfile , "w")) == NULL){
      fprintf(stderr , "Can't open the output file %s.\n" , outfile);
      perror("");
      exit(1);
  }

#ifdef CHINESE
  if((which_dict & 0xff) == WNN_REV_DICT){
#else
  if(which_dict == WNN_REV_DICT){
#endif
      create_rev_dict();
  }else{
      alloc_area();
      if(which_dict == WNN_STATIC_DICT){
	  sdic_sort();
	  uniq_je(sort_func_sdic);
	  output_dic_data();
	  ujistosd(0,0);
      }else{
	  sort_if_not_sorted();
	  uniq_je(sort_func_je);
	  output_dic_data();
	  ujistoud();
	  set_pter1();
      }
  }
  output_dic_index();
  rewind(ofpter);
  output_header(ofpter, &jt, &file_head);
#ifdef nodef
  output_comment(ofpter);	/* In order to change the byte order */
  output_hinsi_list(ofpter);	/* In order to change the byte order */	
#endif
  exit (0);
}


w_char *
addyomient(tn , yomi)
register int tn;
register w_char *yomi;
{

  int len = Strlen(yomi);
  tary[tn].yomi2 = 0;
  tary[tn].yomi1 = yomi[0] << 16;

  uhopter->yomi[0] = len;
  if(yomi[1]){
    tary[tn].yomi1 |= yomi[1];
    if(yomi[2]){
      tary[tn].yomi2 = yomi[2] << 16;
      if(yomi[3]){
	tary[tn].yomi2 |= yomi[3];
      }
      if(len > 4){
	Strncpy(uhopter->yomi + 1, yomi + 4,len - 4);
	return(uhopter->yomi + 1 + len - 4);
      }
    }
  }
  return(uhopter->yomi + 1);
}

static void
ujistoud()
{
  register w_char  *yomi;
  w_char dummy = 0; 	/* 2 byte yomi */
  w_char *pyomi;		/* maeno tangono yomi */
  w_char *wcp;
  register int serial_count;
  w_char *kosuup;

  *(int *)hostart = 0;
  uhopter = (struct uind2 *)((int *)hostart + 1);

  yomi = &dummy;

  for(serial_count = 0 ;serial_count < jt.maxserial; serial_count++){
    pyomi = yomi;
    yomi = jeary[serial_count]->yomi;

    if(Strcmp(yomi, pyomi)){
      tary[tnum++].pter = (char *)uhopter - (char *)hostart;
      uhopter->next = 0;
      uhopter->serial = serial_count;
      uhopter->kanjipter = kanjicount;
      uhopter->kosuu = 1;
      kosuup = &uhopter->kosuu;
      wcp = addyomient(tnum - 1 , yomi);
      uhopter = (struct uind2 *)(AL_INT(wcp));
    }else{
      *kosuup += 1;
    }
    upd_kanjicount(serial_count);
  }
}

void
upd_kanjicount(k)
int k;
{
    kanjicount += *jeary[k]->kanji;
}

static void
set_pter1()
{
  register int k;
  register int len;
  w_char oyomi[LENGTHYOMI],nyomi[LENGTHYOMI];
		/* May be a little slow, but simple! */
  int lasts[LENGTHYOMI];			/* pter_to */

  for(k = 0 ; k < LENGTHYOMI; k++){
    lasts[k] = -1;
  }

  for(k = 0 ; k < tnum ; k++){
    len = ((struct uind2 *)((char *)hostart + tary[k].pter))->yomi[0];
    lasts[len] = k;
    for(len--;len ; len--){
      if(lasts[len] >= 0){
	udytoS(oyomi,lasts[len],hostart,tary);
	udytoS(nyomi,k,hostart,tary);
	if(Substr(oyomi,nyomi)){
	  tary[k].pter1 = lasts[len];
	  break;
	}
      }
    }
    if(len == 0) tary[k].pter1 = -1;
  }
}

void
output_dic_data()
{

    fprintf(stderr , "%d words are in this dictionary\n" , jt.maxserial);

    if(little_endian()){ 
	rev_dic_data();
    } 

    jt.syurui = which_dict;
    jt.gosuu = jt.maxserial;
    output_header(ofpter, &jt, &file_head); /* dummy; Will be rewitten.*/
    output_comment(ofpter);
    output_hinsi_list(ofpter);
    output_hindo(ofpter);
    output_hinsi(ofpter);
#ifdef CHINESE
    if (which_dict == CWNN_REV_DICT)
           output_sisheng(ofpter);
#endif
    output_kanji(ofpter);
}

static void
output_dic_index()
{
    if(which_dict == WNN_UD_DICT){
	fprintf(stderr, " tnum = %d\n ind2= %d\n kanji = %d\n",tnum,(char *)uhopter - (char *)hostart,kanjicount);
	jt.maxtable = tnum;
	jt.maxhontai = (char *)uhopter - (char *)hostart;
	jt.maxri2 = jt.maxri1[D_YOMI] = jt.maxri1[D_KANJI] = 0;
	jt.hontai = hostart;	jt.table = tary;
	jt.ri1[D_YOMI] = NULL;
	jt.ri1[D_KANJI] = NULL;
	jt.ri2 = NULL;
#ifdef CHINESE
    }else if((which_dict & 0xff) == WNN_REV_DICT ) {
#else
    }else if(which_dict == WNN_REV_DICT){
#endif
	fprintf(stderr, "kanji = %d\n",kanjicount);
	jt.maxtable = 0;
	jt.maxhontai = 0;
	jt.maxri2 = jt.maxserial;
	jt.hontai = NULL;
	jt.table = NULL;
		/* jt.ri1, jt.ri2 is set in create_rev_dict */
    }else{
	fprintf(stderr, "node_count = %d  ind= %d\n kanji = %d\n",node_count,(char *)hopter - (char *)hostart,kanjicount);
	jt.maxtable = 0;
	jt.maxhontai = (char *)hopter - (char *)hostart;
	jt.maxri2 = jt.maxri1[D_YOMI] = jt.maxri1[D_KANJI] = 0;
	jt.hontai = hostart;
	jt.table = NULL;
	jt.ri1[D_YOMI] = NULL;
	jt.ri1[D_KANJI] = NULL;
	jt.ri2 = NULL;
    }
    jt.maxkanji = kanjicount;

    if(little_endian()){ 
	revdic(&jt, 1);
    } 

    if(which_dict == WNN_UD_DICT){
	fwrite((char *)tary , sizeof(struct uind1), tnum , ofpter);
	fwrite(hostart , 1 , (char *)uhopter - (char *)hostart , ofpter);
#ifdef CHINESE
    }else if((which_dict & 0xff) == WNN_REV_DICT ){
#else
    }else if(which_dict == WNN_REV_DICT){
#endif
	output_ri(ofpter);
    }else{
	fwrite(hostart , 1 , (char *)hopter - (char *)hostart , ofpter);
    }
}


static void
usage()
{
    fprintf(stderr , "Usage : %s [-r -R -S -e -s maximum word count(default %d) -P passwd (or -N) -p hindo_passwd (or -n) -h hinsi_file_name] <dictonary filename>\n",com_name ,MAX_ENTRIES );
  fprintf(stderr , "Input the ascii dictionary from stdin\n" );
  fprintf(stderr , "-r is for creating dictionary with normal and reverse index\n");
  fprintf(stderr , "-R is for creating reverse dictionary\n");
  fprintf(stderr , "-S is for creating static  dictionary.\n");
  fprintf(stderr , "-e is for compacting kanji string.\n");
  exit1();
}

/* SD commands */

int
yStrcmp(a,b)
register w_char *a,*b;
{
  register int c,d;
  for(;*a && *a == *b; a++, b++);
  if(*a == 0) return(- (int) *b);
  if(*b == 0) return((int) *a);
  c = *(a + 1);
  d = *(b + 1);
  if(c == 0 && d == 0) return((int)*a - (int)*b);
  if(c == 0) return(-1);
  if(d == 0) return(1);
  return((int)*a - (int)*b);
}

int
sort_func_sdic(a,b)
char *a, *b;
{
    register int tmp;
    register struct je *pa, *pb;
    pa = *((struct je **)a);
    pb = *((struct je **)b);
    tmp = yStrcmp(pa->yomi,pb->yomi);
    if(tmp)return(tmp);
    /* Changed the order to sort, from yomi->kanji->hinsi 
     * to yomi->hinsi->kanji, in order to separate (struct jdata) by
     * hinsi'es.
     */
    if(pa->hinsi != pb->hinsi) return ((int)(pa->hinsi) - (int)(pb->hinsi));
    return(Strcmp(pa->kan,pb->kan));
}


static void
sdic_sort()
{
    if(!Sorted((char *)jeary,(int)jt.maxserial, sizeof(struct je*),sort_func_sdic)){
   qsort((char *)jeary,(int)jt.maxserial, sizeof(struct je*),sort_func_sdic);
    }
}


static w_char chartable[YOMI_KINDS];
static w_char numtable1[YOMI_KINDS] ;
static w_char *numtable = numtable1 + 1;
static int endt = 0;

int
analize_size(start_je,level,statep,end_jep,mid_jep)
int start_je,level;
int *statep,*end_jep,*mid_jep;
{
  w_char *c = jeary[start_je]->yomi;
  register int je;
  int level1 = level + 1;
  int end_je,mid_je;
  register w_char y1,y2;
  register int je1,je2;

  for(;endt-- > 0 ;){
    chartable[endt] = numtable[endt] = 0;
  }
  endt = 0;		/* This line is needed when endt is originally 0*/
  for(je = start_je;je < jt.maxserial && Strncmp(c,jeary[je]->yomi,level) == 0 &&
                    jeary[je]->yomi[level1] == 0;je++);
  *mid_jep = mid_je = je;
  for(je = start_je;je < jt.maxserial && Strncmp(c,jeary[je]->yomi,level) == 0;je++);
  *end_jep = end_je = je;

  je1 = start_je;
  je2 = mid_je;
  y1 = (je1 >= mid_je)? 0xffff: jeary[je1]->yomi[level];
  y2 = (je2 >= end_je)? 0xffff: jeary[je2]->yomi[level];
  for(;;){
    if(y1 > y2){
      chartable[endt] = y2;
      for(numtable[endt] = numtable[endt - 1];
	  je2 < end_je && jeary[je2]->yomi[level] == y2;je2++);
      y2 = (je2 >= end_je)? 0xffff: jeary[je2]->yomi[level];
      endt++;
    }else if(y1 < y2){
      chartable[endt] = y1;
      for(numtable[endt] = numtable[endt - 1];
	  je1 < end_je && jeary[je1]->yomi[level] == y1;je1++,numtable[endt]++);
      y1 = (je1 >= mid_je)? 0xffff: jeary[je1]->yomi[level];
      endt++;
    }else{
      if(y1 >= 0xffff && y2 >= 0xffff)break;
      chartable[endt] = y1;
      for(;je2 < end_je &&jeary[je2]->yomi[level] == y2;je2++);
      for(numtable[endt] = numtable[endt - 1];
	  je1 < mid_je && jeary[je1]->yomi[level] == y1;je1++,numtable[endt]++);
      y2 = (je2 >= end_je)? 0xffff: jeary[je2]->yomi[level];
      y1 = (je1 >= mid_je)? 0xffff: jeary[je1]->yomi[level];
      endt++;
    }      
  }
  if(mid_je == start_je){
    if(endt == 1) *statep = ST_SMALL;
    else *statep = ST_NOENT;
  }else if(mid_je == end_je) *statep = ST_NOPTER;
  else *statep = ST_NORMAL;		

  return(endt);
}  



static void
ujistosd(start_je,level)
int start_je;
int level;
{
  int state;
  int tsize;
  w_char *charst;
  w_char *sumst;
  int *ptrst;
  int mid_je, end_je;
  register int je;
  register int k;

  node_count++;

  tsize = analize_size(start_je,level,&state,&end_je,&mid_je);

  *(w_char *)hopter = state;
  hopter += 2;
  switch(state){
  case ST_NORMAL:
    if(hopter + tsize * 8 + 12 + 16 >= hoend) not_enough_area();
    *(w_char *)hopter = tsize;
    hopter += 2;
    *(int *)hopter = start_je;
    hopter += 4;
    *(int *)hopter = kanjicount;
    hopter += 4;
    charst = (w_char *)hopter;
    sumst = ((w_char *)hopter + tsize + 2); /* + 2 keeps two zero words */
    charst[tsize] = charst[tsize + 1] = 0;
    ptrst = (int *)((w_char *)sumst + tsize);
    hopter = (UCHAR *)(ptrst + tsize);
    for(k = 0;k < tsize;k++){
      charst[k] = chartable[k];
      sumst[k] = numtable[k];
    }      
    for(k = start_je;k < mid_je; k++){
	upd_kanjicount(k);
    }
    for(je = mid_je,k = 0; je < end_je;){
      for(;k < tsize && charst[k] < jeary[je]->yomi[level];k++)ptrst[k] = ENDPTR;
      if(k == tsize)fprintf(stderr, "Error\n");
      ptrst[k] = (char *)hopter - (char *)hostart;
      ujistosd(je,level + 1);
      for(;je < end_je && jeary[je]->yomi[level] == charst[k];je++);
      k++;
    }
    break;
  case ST_NOENT:
    if(hopter + tsize * 8 + 4 + 16 >= hoend) not_enough_area();
    *(w_char *)hopter = tsize;
    hopter += 2;
    charst = (w_char *)hopter;
    ptrst = (int *)AL_INT((w_char *)charst + tsize);
    hopter = (UCHAR *)(ptrst + tsize);
    for(k = 0;k < tsize;k++){
      charst[k] = chartable[k];
    }      
    for(je = mid_je,k = 0; je < end_je;){
      for(;k < tsize && charst[k] < (jeary[je]->yomi)[level];k++)ptrst[k] = ENDPTR;
      if(k == tsize)fprintf(stderr, "Error\n");
      ptrst[k] = (char *)hopter - (char *)hostart;
      ujistosd(je,level + 1);
      for(;je < end_je && (jeary[je]->yomi)[level] == charst[k];je++);
      k++;
    }
    break;
  case ST_NOPTER:
    if(hopter + tsize * 4 + 12 + 16 >= hoend) not_enough_area();
    *(w_char *)hopter = tsize;
    hopter += 2;
    *(int *)hopter = start_je;
    hopter += 4;
    *(int *)hopter = kanjicount;
    hopter += 4;
    charst = (w_char *)hopter;
    sumst = ((w_char *)hopter + tsize + 2); /* + 2 keeps two zero words */
    charst[tsize] = charst[tsize + 1] = 0;
    hopter = (UCHAR *)((w_char *)sumst + tsize);
    for(k = 0;k < tsize;k++){
      charst[k] = chartable[k];
      sumst[k] = numtable[k];
    }      
    for(k = start_je;k < mid_je; k++){
	upd_kanjicount(k);
    }
    break;
  case ST_SMALL:
    if(hopter + 4 + 16 >= hoend) not_enough_area();
    *(w_char *)hopter = chartable[0];
    hopter += 2;
    ujistosd(start_je,level + 1);
    break;
  }
}

#define INCR_HO 150

static void
not_enough_area()
{
    /*
    UCHAR *oldstart = hostart;
    int size = hoend - hostart + INCR_HO * HONTAI_PER_ENTRY;
    */
  fprintf(stderr, "Sorry allocated area is exhausted.node_count = %d\n",node_count);
  fprintf(stderr, "Retry with option -S <dict_size>.\n");
  exit(1);

/*
 * Only for Sdic 
 * Can't Realloc!! many pointeres exist on the stack!!
 */

/*
    fprintf(stderr, "Realloc.\n");
    if((hostart = (char *)realloc(hostart, size)) == NULL){
	fprintf(stderr, "Ralloc Failed\n");
	exit(1);
    }
    hopter = hostart + (hopter - oldstart);
    hoend = (char *)((char *)hostart + size);
*/    
}


static void
get_pwd(fname, cripted)
char *fname, *cripted;
{
    char pwd[WNN_PASSWD_LEN];
    FILE *fp;

    if((fp = fopen(fname, "r")) == NULL){
	fprintf(stderr, "No password_file %s.\n", fname);
	exit(1);
    }
    fgets(pwd, WNN_PASSWD_LEN, fp);
    fclose(fp);
    new_pwd(pwd, cripted);
}
    
/* output dict routine's */
int
output_comment(fp)
register FILE *fp;
{
    return(put_n_EU_str(fp, file_comment, jt.maxcomment));
}

int
output_hinsi_list(fp)
register FILE *fp;
{
    return(put_n_EU_str(fp, hinsi_list, jt.maxhinsi_list));
}

static void
output_hindo(ofpter)
register FILE *ofpter;
{
    register int i;
    for(i = 0 ; i < jt.maxserial ; i++){
	putc(asshuku(jeary[i]->hindo),ofpter);
    }
}

static void
output_hinsi(ofpter)
register FILE *ofpter;
{
    register int i;
    short k;
    register int little_endianp = little_endian(); 
    /* hinsi ha koko de hikkuri kaesu */

    for(i = 0 ; i < jt.maxserial ; i++){
	k = jeary[i]->hinsi;
	if(little_endianp){
	    rev_short_fun(&k);
	}
	fwrite(&k, 1, 2, ofpter);
    }
}

static void
output_kanji(ofpter)
register FILE *ofpter;
{
    register int i;

    for(i = 0 ; i < jt.maxserial ; i++){
	fwrite(jeary[i]->kanji, 1, *(jeary[i]->kanji) , ofpter);
    }
}

static void
rev_dic_data()
{
    int i;

    /* Comment and Hinsi list do not need to reverse. */
    for(i = 0 ; i < jt.maxserial ; i++){
	rev_w_char((w_char *)(jeary[i]->kanji + 2),
		   *(jeary[i]->kanji) / 2 - 1);
    }
}

#ifdef 	CONVERT_with_SiSheng
static void
output_sisheng(ofpter)
register FILE *ofpter;
{
    register int i;

    for(i = 0 ; i < jt.maxserial ; i++){
        put_short(ofpter,jeary[i]->ss);
    }
} 
#endif 	/* CONVERT_with_SiSheng */
