/* @(#)slcode.h	2.1 91/07/03 12:01:24 FUJITSU LIMITED. */
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

/*  slcode.h  :  1986.12.02  */

#ifndef slcode
#define slcode
#include "Httawscode.h"

#ifndef NULL
#define NULL        0
#endif
#define SS2         0x8e
#define SS3         0x8f
#define SHF1        0x28
#define SHF2        0x29

#define SLC_NF      0xff
#define SLCNF       0xffff
#define SLCFOREVER  ;;
#define SLCOFF      0
#define SLCON       1
#define SLCFALSE    0
#define SLCTRUE     1
#define SLCMAXBUF   1000

#define SLCKATAKANA 0
#define SLCHIRAGANA 1

#define SLCNTYPE    0
#define SLCHTYPE    1

#define SLCXNOCNV   0
#define SLCXCNV     2

#define SLCNOREFENV 0
#define SLCREFENV   4

#define SLCBARNO    0
#define SLCBARYES   1

#define slkana(a)   ((a) >= 0x80 && (a) <= 0xff)

#define sljgaiji(a) (*(a) < 0xa1 || *(a) > 0xfe || *(a + 1) < 0xa1 ||\
         *(a + 1) > 0xfe || (*(a) * 256 + *(a + 1) == 0x4040))

#define slugaiji(a) (*(a) < 0xa1 || *(a) > 0xfe || *(a + 1) < 0xa1 ||\
                    *(a + 1) > 0xfe)
#endif
