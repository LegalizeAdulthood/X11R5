/*
 * $Id: ujisf.c,v 1.2 1991/09/16 21:32:27 ohm Exp $
 */
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
  Ujis format <--> internal data.
  */

int wnnerror;

#include <stdio.h>
#include "commonhd.h"
#include "jslib.h"
#include "jh.h"
#include "jdata.h"

#ifdef CHINESE
#include "cplib.h"
int pzy_flag = CWNN_PINYIN;	/* Pinyin or Zhuyin */
static void sisheng_num(), read_kanji_str_w();
#endif

#ifndef min
#define min(a, b) ((a > b)? b:a)
#define max(a, b) ((a < b)? b:a)
#endif

static void Kanjistradd(), bunpou_num(), read_kanji_str(), toesc();
void exit1();

extern struct JT jt;

/* extern variables */

struct je **jeary;

#define WNN_HINSI_LEN 4096

w_char file_comment[WNN_COMMENT_LEN];
w_char hinsi_list[WNN_HINSI_LEN];



/* static variables */
static UCHAR *heap, *hp, *heapend;
static w_char *yomi_heap, *yhp, *yheapend;
static struct je *je_heap, *jehp, *jeheapend;
static FILE *ifpter;
static int maxline;

/* extern functions of this file are
   ujis_header();
   read_ujis(reversep, to_esc, which_dict);
   reverse_yomi();
   sort();
   uniq_je(func);
   output_ujis(opter, serial_out, esc_exp);
   */

int lc;

static char stack[LINE_SIZE] = {0};

int
get_line(c)
register char *c;
{
    if(stack[0]){
	strcpy(c, stack);
	stack[0] = 0;
    }else{
	if(fgets(c, LINE_SIZE, ifpter) == NULL){
	    return(EOF);
	}
    }
    return(0);
}

void
unget_line(c)
char *c;
{
    strcpy(stack, c);
}


char *
get_string(str,buf)
register char *str;
char *buf;
{
    register char *c = buf;
    for(;*c == '\t' || *c == ' ';c++);
    if(*c == '\0' || *c == '\n'){
	*str = 0;
	return(NULL);
    }
    for(;*c != '\t' && *c != ' ' && *c != '\n' && *c != '\0';c++){
	*str++ = *c;
    }
    *str = 0;
    return(c);
}

void
bad_line(bf)
char *bf;
{
    static int badl = 0;
    
    fprintf(stderr, "Bad line \"%s\"\n" , bf);
    fprintf(stderr, "Bad line omitted\n");
    if(++badl > BADLMAX){
	fprintf(stderr, "Too many bad lines.\n");
	exit1();
    }
}

void
error_no_heap()
{
    fprintf(stderr, "Heap area is exhausted.\n");
    exit1();
}

static int
get_one_line(buffer, jep, rev, to_esc, which_dict)
char *buffer;
register struct je **jep;
int rev;
int to_esc;
int which_dict;
{
    register char *c = buffer;
    static    char tmp[LINE_SIZE];
    static    char ckanji[LINE_SIZE];
    static    char cyomi[LINE_SIZE];
    static    w_char yomi[LINE_SIZE];
    static    w_char kanji[LINE_SIZE];
    static    w_char comm[LINE_SIZE];
#ifdef CHINESE
    static w_char un_sisheng_yincod_str[LINE_SIZE];
    static w_char yincod_str[LINE_SIZE];
    static char	  csisheng[LINE_SIZE];
    static w_char sisheng[LINE_SIZE];
    static w_char wtmp[LINE_SIZE];
#endif
    char *c1;
    
    if(jehp == jeheapend){
	if((jehp = je_heap =  (struct je *)malloc((HEAPINC * sizeof(struct je)))) == NULL){
	    fprintf(stderr, "Malloc Failed\n");
	    return(-1);
	}
	jeheapend = je_heap + HEAPINC;
    }
    *jep = jehp;
    jehp++;

    if(rev == REVERSE){
	if((c = get_string(ckanji,c)) == NULL)return(1);
    }else{ 
	if((c = get_string(cyomi,c)) == NULL)return(1);
    }
    if(rev == REVERSE){
	if((c = get_string(cyomi,c)) == NULL)return(-1);
    }else{ 
	if((c = get_string(ckanji,c)) == NULL)return(-1);
    }
#ifdef CHINESE
/* here ,should seperate pinyin to two part    */
/* one is usually pinyin string like Zhong.Guo.  */
/* the is sisheng string like 23                */

    if (which_dict == CWNN_REV_DICT || which_dict == BWNN_REV_DICT) { 
	if (pzy_flag == CWNN_ZHUYIN)
	    cwnn_zy_str_analysis(cyomi, csisheng,
				 un_sisheng_yincod_str, yincod_str);
	else
	    cwnn_py_str_analysis(cyomi, csisheng,
			         un_sisheng_yincod_str, yincod_str);

	sisheng_num(csisheng,&((*jep)->ss));
	read_kanji_str_w(wtmp,un_sisheng_yincod_str);
	Strcpy(yomi,wtmp);
    } else {
	read_kanji_str(tmp, cyomi); 
	Sstrcpy(yomi, tmp);
    }
#else
    read_kanji_str(tmp, cyomi);
    Sstrcpy(yomi, tmp);
#endif
    if(Strlen(yomi) >= LENGTHYOMI){
	fprintf(stderr , "YOMI is longer in line %d.\n" , lc);
	return(-1);
    }
    w_stradd(yomi, &((*jep)->yomi));
    
    read_kanji_str(tmp, ckanji);
    Sstrcpy(kanji, tmp);
    if(Strlen(kanji) >= LENGTHYOMI){
	fprintf(stderr , "KANJI is longer in line %d.\n" , lc);
	return(-1);
    }
    w_stradd(kanji, &((*jep)->kan));

    if((c = get_string(tmp,c)) == NULL)return(-1);
    bunpou_num(tmp, &((*jep)->hinsi));
    
    if((c = get_string(tmp,c)) == NULL)return(-1);
    if(tmp[0] == '-'){
	(*jep)->hindo = -1;	/*  Real hindo == -1 means Not to use it */
    }else{
	sscanf(tmp,"%d",&((*jep)->hindo));
    }    

    if((get_string(tmp,c)) == NULL){
	c1 = NULL;
	(*jep)->comm = NULL;
	comm[0] = 0;
    }else{
/*    left entries are all considered as comment */	
	for(;*c == '\t' || *c == ' ';c++);
	if(c[strlen(c) - 1] == '\n')c[strlen(c) - 1] = '\0';
	c1 = c;
	Sstrcpy(comm, c1);
	if(Strlen(comm) >= LENGTHYOMI){
	    fprintf(stderr , "COMMENT is longer in line %d.\n" , lc);
	    return(-1);
	}
	w_stradd(comm, &((*jep)->comm));
    }

    if(to_esc){
	toesc(ckanji, cyomi);
    }
/*
    if(strchr(ckanji, DIC_COMMENT_CHAR) || 
       strchr(ckanji, DIC_YOMI_CHAR)){
	fprintf(stderr, "Bad character in kanji\n");
	return(-1);
    }
    if(which_dict){
	if(strchr(cyomi, DIC_COMMENT_CHAR) || 
	   strchr(cyomi, DIC_YOMI_CHAR)){
	    fprintf(stderr, "Bad character in yomi\n");
	    return(-1);
	}
    }
*/
    Kanjistradd(kanji, 
#ifdef CHINESE
		((which_dict == CWNN_REV_DICT || which_dict == BWNN_REV_DICT) ?
		    yincod_str : ((which_dict == WNN_REV_DICT) ? yomi:NULL)),
#else
		(which_dict)? yomi:NULL,
#endif
		comm,
		&(*jep)->kanji);
    return(0);
}

static void
Kanjistradd(k, y, c, cp)
register UCHAR **cp;
w_char *k, *y, *c;
{
    int len;
    if(hp + LENGTHKANJI >= heapend){
	if((hp = heap = (UCHAR *)malloc((HEAPINC * HEAP_PER_LINE))) == NULL){
	    fprintf(stderr, "Malloc Failed\n");
	    exit(1);
	}
	heapend = heap + (HEAPINC * HEAP_PER_LINE);
    }
    *cp = hp;
    if((len = kanjiaddr(hp, k, y, c)) >= LENGTHKANJI){
	fprintf(stderr , "KANJI is longer in line %d.\n" , lc);
	exit(1);
    }
    hp += len;
}

int
w_stradd(str, cp)
register w_char **cp;
register w_char *str;
{
    register int len = Strlen(str);
    
    if(yhp + len + 1 >= yheapend){
	if((yhp = yomi_heap = (w_char *)malloc((HEAPINC * sizeof(w_char)))) == NULL){
	    fprintf(stderr, "Malloc Failed\n");
	    return(-1);
	}
	yheapend = yomi_heap + HEAPINC;
    }
    *cp = yhp;
    Strcpy(yhp,str);
    yhp += len + 1;
    return(0);
}

void
#ifdef CHINESE
ujis_header(which_dict)
int *which_dict;
#else
ujis_header()
#endif
{
    char buffer[LINE_SIZE];
    char *c = buffer;
    char str[LINE_SIZE];

    jt.total = 0;
    file_comment[0] = 0;
    hinsi_list[0] = 0;
    
    for(;;){
	if(get_line(buffer) == EOF){
	    goto EOF_HEAD;
	}
	c = buffer;
	if((c = get_string(str,c)) == NULL) continue;
	if(strcmp(str, COMMENT) == 0){
/*	    for(;;){
		if(get_line(buffer) == EOF){
		goto EOF_EHAD;
		}
		if(buffer[0] == '\\'){
		    unget_line(buffer);
		    break;
		}
		if(Strlen(file_comment) + strlen(buffer)
		    >= WNN_COMMENT_LEN){
		    fprintf(stderr, "Too Long Comment.\n");
		    exit1();
		}
		Sstrcpy(file_comment + Strlen(file_comment), buffer);
	    }
*/	    
	    get_string(str, c);
/*
	    if(str[strlen(str) - 1] == '\n'){
		c[strlen(str) - 1] = '\0';
	    }
*/
	    Sstrcpy(file_comment, str);
#ifdef CHINESE
	}else if(strcmp(str, PINYIN) == 0){
		*which_dict = CWNN_REV_DICT;
		pzy_flag = CWNN_PINYIN;
	}else if(strcmp(str, ZHUYIN) == 0){
		*which_dict = CWNN_REV_DICT;
		pzy_flag = CWNN_ZHUYIN;
	}else if(strcmp(str, BIXING) == 0){
		*which_dict = BWNN_REV_DICT;
#endif
	}else if(strcmp(str, HINSI) == 0
#ifdef CHINESE
		|| strcmp(str, CHINSI) == 0
#endif
	){
	    for(;;){
		if(get_line(buffer) == EOF){
		    goto EOF_HEAD;
		}
		if(buffer[0] == '\\' || buffer[0] == '\n'){
		    unget_line(buffer);
		    break;
		}
		Sstrcpy(hinsi_list + Strlen(hinsi_list), buffer);
	    }
	}else if(strcmp(str, TOTAL) == 0){
	    get_string(str, c);
	    jt.total = atoi(str);
	}else if(strcmp(str, DIC_NO) == 0){/* for word_reg.c */
	    get_string(str, c);
	    jt.total = atoi(str);
	}else{
	    unget_line(buffer);
	    break;
	}
    }
 EOF_HEAD:
    jt.maxcomment = Strlen(file_comment);
    jt.maxhinsi_list = Strlen(hinsi_list) + 1;
}

void
read_ujis(rev, to_esc, which_dict)
int rev;
int to_esc;
int which_dict;
{
    char buffer[LINE_SIZE];
    register int tmp;
    
    for(lc = 0;get_line(buffer) != EOF;){
	if((tmp = get_one_line(buffer, jeary + lc, rev, to_esc, which_dict)) == -1){
	    bad_line(buffer);
	}else if(tmp == 0){ /* succeed */
	    lc++;
	    if(lc > maxline) {
		error_no_heap();
	    }
	}
    }
    jt.maxserial = lc;	   /* i starts with 1 in order to leave 0 unused */
#ifdef CHINESE
    jt.syurui = which_dict;
#endif
}  

void
reverse_yomi()
{
    register int i;
    w_char ytmp[LINE_SIZE];
    
    for(i = 0 ; i < jt.maxserial ; i++){
	if (jeary[i]->yomi != 0) { /* 削除されてないものだけ */
	    Sreverse(ytmp, jeary[i]->yomi);
	    Strcpy(jeary[i]->yomi, ytmp);
	    Sreverse(ytmp, jeary[i]->kan);
	    Strcpy(jeary[i]->kan, ytmp);
	}

    }
}

extern char *wnn_get_hinsi_name();

void
print_je(jep,opter, serial_out, esc_exp)
register FILE *opter;
register struct je *jep;
int serial_out;
int esc_exp;
{
/*    if (jep->yomi != 0) { */
    if(jep-> hinsi != SAKUJO_HINSI){
	Print_entry(jep->yomi, jep->kan, jep->comm, jep->hindo, 0, jep->hinsi, 
		    serial_out? jep->serial:-1, opter, esc_exp);
    }
}

#ifdef nodef
kprint(fp , kpter)
register FILE *fp;
register w_char *kpter;
{
    char out_str[LENGTHKANJI];
    register int out_len;
    char tmp[LENGTHKANJI];
    
    sStrcpy(tmp, kpter);
    out_len = make_kanji_str(out_str, tmp);
    fprintf(fp ,"%s" , out_str);
    if(out_len < 8) putc('\t' , fp);
    if(out_len < 16)putc('\t' , fp);
    putc('\t' , fp);
}
#endif

void
output_ujis(opter, serial_out, esc_exp)
register FILE *opter;
int serial_out;
int esc_exp;
{
    register struct je **jep;
    char buffer[WNN_COMMENT_LEN + WNN_HINSI_LEN];
    register int i;
    
    sStrcpy(buffer, file_comment);
    fprintf(opter, "%s\t%s\n", COMMENT, buffer);
    fprintf(opter, "%s\t%d\n", TOTAL, jt.total);
    sStrcpy(buffer, hinsi_list);
#ifdef CHINESE
    fprintf(opter, "%s\n", CHINSI);
    if (jt.syurui == CWNN_REV_DICT) {
	if (pzy_flag == CWNN_PINYIN) {
	    fprintf(opter, "%s\n", PINYIN);
	} else {
	    fprintf(opter, "%s\n", ZHUYIN);
	}
    } else if (jt.syurui == BWNN_REV_DICT) {
	fprintf(opter, "%s\n", BIXING);
    }
#else
    fprintf(opter, "%s\n", HINSI);
#endif
    fprintf(opter, "%s", buffer);
    fprintf(opter, "\n");
    for(jep = jeary, i = 0; i < jt.maxserial ; i++, jep++){
	print_je(*jep, opter, serial_out, esc_exp);
    }
}  

int
init_heap(hpb,yhpb,l,rl,ipf)
int hpb, yhpb, l, rl;
FILE *ipf;
{
    jehp = je_heap =  (struct je *)malloc((rl * sizeof(struct je)));
    hp = heap = (UCHAR *)malloc(hpb);
    yhp = yomi_heap = (w_char *)malloc((yhpb * sizeof(w_char)));
    if((jeary = (struct je **)calloc(l, sizeof(struct je *))) == NULL){
	fprintf(stderr, "Malloc Failed\n");
	return(-1);
    }
    ifpter = ipf;
    maxline = l;
    heapend = heap + hpb;
    yheapend = yomi_heap + yhpb;
    jeheapend = je_heap + rl;
    return(0);
}

void
init_jeary()
{
    int k;
    for(k = 0; k < jt.maxserial ; k++){
	jeary[k] = je_heap + k;
    }	
}

/* test  program
   main()
   {
   yhp = yomi_heap = (w_char *)malloc(100000);
   jeary = (struct je *)malloc(100000);
   
   ifpter = stdin;
   ujis_header();
   read_ujis();
   
   sort();
   uniq_je(func);
   output_ujis(stdout, 0, 1);
   }
   */

void
exit1()
{
    exit(1);
}

/* must be updated later */

static void
bunpou_num(a, p)
register char *a;
register int *p;
{
    int tmp;
    if((tmp = wnn_find_hinsi_by_name(a)) == -1){
	if(sscanf(a, "%d", p) == 0){
	    fprintf(stderr, "Bad hinsi name \"%s\".\n",a);
	    exit1();
	}
    }else{
	*p = tmp;
    }
#ifdef nodef
    sscanf(a,"%d",p);
#endif
}

#ifdef CHINESE
static void
sisheng_num(a, p)
register char *a;
register int *p;
{
    sscanf(a,"%d",p);
}
#endif

int
sort_func_je(a,b)
char *a, *b;
{
    return(sort_func(a, b, D_YOMI));
}

int
sort_func_je_kanji(a,b)
char *a, *b;
{
    return(sort_func(a, b, D_KANJI));
}

int
sort_func(a, b, which)
register char *a, *b;
int which;
{
    register int tmp;
    register struct je *pa, *pb;
    pa = *((struct je **)a);
    pb = *((struct je **)b);
    if(pa->hinsi == SAKUJO_HINSI){
	if(pb->hinsi == SAKUJO_HINSI) return(0);
	return(-1);
    }
    if(pb->hinsi == SAKUJO_HINSI) return(1);

    if(which == D_YOMI){
	    
	if(!(pa->yomi) || !(pb->yomi))return(0); 
	tmp = Strcmp(pa->yomi,pb->yomi);
	if(tmp)return(tmp);

	if(pa->hinsi != pb->hinsi){
	    return ((int)(pa->hinsi) - (int)(pb->hinsi));
	}

#ifdef CHINESE
        if(jt.syurui == CWNN_REV_DICT){
	    if(pa->ss != pb->ss){
	  	 return ((int)(pa->ss) - (int)(pb->ss));
	    }
        }
#endif
	if(!(pa->kan) || !(pb->kan))return(0);
	tmp = Strcmp(pa->kan,pb->kan);
	if(tmp)return(tmp);
    }else{
	if(!(pa->kan) || !(pb->kan))return(0);
	tmp = Strcmp(pa->kan,pb->kan);
	if(tmp)return(tmp);

	if(pa->hinsi != pb->hinsi){
	    return ((int)(pa->hinsi) - (int)(pb->hinsi));
	}
#ifdef CHINESE
        if(jt.syurui == CWNN_REV_DICT){
	    if(pa->ss != pb->ss){
	  	 return ((int)(pa->ss) - (int)(pb->ss));
	    }
        }
#endif
	if(!(pa->yomi) || !(pb->yomi))return(0); 
	tmp = Strcmp(pa->yomi,pb->yomi);
	if(tmp)return(tmp);
    }
    return(0);
}

void
sort()
{
    qsort((char *)jeary,jt.maxserial, sizeof(struct je *),sort_func_je);
}

void
sort_if_not_sorted()
{
    if(!Sorted((char *)jeary,jt.maxserial, sizeof(struct je *),sort_func_je)){
	sort();
    }
}

void
sort_kanji()
{
    qsort((char *)jeary,jt.maxserial, sizeof(struct je *),sort_func_je_kanji);
}

void
uniq_je(func)
int (*func)();
{
    int k;
    struct je **prev, **jep;

    if(jt.maxserial == 0) return;
    prev = &jeary[0];
    for(k = 1; k < jt.maxserial; k++){
	jep = &jeary[k];
	if(func((char *)jep, (char *)prev) == 0){
	    w_char tmp[LENGTHYOMI];
	    char tmp1[LENGTHYOMI];
	    char tmp2[LENGTHKANJI];
#ifdef CHINESE
            char tmp3[LENGTHKANJI];
	    if (jt.syurui == BWNN_REV_DICT || jt.syurui == CWNN_REV_DICT){
		Strcpy(tmp,(*jep)->yomi);
		sStrcpy(tmp1, tmp);
		Strcpy(tmp, (*jep)->kan);
		sStrcpy(tmp2, tmp);
		sprintf(tmp3,"%d",(*jep)->ss);
            } else {
#endif
		Sreverse(tmp, (*jep)->yomi);
		sStrcpy(tmp1, tmp);
		Sreverse(tmp, (*jep)->kan);
		sStrcpy(tmp2, tmp);
#ifdef CHINESE
	    }
	    if (jt.syurui == CWNN_REV_DICT)
		fprintf(stderr,
"Entries with same yomi(%s), kanji(%s), hinsi(%s),sisheng(%s) are marged.\n",
		    tmp1, tmp2, wnn_get_hinsi_name((*jep)->hinsi),tmp3);
	    else
#endif
		fprintf(stderr,
"Entries with same yomi(%s), kanji(%s), hinsi(%s) are marged.\n",
		    tmp1, tmp2, wnn_get_hinsi_name((*jep)->hinsi));
	}else{
	    prev++;
	    if(prev != jep){
		*prev = *jep;
	    }
	}
    }
    prev++;
    jt.maxserial = prev - &jeary[0];
}

#ifdef nodef
make_kanji_str(o , c)
register UCHAR *o , *c;
{
    register UCHAR *o0 = o;
    
    for(;*c;c++){
	if(*c == '\\'){
	    *o++ = '\\';
	    *o++ = '\\';
	}else if(*c > 0x20){
	    *o++ = *c;
	}else{
	    sprintf(o , "\\0%o" , *c);
	    for(;*o;o++);
	}
    }
    *o = 0;
    return(o - o0);
}
#endif

static void
read_kanji_str(c , o)
register char *c , *o;
{
    for(;*o;c++){
	if(*o == '\\'){
	    if(*++o == '0'){
		o += 1;
		if(*o >= '0' && *o <= '7'){
		    *c = (*o++ - '0');
		}else continue;
		if(*o >= '0' && *o <= '7'){
		    *c *= 8;
		    *c |= (*o++ - '0');
		}else continue;
	    }else{
		*c = *o++;
	    }
	}else{
	    *c = *o++;
	}
    }
    *c = 0;
}

#ifdef CHINESE
static void
read_kanji_str_w(c , o)
register w_char *c , *o;
{
    for(; *o; c++){
	if(*o == (w_char)'\\'){
	    if(*++o == (w_char)'0'){
		o += 1;
		if(*o >= (w_char)'0' && *o <= (w_char)'7'){
		    *c = (*o++ - (w_char)'0');
		}else continue;
		if(*o >= (w_char)'0' && *o <= (w_char)'7'){
		    *c *= 8;
		    *c |= (*o++ - (w_char)'0');
		}else continue;
	    }else{
		*c = *o++;
	    }
	}else{
		*c = *o++;
	}
    }
    *c = 0;
}
#endif

int
Sorted(st, lc, size, sort_fun)
register char *st;
register int lc;
int size;
int (*sort_fun)();
{
    char *dst = st + size;
    for(lc --; lc > 0; lc--, st = dst, dst += size){
	if(sort_fun(st, dst) > 0){
	    return(0);
	}
    }
    return(1);
}

int
is_katakana(k, y)
register char *k, *y;
{
    for(;*k && *y;){
	if(*y == (char)0xa1 && *k == (char)0xa1 && 
	   *(y + 1) == (char)0xbc && *(y + 1) == (char)0xbc) {/*"ー" */
	    y+= 2;k+= 2; continue;
	}
	if (*y++ != (char)0xa4 || *k++ != (char)0xa5) return(0);  
	/* be careful, char comparison. */
	if (*y++ != *k++ ){ return(0);}
    }
    return (!(*k | *y));
}

static void
toesc(ckanji, cyomi)
char *ckanji, *cyomi;
{
    if(strcmp(ckanji, cyomi) == 0){
	strcpy(ckanji, DIC_HIRAGANA);
    }else if(is_katakana(ckanji, cyomi)){
	strcpy(ckanji, DIC_KATAKANA);
    }
}
