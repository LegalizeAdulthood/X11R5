/* @(#)awscode.h	2.2 91/07/05 16:06:42 FUJITSU LIMITED. */
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

/*
 * Definitions about Code-Types / 1985 Jun. 7
 */
 
/* (1) Definitions about Code-Types Composed of 1-byte-Unit */
#define C_NONE1   0x0000    /* Code-Type is Undefined    */
#define C_ASCII   0x0100    /* (7-bit)ASCII              */
#define C_JIS8    0x0200    /* JIS8                      */
#define C_UXJIS   0x0300    /* UNIX JIS                  */
#define C_USASCI  0x0400    /* U.S.ASCII                 */
#define C_JPNKAT  0x0500    /* EBCDIC(Japanese Katakana) */
#define C_JPNENG  0x0600    /* EBCDIC(Japanese English)  */
#define C_EBCDEF  0x0700    
 
/* (2) Definitions about Code-Types Composed of 2-byte-Unit */
#define C_NONE2   0x0000    /* Code-Type is Undefined    */
#define C_SHFJIS  0x0001    /* Shift JIS                 */
#define C_JEF     0x0002    /* JEF                       */
#define C_JIS     0x0003    /* JIS                       */
 
/* (3) Check Slash ('/' in Shift-JIS Code) or Not */
#define SFTH      0x81      /* Slash('/') Code in Shift-JIS */
#define SL_SO     0x0e      
#define SL_SI     0x0f      
#define SL_SHF1   0x28      
#define SL_SHF2   0x29      
#define SL_SS2    0x8e      
#define SL_SS3    0x8f      
#define SL_SFTH   0x81      
#define SL_SFTL   0x5e      
 
#define SFTJIS(sft) \
        (*sft++ == SFTH ? \
          (*sft == SFTL ? (u->u_dirp = ++sft, 1) : 0) : 0)
