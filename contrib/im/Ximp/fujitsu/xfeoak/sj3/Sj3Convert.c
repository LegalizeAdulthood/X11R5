/* @(#)Sj3Convert.c	2.1 91/07/03 10:34:32 FUJITSU LIMITED. */
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

  Author: Takanori Tateno      FUJITSU LIMITED

******************************************************************/

#ifndef lint
static char sccs_id[] = "@(#)Sj3Convert.c	2.1 91/07/03 10:34:32 FUJITSU LIMITED.";
#endif

typedef unsigned short    ushort;
typedef unsigned char     uchar;
#include <stdio.h>

ushort jis_to_sjis();
ushort sjis_to_jis();

ShiftJisToEuc(sjis_str,sjis_size,euc_str,euc_num)
uchar	*sjis_str;
int	sjis_size;
ushort	*euc_str;
int	euc_num;
{
	int i,j;
	ushort	tmp;
	i=j=0;

	while(euc_num>i && sjis_size >j){
		if( (sjis_str[j]>=0x81 && sjis_str[j]<=0x9f) ||
			(sjis_str[j]>=0xe0 && sjis_str[j]<=0xfc)){
				tmp=sjis_to_jis(
					((((ushort)(sjis_str[j]))<<8) & 0xff00)|
					((ushort)(sjis_str[j+1]) & 0x00ff)
			  			);
				tmp=tmp | 0x8080;
				euc_str[i]=tmp;
				j+=2;
				i++;
		}else{
				euc_str[i]=((ushort)(sjis_str[j])) & 0x00ff ;
				i++;
				j++;
		}
	}
	return(i);
}

int
EucToShiftJis(euc_str,euc_num,sjis_str,sjis_size)
ushort	*euc_str;
int	euc_num;
uchar	*sjis_str;
int	sjis_size;
{
	int i,j=0;
	ushort	tmp;
	for(i=0;i<euc_num ;i++){
		if(euc_str[i]<256){
			if(j<sjis_size){
				sjis_str[j]=(uchar)euc_str[i];
				j++;
			}else{
				return(j);
			}
		}else{
			if(j+1 < sjis_size){
				tmp = euc_str[i] & 0x7f7f;
				tmp=jis_to_sjis(tmp);
				sjis_str[j]=(uchar)(tmp>>8);
				sjis_str[j+1]=(uchar)(tmp & 0xff);
				j+=2;
			}else{
				return(j);
			}
		}
	}
	return(j);
}

ushort
jis_to_sjis(code)
ushort	code;
{
	unsigned short	a,b;
	a=(code>>8) & 0xff;
	b=code & 0xff;
	if(a<0x21 || a>0x7e && a<0x21 || b> 0x7e ){
		return(-1);
	}
	a++;
	b+=(a & 0x1 ? 0x7e : 0x20);
	if(b < 0x80)--b;
	a=a >> 1;
	a+=0x70;
	if(a>=0x9f)a+=0x40;
	
	return(((a<<8) & 0xff00)|b);
}

ushort
sjis_to_jis(code)
ushort	code;
{
	unsigned short	a,b;
	a=(code>>8) & 0xff;
	b=code & 0xff;
	if(a<0x81 || (a>0x9f && a<0xe0) || a> 0xfc ||
		b < 0x40 || b== 0x7f || b>0xfc){
		return(-1);
	}
	a-=(a>=0xe0 ? 0xb0 : 0x70);
	a =a << 1;
	if(b<0x80)b++;
	if(b>=0x9f){
		a|=0x01;
		b-=0x7e;
	}else{
		b-= 0x20;
	}
	--a;
	return(((a<<8) & 0xff00)|b);
}
