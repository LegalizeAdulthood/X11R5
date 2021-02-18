/* @(#)slcetoa.c	2.2 91/07/05 16:07:19 FUJITSU LIMITED. */
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

  Author: Takashi Fujiwara     FUJITSU LIMITED 
                               fujiwara@a80.tech.yk.fujitsu.co.jp
          Kiyoaki oya          FUJITSU LIMITED

******************************************************************/

#ifndef lint
static char sccs_id[] = "@(#)slcetoa.c	2.2 91/07/05 16:07:19 FUJITSU LIMITED.";
#endif

/*  slcetoa.c  : 1986.07.24 */

#include <sys/types.h>
#include "slcode.h"

#define SLC_NF     0xff
#define SLCNF      0xffff

/*
 * 0byte is JAPANESE EBCDIC KANA
 * 1byte is JAPANESE EBCDIC ENGLISH
 * 2byte is USASCII
 */

extern unsigned char slceatb[][3];

int slcetoa(incode, ebctype)
     register unsigned char incode;
     register int ebctype;
     {
     register int type;
     register unsigned char (*ea)[3] = slceatb;

     if(ebctype == C_USASCI)
          type = 2;
     else if(ebctype == C_JPNKAT)
          type = 0;
     else
          type = 1;
     if(ea[(unsigned)incode & 0377][type] == SLC_NF)
          return(-1);
     else
          return(ea[(unsigned)incode & 0377][type] & 0xff);
     }
