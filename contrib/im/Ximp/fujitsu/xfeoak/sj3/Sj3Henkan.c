/* @(#)Sj3Henkan.c	2.1 91/07/03 10:34:33 FUJITSU LIMITED. */
/******************************************************************

              Copyright 1991, by FUJITSU LIMITED

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and
that both that copyright notice and this permission notice appear
in supporting documentation, and that the name of FUJITSU LIMITED
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.
FUJITSU LIMITED makes no representations about the suitability of
this software for any purpose.  It is provided "as is" without
express or implied warranty.

FUJITSU LIMITED DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL FUJITSU LIMITED BE LIABLE FOR ANY SPECIAL, INDIRECT
OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
OR PERFORMANCE OF THIS SOFTWARE.

  Author: Masahiko Narita      FUJITSU LIMITED
  	  Takanori Tateno      FUJITSU LIMITED
  Modify: Takashi Fujiwara     FUJITSU LIMITED 
          Kiyoaki oya          FUJITSU LIMITED

******************************************************************/

#ifndef lint
static char sccs_id[] = "@(#)Sj3Henkan.c	2.1 91/07/03 10:34:33 FUJITSU LIMITED.";
#endif

#ifdef SJ3_DICTIONARY

#include <stdio.h>
#include <sj3lib.h>
#include "Henkan.h"
typedef unsigned short  ushort;
typedef unsigned char   uchar;

#ifdef DEBUG
#define Static /**/
#else
#define Static static
#endif

#ifndef NULL
#define NULL	(char *)0
#endif

static ushort *_dic_old_buff;

int	Sj3_Argv();
int	Sj3_Help();
int	Sj3_Open2();
int	Sj3_Open();
int	Sj3_Close();
int	Sj3_Henkan();
int	Sj3_Zenkouho();
int	Sj3_HenkanCancel();

DictionaryFuncRec sj3_dictionary_func = {
        "-sj3",
	Sj3_Argv,
	Sj3_Help,
	Sj3_Open2,
	Sj3_Close,
	Sj3_Henkan,
	Sj3_Zenkouho,
	Sj3_HenkanCancel
	};

Static char *uname = NULL;

#define BUFSIZE 	1024
#define True		1
#define False		0

static int	bun_max =0;
static int	open_dic =0;
static int	open_udic =0;

Static int
Sj3_Argv( argc, argv)
    int 	argc;
    char	*argv[];
{
    if(!strcmp(argv[0], "-uname")) {
	uname = argv[1];
	return( 2 );
	}
    return( 0 );
}

Static int
Sj3_Help( fp )
FILE    *fp;
{
    fprintf(fp,"[-uname <user name>]");
}

#define UNAME "xfeoak"

Static int
Sj3_Open2()
{
	char *home;
	if(uname == NULL) {
            uname = (char *)malloc( strlen(UNAME)+1);
            strcpy(uname,UNAME);
        }
	return Sj3_Open( uname );
}

Static int
Sj3_Open(uname)
uchar *uname;
{
	int	ret;
	ret = sj3_open( 0, uname );
	if( ret ) {
		return(-1);
	}
	open_dic = 1;
	return(0);
}

Static int
Sj3_Close(){
	if( open_dic)
		sj3_close();
	open_dic = 0;
	return;
}

Static int
Sj3_Henkan(cnvrt,count,in_euc_str,in_euc_num,out_euc_str,out_euc_num)
uchar	cnvrt;
int	count;
ushort	*in_euc_str;
int	in_euc_num;
ushort	*out_euc_str;
int	out_euc_num;
{
	int	ret,i,len;
	uchar	in_sjis_buff[BUFSIZE],*p;
	uchar	out_sjis_buff[BUFSIZE];	
	static uchar	kanji[BUFSIZE];	
	static struct bunsetu bun[BUFSIZE];
	static struct douon dou[BUFSIZE];  
	static int bnum,koho_num; 


	for(i=0;i<BUFSIZE;i++)out_sjis_buff[i]=0x0;
	ret=EucToShiftJis(in_euc_str,in_euc_num,in_sjis_buff,BUFSIZE);

	if(count == 0){
		if( _dic_old_buff != 0) {
			Sj3_HenkanCancel();
		}
	in_sjis_buff[ret]=0x0;
        bnum=sj3_getkan(in_sjis_buff,bun,kanji,BUFSIZE);
	
        p  = bun[bnum-1].deststr;
        p += bun[bnum-1].destlen;
        *p = '\0';
 
        p  = bun[0].srcstr;
        p += bun[0].srclen;
        *p = '\0';

	 koho_num = sj3_getdouon(bun[0].srcstr,dou);
		ret=ShiftJisToEuc(kanji,strlen(kanji),
						out_euc_str,out_euc_num);
		_dic_old_buff = in_euc_str;
				return ret;
	} else{

		if(_dic_old_buff != in_euc_str) {
			Sj3_HenkanCancel();
			_dic_old_buff = in_euc_str;
			bun_max = 0;
		}
			strncpy(out_sjis_buff,dou[count%koho_num].ddata,dou[count%koho_num].dlen);
			if(bun[1].deststr){
			strcpy(out_sjis_buff+dou[count%koho_num].dlen,bun[1].deststr);	
			len=strlen(bun[1].deststr);
			}else{len=0;}

		ret=ShiftJisToEuc(out_sjis_buff,
					dou[count%koho_num].dlen+len,
					out_euc_str,out_euc_num);
	return ret;
	}
}

Static int
Sj3_Zenkouho(type,euc_str,ecu_num)
int 	type;
ushort	*euc_str;
int	ecu_num;
{
	return -1;
}

Static int
Sj3_HenkanCancel()
{
	if(_dic_old_buff != (ushort *)0){
		_dic_old_buff=(ushort *)0;
	}
}

#endif /*SJ3_DICTIONARY*/
