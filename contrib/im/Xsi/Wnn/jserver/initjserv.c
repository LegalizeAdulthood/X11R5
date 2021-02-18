/*
 * $Id: initjserv.c,v 1.2 1991/09/16 21:31:38 ohm Exp $
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
#include <stdio.h>
#include <ctype.h>
#include "commonhd.h"
#include "ddefine.h"
#include "de_header.h"
#include "wnn_malloc.h"
#include "config.h"
#include "kaiseki.h"

struct wnn_param default_para = {
   2, 10,
/* £Î ¾®Ê¸Àá¿ô */
   2,   34,      5,     80,        5,   1,         20,      0,
/* ÉÑÅÙ ¾®Ê¸ÀáÄ¹ ´´¸ìÄ¹ º£»È¤Ã¤¿¤è ¼­½ñ ¾®Ê¸ÀáÉ¾²Á ÂçÊ¸ÀáÄ¹ ¾®Ê¸Àá¿ô */
   400, -100,400, 80,  200,   2,     200
/* ¿ô»ú ¥«¥Ê ±Ñ¿ô µ­¹æ ÊÄ³ç¸Ì ÉÕÂ°¸ì ³«³ç¸Ì */
};

/* daemon initialize routin */
int
read_default()
{
    FILE *fp, *fopen();
    char data[EXPAND_PATH_LENGTH];
    char code[EXPAND_PATH_LENGTH];
    char s[20][EXPAND_PATH_LENGTH];
    register int num ;
    register int i;

    strcpy(jserver_dir,JSERVER_DIR);

    if((fp = fopen(jserverrcfile , "r")) == NULL){
	perror("");
	printf("Error can't open %s\n" , jserverrcfile);
	return(-1);
    }

    while (fgets(data, EXPAND_PATH_LENGTH, fp) != NULL) {
	num = sscanf(data,
	  "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s",
	       code, s[0],s[1],s[2],s[3],s[4],s[5],s[6],s[7],s[8],s[9],s[10],
	       s[11],s[12],s[13],s[14],s[15],s[16],s[17],s[18],s[19]);
/*
	if(strcmp(code, "jt_len") == 0){
	    jt_len = atoi(s[0]);
	}else if(strcmp(code, "hjt_len") == 0){
	    hjt_len = atoi(s[0]);
	}
*/
	if(strcmp(code, "max_client") == 0){
		sscanf(data,"%s %d ",code,&max_client);
		error1("max_client=%d\n",max_client);
	}else if(strcmp(code, "max_sticky_env") == 0){
		sscanf(data,"%s %d ",code,&max_sticky_env);
		error1("max_client=%d\n",max_client);
	}else if(strcmp((code+1), "server_dir") == 0){
	    strcpy(jserver_dir,s[0]);
	}else if(strcmp(code, "def_param") == 0){
	    change_ascii_to_int(s[0], &default_para.n);
	    change_ascii_to_int(s[1], &default_para.nsho);
	    change_ascii_to_int(s[2], &default_para.p1);
	    change_ascii_to_int(s[3], &default_para.p2);
	    change_ascii_to_int(s[4], &default_para.p3);
	    change_ascii_to_int(s[5], &default_para.p4);
	    change_ascii_to_int(s[6], &default_para.p5);
	    change_ascii_to_int(s[7], &default_para.p6);
	    change_ascii_to_int(s[8], &default_para.p7);
	    change_ascii_to_int(s[9], &default_para.p8);
	    change_ascii_to_int(s[10], &default_para.p9);
	
	    change_ascii_to_int(s[11], &default_para.p10);
	    change_ascii_to_int(s[12], &default_para.p11);
	    change_ascii_to_int(s[13], &default_para.p12);
	    change_ascii_to_int(s[14], &default_para.p13);
	    change_ascii_to_int(s[15], &default_para.p14);
	    change_ascii_to_int(s[16], &default_para.p15);
#ifndef CHINESE
	} else if (strcmp(code, "set_giji_eisuu") == 0 && num >= 2){
	    for (i = 0; i < num - 1; i++) {
		giji_eisuu[i] = expand_argument(s[i]);
	    }
	    for ( ; i < 20; i++) {
		giji_eisuu[i] = 0xffff;
	    }
#endif
	}
    }
    fclose(fp);
    return(0);
}

int
read_default_files()
{
    FILE *fp, *fopen();
    char data[256];
    int num;
    char code[256];
    char file[256];

    if((fp = fopen(jserverrcfile , "r")) == NULL){
	perror("");
	printf("Error can't open %s\n" , jserverrcfile);
	return(-1);
    }
    while(fgets(data,256,fp  ) != NULL){
	num = sscanf(data,"%s %s",code,file ) ;
	if (strcmp(code, "readfile") == 0 && num == 2){
	    read_default_file(file);
	}
    }
    fclose(fp);

    printf("Finished Reading Files\n");
    
    return(0);
}

/*
dummy_env()
{
    int i = 0;
    struct cnv_env *ne;

    if((ne= (struct cnv_env *)malloc(sizeof(struct cnv_env)))==0) return -1;
    env[i]=ne; new_env(i,"dummy_env");
}
*/

int
read_default_file(n)
char *n;
{
    int fid;

    expand_file_name(n);
    fid = file_loaded(n);
    if(fid == -1){  /* Not correct file */
	printf("Error reading %s\n", n);
	return(-1);
    }
    if(FILE_NOT_READ != fid){ /* already read */
	return(-1);
    }
    if((fid= get_new_fid()) == -1){ /* no more file */
	printf("Error reading %s\n", n);
	return(-1);
    }

    files[fid].localf= LOCAL;
    strcpy(files[fid].name, n);
    printf("Reading %s\t Fid = %d\n", n, fid);
    if(read_file(&files[fid])==-1){
	printf("Error reading %s\n", n);
	files[fid].ref_count= -1;	/* fail */
	return(-1);
    }
    files[fid].ref_count= 1;
    return(fid);
}

int
change_ascii_to_int(st,dp)
char *st;
int *dp;
{
    register int total,flag;

    total = 0;
    flag = 0;
    while(*st != NULL){
	if (isdigit(*st)){
	    total = total * 10 + (*st - '0');
	} else if (*st == '+') {
	    if (flag != 0) { return(-1); }
	    flag = 1;
	} else if (*st == '-') {
	    if (flag != 0) { return(-1); }
	    flag = -1;
	} else { return(-1); }
	st++;
    }
    if (flag == 0){
	flag = 1;
    }
    *dp = total * flag;
    return(1);
}


int
expand_argument(st)
register unsigned char *st;
{
  int num;

  if(*st == '^'){
    return ((*(st + 1)) & 0x1f);
  } else if (*st == '\\') {
    return (get_bcksla(st + 1));
  } else if (*st == '0') {
    if (*(st + 1) == 'x' || *(st + 1) == 'X') {
      sscanf((char *)(st + 2),"%x",&num);
      return(num);
    } else {
      sscanf((char *)st,"%o",&num);
      return(num);
    }
  } else if(*st == '\'') {
    if (*(st + 1) < 0x80)
	return (*(st + 1));
    else
	return (*(st + 1) * 0x100 + *(st + 2));
  } else if(!('0' <= *st && *st <= '9')){
    return(-1);
  } else {
    return (atoi(st));
  }
}

int
get_bcksla(st)
register char *st;
{
    int num;

    switch(*st) {
    case 'n':	return('\n');
    case 't':	return('\t');
    case 'b':	return('\b');
    case 'r':	return('\r');
    case 'f':	return('\f');
    case 'x':
    case 'X':
	sscanf(st +1, "%x",&num);
	return(num);
    case 'd':
    case 'D':
	return (atoi(*(st+1)));
    case 'o':
    case 'O':
	sscanf(st +1, "%o",&num);
	return(num);
    }
    return(-1);
}

/*
is_g_digit(c)
w_char c;
{
    register int i;

    if (isascii(c) && isdigit(c) || isjdigit(c)) {
	return(YES);
    } else {
	for (i = 0; i < MAX_GIJI_CHAR; i++) {
	    if (giji_digit[i] == 0xffff)
		return (NO);
	    else if (giji_digit[i] == c)
		return(YES);
	}
    }
    return(NO);
}
*/
