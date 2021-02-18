/*
 * $Id: sisheng.c,v 1.2 1991/09/16 21:32:03 ohm Exp $
 */
/** 		Copyright OMRON Corporation. 1989, 1990, 1991
 *
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose is hereby granted without fee, provided 
 * that all of the following conditions are satisfied:
 *
 * <1>	The above copyright notices appear in all copies
 * <2>	Both the copyright notices and this permission notice appear in 
 *	supporting documentation
 * <3>	The name of "cWnn" isn't changed unless substantial modifications 
 * 	are made
 * <4>	Following words followed by the above copyright notices appear
 *    	in all supporting documentation of software based on "cWnn":
 *
 * 	This software is based on the original version of cWnn developed 
 *	by OMRON Corporation and Wnn developed by Kyoto University Research 
 *	Institute for Mathematical Sciences (KURIMS), OMRON Corporation and 
 * 	ASTEC Inc."
 *
 * <5>	The names of OMRON may not be used in advertising or publicity 
 *	pertaining to distribution of the software without specific, written 
 *	prior permission
 *
 *  OMRON Corporation makes no representations about the suitability of this 
 *  software for any purpose.  It is provided "as is" without express or 
 *  implied warranty.
 *
 *  Wnn consortium is one of distributors of the official Wnn source code
 *  release.  Wnn consortium also makes no representations about the
 *  suitability of this software for any purpose.  It is provided "as is"
 *  without express or implied warranty.
 *
 *  OMRON AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 *  SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 *  IN NO EVENT SHALL OMRON OR WNN CONSORTIUM BE LIABLE FOR ANY SPECIAL,
 *  INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 *  LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 *  OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 *  PERFORMANCE OF THIS SOFTWARE.
 *
 *  This sofeware is developed by OMRON Corporation, based on the original 
 *  version of Wnn which was developed by Kyoto University Research Institute 
 *  for Mathematical Sciences (KURIMS), OMRON Corporation and ASTEC Inc. 
 *
 *  	Copyright Kyoto University Research Institute for Mathematical 
 *		Sciences 1987,1988,1989,1990,1991
 * 	Copyright OMRON Corporation 1987,1988,1989,1990,1991
 *	Copyright ASTEC Inc. 1987, 1988,1989,1990,1991
 *
 *  Contact:	Tel:   	+81-75-951-5111
 *		Fax:   	+81-75-955-2442
 *		E-mail: zhong@ari.ncl.omron.co.jp
**/
/**  cWnn  Version 1.1	 **/

#ifdef CHINESE

#include <stdio.h>
#include "commonhd.h"
#include "cplib.h"

#define isfuyuanyin(c)  ((c=='m')||(c=='n')||(c=='g'))
#define isyuanyin(c)  \
	((c=='a')||(c=='e')||(c=='i')||(c=='o')||(c=='u')||(c=='v'))

/* yincod_flt_sisheng(): filter sisheng of Yincode strings */

static int 
yincod_flt_sisheng(yincod,si,siyincod)
w_char *yincod;				/* Yincode without sisheng */
char *si;				/* sisheng string */
w_char *siyincod;			/* Yincode with sisheng */
{
  register int pan_count = 0;
	for ( ; *siyincod; siyincod++ )  {
		if ( _cwnn_isyincod_d(*siyincod) )  {
			*si++ = (char)(_cwnn_sisheng(*siyincod) + 0x30);
			*yincod++ = _cwnn_yincod_0(*siyincod);
			} else {
			*si++ = '5';
			*yincod++ = *siyincod;
			}
			if (pan_count++ == 5)   *(si-1) = 0;
		}
	*yincod = 0;
	*si = 0;
	return(strlen(si));
}

int get_sisheng(yomi,si,yomi_tmp)
w_char  *yomi;
w_char  *yomi_tmp;
char    *si;
{
  int length;

	length = yincod_flt_sisheng(yomi_tmp,si,yomi);
        return(length);
}

w_char *biki_sisheng(yomi,si,yomi_tmp)
w_char  *yomi;
w_char  *yomi_tmp;
char    *si;
{
  int length;

	length = yincod_flt_sisheng(yomi_tmp,si,yomi);
        return(yomi_tmp);
}

unsigned diff_sisheng(si1, si2)    /* PAN deleted by pan shilei */
int si1;
int si2;
{
  register int length;
  register unsigned int reslt;
  char s1[7];
  char s2[7];

	sprintf(s1,"%d",si1);
	sprintf(s2,"%d",si2);
	length = strlen(s1) - 1;
	for (reslt = 1; length >= 0; length--)
		if(s1[length] == s2[length])
			reslt = reslt*(10 - length);
        return(reslt);
}

#endif /* CHINESE */
