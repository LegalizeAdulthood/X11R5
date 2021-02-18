/*
 *        %Z%%M% %I% %E%  
 * 
 * Derived from sxc functions of FUJITSU LIMITED. */
/*
****************************************************************************

            Copyright 1991, by FUJITSU LIMITED
            Copyright 1991, by Sun Microsystems, Inc.

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of FUJITSU LIMITED or Sun
Microsystems, Inc.  not be used in advertising or publicity pertaining
to distribution of the software without specific, written prior
permission.  FUJITSU LIMITED and Sun Microsystems, Inc. make no
representations about the suitability of this software for any
purpose.  It is provided "as is" without express or implied warranty.

FUJITSU LIMITED AND SUN MICROSYSTEMS, INC. DISCLAIMS ALL WARRANTIES
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU LIMITED AND SUN
MICROSYSTEMS, INC. BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

  Author: Takashi Fujiwara     FUJITSU LIMITED 
                               fujiwara@a80.tech.yk.fujitsu.co.jp
          Kiyoaki oya          FUJITSU LIMITED
          Hideki Hiura (hhiura@Sun.COM) 	Sun Microsystems, Inc.

****************************************************************************
*/
#ifndef slcode
#define slcode
#include <sys/types.h>

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

/*  slcode.h  :  1986.12.02  */

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

/*  slcrtok.h  :  1986.09.03  */

#define  NO_FOUND    0
#define  NOCHAR      0
#define  UNCHANGED  -1
#define  NO          0
#define  ERRSLCD    -1

#define  SLCD_A      0xc1
#define  SLCD_B      0xc2
#define  SLCD_C      0xc3
#define  SLCD_D      0xc4
#define  SLCD_E      0xc5
#define  SLCD_F      0xc6
#define  SLCD_G      0xc7
#define  SLCD_H      0xc8
#define  SLCD_I      0xc9
#define  SLCD_J      0xd1
#define  SLCD_K      0xd2
#define  SLCD_L      0xd3
#define  SLCD_M      0xd4
#define  SLCD_N      0xd5
#define  SLCD_O      0xd6
#define  SLCD_P      0xd7
#define  SLCD_Q      0xd8
#define  SLCD_R      0xd9
#define  SLCD_S      0xe2
#define  SLCD_T      0xe3
#define  SLCD_U      0xe4
#define  SLCD_V      0xe5
#define  SLCD_W      0xe6
#define  SLCD_X      0xe7
#define  SLCD_Y      0xe8
#define  SLCD_Z      0xe9
#define  SLCD_BAR    0x60
#define  SLCD_APOST  0x7d
#define  SLCD_CHOU   0x58

/*  slcrtok.c  : 1986.04.22 */

#define  SLCD_A        0xc1
#define  SLCD_B        0xc2
#define  SLCD_C        0xc3
#define  SLCD_D        0xc4
#define  SLCD_E        0xc5
#define  SLCD_F        0xc6
#define  SLCD_G        0xc7
#define  SLCD_H        0xc8
#define  SLCD_I        0xc9
#define  SLCD_J        0xd1
#define  SLCD_K        0xd2
#define  SLCD_L        0xd3
#define  SLCD_M        0xd4
#define  SLCD_N        0xd5
#define  SLCD_O        0xd6
#define  SLCD_P        0xd7
#define  SLCD_Q        0xd8
#define  SLCD_R        0xd9
#define  SLCD_S        0xe2
#define  SLCD_T        0xe3
#define  SLCD_U        0xe4
#define  SLCD_V        0xe5
#define  SLCD_W        0xe6
#define  SLCD_X        0xe7                               
#define  SLCD_Y        0xe8
#define  SLCD_Z        0xe9
#define  SLCD_BAR      0x60
#define  SLCD_APOST    0x7d
#define  SLCD_CHOU     0x58                               

extern unsigned char slcrkboin[5];
extern unsigned char slcrksiin[20];

extern unsigned char slcrkk1[][2];
extern unsigned char slcrkk2[][4];
extern unsigned char slcrkk3[][2];

extern unsigned char slcrkt1[][4];
extern unsigned char slcrkt2[][3];
extern unsigned char slcrkt3[][2];

static int  onemoji(), twomoji(), threemoji(), chk_boin(), chk_siin();
static int  chk_hswy(), outchar();
static void goaheadinbuf();

int slcrtok(incode, inbuf, outbuf, nhtype)
     register unsigned char incode;
     register unsigned char *inbuf, *outbuf;
     register int  nhtype;
     {
     register int counter = cstrlen(inbuf);
     register int i;

     for(i = 0; i < 4; i++)
          *(outbuf + i) = NULL;
     if(nhtype & SLCXCNV && incode == SLCD_CHOU)          
          incode = SLCD_CHOU;                             
     inbuf[counter++] = incode;
     switch(counter)
          {
          case  1 : return(onemoji(inbuf, outbuf));
          case  2 : return(twomoji(inbuf, outbuf, nhtype));
          default : return(threemoji(inbuf, outbuf));
          }
     }

static int onemoji(inbuf,outbuf)
     register unsigned char *inbuf, *outbuf;
     {
     register int i, k;
     register unsigned char c;
     register unsigned char (*rkk1)[2] = slcrkk1;

     if((i = chk_boin(c = *inbuf)) != NO_FOUND)
          {
          k = outchar(1, rkk1[i - 1], outbuf);
          goaheadinbuf(1, inbuf);
          return(k);
          }
     if(chk_siin(c) != NO_FOUND)
          return(NOCHAR);
     else {
          outchar(1, inbuf, outbuf);
          goaheadinbuf(1, inbuf);
          return(UNCHANGED);
          }
     }

static int twomoji(inbuf, outbuf, nhtype)
     register unsigned char *inbuf, *outbuf;
     register int nhtype;
     {
     register int i, j, k, l;
     register unsigned char c, c1;
     unsigned char tempbuf[5];
     register unsigned char (*rkk2)[4] = slcrkk2,
                            (*rkk3)[2] = slcrkk3;
     static unsigned char chou[2] = {SLCD_CHOU, NULL};  

     j = chk_siin(c = *inbuf);
     if((i = chk_boin(c1 = *(inbuf + 1))) != NO_FOUND)
          {
          l = (j - 1) * 5 + (i - 1);
          k = outchar(0, rkk2[l], outbuf);
          goaheadinbuf(2, inbuf);
          return(k);
          }
     if(c == SLCD_N)
          {
          if(c1 == SLCD_APOST ||
             (c1 == SLCD_N && !(nhtype & SLCHTYPE)) ||    
             (c1 == SLCD_BAR && nhtype & SLCHTYPE))       
               {
               k = outchar(1, rkk3[0], outbuf);
               goaheadinbuf(2, inbuf);
               return(k);
               }
          else if(c1 == SLCD_N && nhtype & SLCHTYPE)      
               {
               k = outchar(1, rkk3[0], outbuf);
               goaheadinbuf(1, inbuf);
               return(k);
               }
          else if(c1 == SLCD_BAR && !(nhtype & SLCHTYPE)) 
               {
               outchar(0, rkk3[0], tempbuf);
               cstrcat(tempbuf, chou);
               k = outchar(0, tempbuf, outbuf);
               goaheadinbuf(2, inbuf);
               return(k);
               }
          else if(c1 != SLCD_Y && chk_siin(c1) != NO_FOUND)
               {
               k = outchar(1, rkk3[0], outbuf);
               goaheadinbuf(1, inbuf);
               return(k);
               }
          else if(chk_siin(c1) == NO_FOUND)
               {   
               *inbuf = rkk3[0][0];
               k = outchar(2, inbuf, outbuf);
               goaheadinbuf(2, inbuf);
               return(k);
               }
          }
     if(j != NO_FOUND && c == c1)
          {
          k = outchar(1, rkk3[1], outbuf);
          goaheadinbuf(1, inbuf);
          return(k);
          }
     if(chk_hswy(c, c1) != NO_FOUND)
          return(NOCHAR);
     else
          {
          if(chk_siin(c1) != NO_FOUND)
               {
               k = outchar(1, inbuf, outbuf);
               goaheadinbuf(1, inbuf);
               }
          else
               {
               k = outchar(2, inbuf, outbuf);
               goaheadinbuf(2, inbuf);
               }
          return(k);
          }
     }

static int threemoji(inbuf, outbuf)
     register unsigned char *inbuf, *outbuf;
     {
     register int i, j, k;
     register unsigned char (*rkt1)[4] = slcrkt1,
                            (*rkt2)[3] = slcrkt2,
                            (*rkt3)[2] = slcrkt3,
                            (*rkk3)[2] = slcrkk3;
     unsigned char tempbuf[5];

     if((i = chk_hswy(inbuf[0], inbuf[1])) >= 25)
          return(twudwu(inbuf, outbuf, i));
     if((j = chk_boin(inbuf[2])) != NO_FOUND)
          {
          if((i = chk_hswy(inbuf[0], inbuf[1])) <= 7)
               k = outchar(0,
               rkt1[(i - 1) * 5 + (j - 1)], outbuf);
          else
               {
               outchar(0, rkt2[i - 8], tempbuf);
               cstrcat(tempbuf, rkt3[j - 1]);
               k = outchar(0, tempbuf, outbuf);
               }
          goaheadinbuf(3, inbuf);
          return(k);
          }
     else if(chk_siin(inbuf[2]) != NO_FOUND)
          { 
          if(inbuf[1] == inbuf[2])
               {
               outchar(1, &inbuf[0], tempbuf);
               cstrcat(tempbuf, rkk3[1]);
               k = outchar(0, tempbuf, outbuf);
               goaheadinbuf(2, inbuf);
               }
          else if(chk_hswy(inbuf[1], inbuf[2]) != NO_FOUND)
               {
               k = outchar(1, inbuf, outbuf);
               goaheadinbuf(1, inbuf);
               } 
          else
               {
               k = outchar(2, inbuf, outbuf);
               goaheadinbuf(2, inbuf);
               }
          }
     else
          {
          k = outchar(3, inbuf, outbuf);
          goaheadinbuf(3, inbuf);
          }
     return(k);
     }

static int twudwu(inbuf, outbuf, i)
     register unsigned char *inbuf, *outbuf;
     register int i;
     {
     static unsigned char twu[3] = {0x95, 0x49, NULL},
                          dwu[4] = {0x95, 0xbe, 0x49, NULL},
                            T[2] = {0xe3, NULL},
                            D[2] = {0xc4, NULL}; 

     register unsigned char (*rkk2)[4] = slcrkk2,
                            (*rkk3)[2] = slcrkk3;

     register int k, l, j = -1;
     unsigned char tempbuf[5]; 

     if((j = chk_boin(inbuf[2])) == 3)
          {
          k = outchar(0, (i == 25) ? twu : dwu, outbuf);
          goaheadinbuf(3, inbuf);
          return(k);
          }
     else if(j >= 1)
          { 
          outchar(0, (i == 25)? T : D, tempbuf);
          l = 17 * 5 + (j - 1);
          cstrcat(tempbuf, rkk2[l]);
          k = outchar(0, tempbuf, outbuf);
          goaheadinbuf(3, inbuf);
          return(k);
          }
     else if(chk_siin(inbuf[2]) != NO_FOUND)
          {  
          if(inbuf[1] == inbuf[2])
               {
               outchar(1, &inbuf[0], tempbuf);
               cstrcat(tempbuf, rkk3[1]);
               k = outchar(0, tempbuf, outbuf);
               goaheadinbuf(2, inbuf);
               return(k);
               }
          else
               {                                   
               k = outchar(2, inbuf, outbuf);
               goaheadinbuf(2, inbuf);
               return(k);
               }
          }
     }

static int chk_boin(c)
     register unsigned char c;
     {
     register int i;

     for(i = 0; i < 5; i++)
          if(slcrkboin[i] == c)
               return(i + 1);
     return(NO_FOUND);
     }

static int chk_siin(c)
     register unsigned char c;
     {
     register int i;

     for(i = 0; i < 20; i++)
          if(slcrksiin[i] == c)
               return(i + 1);
     return(NO_FOUND);
     }

static int chk_hswy(c, c1)
     register unsigned char c, c1;
     {

     switch(c1)
          {
     case SLCD_H :
          switch( c )
               {
          case SLCD_C :  return(1);
          case SLCD_D :  return(2);
          case SLCD_S :  return(3);
          case SLCD_T :  return(4);
          default     :  return(NO_FOUND);
               }
     case SLCD_S :
          if(c == SLCD_T)
               return(5);
          else
               return(NO_FOUND);
     case SLCD_W :
          if(c == SLCD_G)
               return(6);
          else if(c == SLCD_K)
               return(7);
          else if(c == SLCD_T)
               return(25);
          else if(c == SLCD_D)
               return(26);
          else
               return(NO_FOUND);
     case SLCD_Y :
          switch( c )
               {
          case SLCD_B :  return(8);
          case SLCD_C :  return(9);
          case SLCD_D :  return(10);
          case SLCD_F :  return(11);
          case SLCD_G :  return(12);
          case SLCD_H :  return(13);
          case SLCD_J :  return(14);
          case SLCD_K :  return(15);
          case SLCD_L :  return(16);
          case SLCD_M :  return(17);
          case SLCD_N :  return(18);
          case SLCD_P :  return(19);
          case SLCD_Q :  return(20);
          case SLCD_R :  return(21);
          case SLCD_S :  return(22);
          case SLCD_T :  return(23);
          case SLCD_Z :  return(24);
          default     :  return(NO_FOUND);
               }
     default:
          return(NO_FOUND);
          }
     }

static void goaheadinbuf(n, s)
     register int  n;
     register unsigned char *s;
     {
     register int i;

     for(i = 1; i <= n; i++)
          {
          s[0] = s[1];
          s[1] = s[2];
          s[2] = NULL;
          }
     }

static int outchar(n, s, t)
     register int  n;
     register unsigned char *s, *t;
     {
     register int i;

     for(i = 0; n == 0 && *s != NULL || n != 0 && i < n ; s++,t++,i++ )
          *t = *s;
     *t = NULL;
     return(i);
     }

static int cstrlen(s)
     register char *s;
     {
     register int i;

     for(i = 0; *s != NULL; i++, s++)
          ;
     return(i);
     }

static cstrcat(s, t)
     register char *s, *t;
     {
     register int i, j;
     i = j = 0;
     while(s[i] != NULL)
          i++;
     while((s[i++] = t[j++]) != NULL)
          ;
     }

/*  slctab.c  : 1986.07.16  */

#define SLC_NF     0xff
#define SLCNF      0xffff

/*
 * 0byte is JAPANESE EBCDIC KANA
 * 1byte is JAPANESE EBCDIC ENGLISH
 * 2byte is USASCII
 */

unsigned char slcaetb[][3] = {
     {0x00, 0x00, 0x00},           /* 00 (NUL)*/
     {0x01, 0x01, 0x01},           /* 01 (SOH)*/
     {0x02, 0x02, 0x02},           /* 02 (STX)*/
     {0x03, 0x03, 0x03},           /* 03 (EXT)*/
     {0x37, 0x37, 0x37},           /* 04 (EOT)*/
     {0x2d, 0x2d, 0x2d},           /* 05 (ENQ)*/
     {0x2e, 0x2e, 0x2e},           /* 06 (ACK)*/
     {0x2f, 0x2f, 0x2f},           /* 07 (BEL)*/
     {0x16, 0x16, 0x16},           /* 08 (Back Space)*/
     {0x05, 0x05, 0x05},           /* 09 (Horizontal Tab)*/
     {0x15, 0x15, 0x15},           /* 0a (LF)*/
     {0x0b, 0x0b, 0x0b},           /* 0b (VT)*/
     {0x0c, 0x0c, 0x0c},           /* 0c (FF)*/
     {0x0d, 0x0d, 0x0d},           /* 0d (Carriage Return)*/
     {0x0e, 0x0e, 0x0e},           /* 0e (SO)*/
     {0x0f, 0x0f, 0x0f},           /* 0f (SI)*/
     {0x10, 0x10, 0x10},           /* 10 (DLE)*/
     {0x11, 0x11, 0x11},           /* 11 (DC1)*/
     {0x12, 0x12, 0x12},           /* 12 (DC2)*/
     {0x13, 0x13, 0x13},           /* 13 (DC3)*/
     {0x3c, 0x3c, 0x3c},           /* 14 (DC4)*/
     {0x3d, 0x3d, 0x3d},           /* 15 (NAK)*/
     {0x32, 0x32, 0x32},           /* 16 (SYN)*/
     {0x26, 0x26, 0x26},           /* 17 (ETB)*/
     {0x18, 0x18, 0x18},           /* 18 (CAN)*/
     {0x19, 0x19, 0x19},           /* 19 (EM)*/
     {0x3f, 0x3f, 0x3f},           /* 1a (SUB)*/
     {0x27, 0x27, 0x27},           /* 1b (ESC)*/
     {0x1c, 0x1c, 0x1c},           /* 1c (Cursor Right)*/
     {0x1d, 0x1d, 0x1d},           /* 1d (Cursor Left)*/
     {0x1e, 0x1e, 0x1e},           /* 1e (Cursor Up)*/
     {0x1f, 0x1f, 0x1f},           /* 1f (Cursor Down)*/
     {0x40, 0x40, 0x40},           /* 20 (SP)*/
     {0x4f, 0x4f, 0x4f},           /* 21  ! */ 
     {0x7f, 0x7f, 0x7f},           /* 22  " */
     {0x7b, 0x7b, 0x7b},           /* 23  # */
     {0xe0, 0x5b, 0x5b},           /* 24  $ */
     {0x6c, 0x6c, 0x6c},           /* 25  % */
     {0x50, 0x50, 0x50},           /* 26  & */
     {0x7d, 0x7d, 0x7d},           /* 27  ' */
     {0x4d, 0x4d, 0x4d},           /* 28  ( */
     {0x5d, 0x5d, 0x5d},           /* 29  ) */
     {0x5c, 0x5c, 0x5c},           /* 2a  * */
     {0x4e, 0x4e, 0x4e},           /* 2b  + */
     {0x6b, 0x6b, 0x6b},           /* 2c  , */
     {0x60, 0x60, 0x60},           /* 2d  - */
     {0x4b, 0x4b, 0x4b},           /* 2e  . */
     {0x61, 0x61, 0x61},           /* 2f  / */
     {0xf0, 0xf0, 0xf0},           /* 30  0 */
     {0xf1, 0xf1, 0xf1},           /* 31  1 */
     {0xf2, 0xf2, 0xf2},           /* 32  2 */
     {0xf3, 0xf3, 0xf3},           /* 33  3 */
     {0xf4, 0xf4, 0xf4},           /* 34  4 */
     {0xf5, 0xf5, 0xf5},           /* 35  5 */
     {0xf6, 0xf6, 0xf6},           /* 36  6 */
     {0xf7, 0xf7, 0xf7},           /* 37  7 */
     {0xf8, 0xf8, 0xf8},           /* 38  8 */
     {0xf9, 0xf9, 0xf9},           /* 39  9 */
     {0x7a, 0x7a, 0x7a},           /* 3a  : */
     {0x5e, 0x5e, 0x5e},           /* 3b  ; */
     {0x4c, 0x4c, 0x4c},           /* 3c  < */
     {0x7e, 0x7e, 0x7e},           /* 3d  = */
     {0x6e, 0x6e, 0x6e},           /* 3e  > */
     {0x6f, 0x6f, 0x6f},           /* 3f  ? */
     {0x7c, 0x7c, 0x7c},           /* 40  @ */
     {0xc1, 0xc1, 0xc1},           /* 41  A */
     {0xc2, 0xc2, 0xc2},           /* 42  B */
     {0xc3, 0xc3, 0xc3},           /* 43  C */
     {0xc4, 0xc4, 0xc4},           /* 44  D */
     {0xc5, 0xc5, 0xc5},           /* 45  E */
     {0xc6, 0xc6, 0xc6},           /* 46  F */
     {0xc7, 0xc7, 0xc7},           /* 47  G */
     {0xc8, 0xc8, 0xc8},           /* 48  H */
     {0xc9, 0xc9, 0xc9},           /* 49  I */
     {0xd1, 0xd1, 0xd1},           /* 4a  J */
     {0xd2, 0xd2, 0xd2},           /* 4b  K */
     {0xd3, 0xd3, 0xd3},           /* 4c  L */
     {0xd4, 0xd4, 0xd4},           /* 4d  M */
     {0xd5, 0xd5, 0xd5},           /* 4e  N */
     {0xd6, 0xd6, 0xd6},           /* 4f  O */
     {0xd7, 0xd7, 0xd7},           /* 50  P */
     {0xd8, 0xd8, 0xd8},           /* 51  Q */
     {0xd9, 0xd9, 0xd9},           /* 52  R */
     {0xe2, 0xe2, 0xe2},           /* 53  S */
     {0xe3, 0xe3, 0xe3},           /* 54  T */
     {0xe4, 0xe4, 0xe4},           /* 55  U */
     {0xe5, 0xe5, 0xe5},           /* 56  V */
     {0xe6, 0xe6, 0xe6},           /* 57  W */
     {0xe7, 0xe7, 0xe7},           /* 58  X */
     {0xe8, 0xe8, 0xe8},           /* 59  Y */
     {0xe9, 0xe9, 0xe9},           /* 5a  Z */
     {0x4a, 0x4a, 0x4a},           /* 5b  [ */
     {0x5b, 0xe0, 0xe0},           /* 5c  \ */
     {0x5a, 0x5a, 0x5a},           /* 5d  ] */
     {0x5f, 0x5f, 0x5f},           /* 5e  ^ */
     {0x6d, 0x6d, 0x6d},           /* 5f  _ */
     {0x79, 0x79, 0x79},           /* 60  ` */
     {SLC_NF, 0x81, 0x81},         /* 61  a */
     {SLC_NF, 0x82, 0x82},         /* 62  b */
     {SLC_NF, 0x83, 0x83},         /* 63  c */
     {SLC_NF, 0x84, 0x84},         /* 64  d */
     {SLC_NF, 0x85, 0x85},         /* 65  e */
     {SLC_NF, 0x86, 0x86},         /* 66  f */
     {SLC_NF, 0x87, 0x87},         /* 67  g */
     {SLC_NF, 0x88, 0x88},         /* 68  h */
     {SLC_NF, 0x89, 0x89},         /* 69  i */
     {SLC_NF, 0x91, 0x91},         /* 6a  j */
     {SLC_NF, 0x92, 0x92},         /* 6b  k */
     {SLC_NF, 0x93, 0x93},         /* 6c  l */
     {SLC_NF, 0x94, 0x94},         /* 6d  m */
     {SLC_NF, 0x95, 0x95},         /* 6e  n */
     {SLC_NF, 0x96, 0x96},         /* 6f  o */
     {SLC_NF, 0x97, 0x97},         /* 70  p */
     {SLC_NF, 0x98, 0x98},         /* 71  q */
     {SLC_NF, 0x99, 0x99},         /* 72  r */
     {SLC_NF, 0xa2, 0xa2},         /* 73  s */
     {SLC_NF, 0xa3, 0xa3},         /* 74  t */
     {SLC_NF, 0xa4, 0xa4},         /* 75  u */
     {SLC_NF, 0xa5, 0xa5},         /* 76  v */
     {SLC_NF, 0xa6, 0xa6},         /* 77  w */
     {SLC_NF, 0xa7, 0xa7},         /* 78  x */
     {SLC_NF, 0xa8, 0xa8},         /* 79  y */
     {SLC_NF, 0xa9, 0xa9},         /* 7a  z */
     {0xc0, 0xc0, 0xc0},           /* 7b  { */
     {0x6a, 0x6a, 0x6a},           /* 7c  | */
     {0xd0, 0xd0, 0xd0},           /* 7d  } */
     {0xa1, 0xa1, 0xa1},           /* 7e  ~ */
     {0x07, 0x07, 0x07},           /* 7f (DEL)*/
     {SLC_NF, SLC_NF, SLC_NF},     /* 80 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 81 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 82 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 83 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 84 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 85 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 86 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 87 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 88 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 89 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 8a */
     {SLC_NF, SLC_NF, SLC_NF},     /* 8b */
     {SLC_NF, SLC_NF, SLC_NF},     /* 8c */
     {SLC_NF, SLC_NF, SLC_NF},     /* 8d */
     {SLC_NF, SLC_NF, SLC_NF},     /* 8e */
     {SLC_NF, SLC_NF, SLC_NF},     /* 8f */
     {SLC_NF, SLC_NF, SLC_NF},     /* 90 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 91 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 92 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 93 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 94 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 95 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 96 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 97 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 98 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 99 */
     {SLC_NF, SLC_NF, SLC_NF},     /* 9a */
     {SLC_NF, SLC_NF, SLC_NF},     /* 9b */
     {SLC_NF, SLC_NF, SLC_NF},     /* 9c */
     {SLC_NF, SLC_NF, SLC_NF},     /* 9d */
     {SLC_NF, SLC_NF, SLC_NF},     /* 9e */
     {SLC_NF, SLC_NF, SLC_NF},     /* 9f */
     {0x40, 0x40, 0x40},           /* a0 (SP)*/
     {0x41, SLC_NF, SLC_NF},       /* a1  */
     {0x42, SLC_NF, SLC_NF},       /* a2  */
     {0x43, SLC_NF, SLC_NF},       /* a3  */
     {0x44, SLC_NF, SLC_NF},       /* a4  */
     {0x45, SLC_NF, SLC_NF},       /* a5  */
     {0x46, SLC_NF, SLC_NF},       /* a6  */
     {0x47, SLC_NF, SLC_NF},       /* a7  */
     {0x48, SLC_NF, SLC_NF},       /* a8  */
     {0x49, SLC_NF, SLC_NF},       /* a9  */
     {0x51, SLC_NF, SLC_NF},       /* aa  */
     {0x52, SLC_NF, SLC_NF},       /* ab  */
     {0x53, SLC_NF, SLC_NF},       /* ac  */
     {0x54, SLC_NF, SLC_NF},       /* ad  */
     {0x55, SLC_NF, SLC_NF},       /* ae  */
     {0x56, SLC_NF, SLC_NF},       /* af  */
     {0x58, SLC_NF, SLC_NF},       /* b0  */
     {0x81, SLC_NF, SLC_NF},       /* b1  */
     {0x82, SLC_NF, SLC_NF},       /* b2  */
     {0x83, SLC_NF, SLC_NF},       /* b3  */
     {0x84, SLC_NF, SLC_NF},       /* b4  */
     {0x85, SLC_NF, SLC_NF},       /* b5  */
     {0x86, SLC_NF, SLC_NF},       /* b6  */
     {0x87, SLC_NF, SLC_NF},       /* b7  */
     {0x88, SLC_NF, SLC_NF},       /* b8  */
     {0x89, SLC_NF, SLC_NF},       /* b9  */
     {0x8a, SLC_NF, SLC_NF},       /* ba  */
     {0x8c, SLC_NF, SLC_NF},       /* bb  */
     {0x8d, SLC_NF, SLC_NF},       /* bc  */
     {0x8e, SLC_NF, SLC_NF},       /* bd  */
     {0x8f, SLC_NF, SLC_NF},       /* be  */
     {0x90, SLC_NF, SLC_NF},       /* bf  */
     {0x91, SLC_NF, SLC_NF},       /* c0  */
     {0x92, SLC_NF, SLC_NF},       /* c1  */
     {0x93, SLC_NF, SLC_NF},       /* c2  */
     {0x94, SLC_NF, SLC_NF},       /* c3  */
     {0x95, SLC_NF, SLC_NF},       /* c4  */
     {0x96, SLC_NF, SLC_NF},       /* c5  */
     {0x97, SLC_NF, SLC_NF},       /* c6  */
     {0x98, SLC_NF, SLC_NF},       /* c7  */
     {0x99, SLC_NF, SLC_NF},       /* c8  */
     {0x9a, SLC_NF, SLC_NF},       /* c9  */
     {0x9d, SLC_NF, SLC_NF},       /* ca  */
     {0x9e, SLC_NF, SLC_NF},       /* cb  */
     {0x9f, SLC_NF, SLC_NF},       /* cc  */
     {0xa2, SLC_NF, SLC_NF},       /* cd  */
     {0xa3, SLC_NF, SLC_NF},       /* ce  */
     {0xa4, SLC_NF, SLC_NF},       /* cf  */
     {0xa5, SLC_NF, SLC_NF},       /* d0  */
     {0xa6, SLC_NF, SLC_NF},       /* d1  */
     {0xa7, SLC_NF, SLC_NF},       /* d2  */
     {0xa8, SLC_NF, SLC_NF},       /* d3  */
     {0xa9, SLC_NF, SLC_NF},       /* d4  */
     {0xaa, SLC_NF, SLC_NF},       /* d5  */
     {0xac, SLC_NF, SLC_NF},       /* d6  */
     {0xad, SLC_NF, SLC_NF},       /* d7  */
     {0xae, SLC_NF, SLC_NF},       /* d8  */
     {0xaf, SLC_NF, SLC_NF},       /* d9  */
     {0xba, SLC_NF, SLC_NF},       /* da  */
     {0xbb, SLC_NF, SLC_NF},       /* db  */
     {0xbc, SLC_NF, SLC_NF},       /* dc  */
     {0xbd, SLC_NF, SLC_NF},       /* dd  */
     {0xbe, SLC_NF, SLC_NF},       /* de  */
     {0xbf, SLC_NF, SLC_NF},       /* df  */
     {SLC_NF, SLC_NF, SLC_NF},     /* e0 */
     {SLC_NF, SLC_NF, SLC_NF},     /* e1 */
     {SLC_NF, SLC_NF, SLC_NF},     /* e2 */
     {SLC_NF, SLC_NF, SLC_NF},     /* e3 */
     {SLC_NF, SLC_NF, SLC_NF},     /* e4 */
     {SLC_NF, SLC_NF, SLC_NF},     /* e5 */
     {SLC_NF, SLC_NF, SLC_NF},     /* e6 */
     {SLC_NF, SLC_NF, SLC_NF},     /* e7 */
     {SLC_NF, SLC_NF, SLC_NF},     /* e8 */
     {SLC_NF, SLC_NF, SLC_NF},     /* e9 */
     {SLC_NF, SLC_NF, SLC_NF},     /* ea */
     {SLC_NF, SLC_NF, SLC_NF},     /* eb */
     {SLC_NF, SLC_NF, SLC_NF},     /* ec */
     {SLC_NF, SLC_NF, SLC_NF},     /* ed */
     {SLC_NF, SLC_NF, SLC_NF},     /* ee */
     {SLC_NF, SLC_NF, SLC_NF},     /* ef */
     {SLC_NF, SLC_NF, SLC_NF},     /* f0 */
     {SLC_NF, SLC_NF, SLC_NF},     /* f1 */
     {SLC_NF, SLC_NF, SLC_NF},     /* f2 */
     {SLC_NF, SLC_NF, SLC_NF},     /* f3 */
     {SLC_NF, SLC_NF, SLC_NF},     /* f4 */
     {SLC_NF, SLC_NF, SLC_NF},     /* f5 */
     {SLC_NF, SLC_NF, SLC_NF},     /* f6 */
     {SLC_NF, SLC_NF, SLC_NF},     /* f7 */
     {SLC_NF, SLC_NF, SLC_NF},     /* f8 */
     {SLC_NF, SLC_NF, SLC_NF},     /* f9 */
     {SLC_NF, SLC_NF, SLC_NF},     /* fa */
     {SLC_NF, SLC_NF, SLC_NF},     /* fb */
     {SLC_NF, SLC_NF, SLC_NF},     /* fc */
     {SLC_NF, SLC_NF, SLC_NF},     /* fd */
     {SLC_NF, SLC_NF, SLC_NF},     /* fe */
     {0xff, 0xff, 0xff}            /* ff */
     };

unsigned char slceatb[][3] = {
     {0x00, 0x00, 0x00},        /* 00 (NUL)          */
     {0x01, 0x01, 0x01},        /* 01 (SOH)          */
     {0x02, 0x02, 0x02},        /* 02 (STX)          */
     {0x03, 0x03, 0x03},        /* 03 (EXT)          */
     {SLC_NF, SLC_NF, SLC_NF},  /* 04 */
     {0x09, 0x09, 0x09},        /* 05 (ENQ)          */
     {SLC_NF, SLC_NF, SLC_NF},  /* 06 */
     {0x7f, 0x7f, 0x7f},        /* 07 (BEL)          */
     {SLC_NF, SLC_NF, SLC_NF},  /* 08 */
     {0x09, 0x09, 0x09},        /* 09 (HT)           */
     {SLC_NF, SLC_NF, SLC_NF},  /* 0a */
     {0x0b, 0x0b, 0x0b},        /* 0b (VT)           */
     {0x0c, 0x0c, 0x0c},        /* 0c (FF)           */
     {0x0d, 0x0d, 0x0d},        /* 0d (CR)           */
     {0x0e, 0x0e, 0x0e},        /* 0e (SO)           */
     {0x0f, 0x0f, 0x0f},        /* 0f (SI)           */
     {0x10, 0x10, 0x10},        /* 10 (DEL)          */
     {0x11, 0x11, 0x11},        /* 11 (DC1)          */
     {0x12, 0x12, 0x12},        /* 12 (DC2)          */
     {0x13, 0x13, 0x13},        /* 13 (DC3)          */
     {SLC_NF, SLC_NF, SLC_NF},  /* 14 (DC4)          */
     {0x0a, 0x0a, 0x0a},        /* 15 (New Line)     */
     {0x08, 0x08, 0x08},        /* 16 (Back Space)   */
     {SLC_NF, SLC_NF, SLC_NF},  /* 17 */
     {0x18, 0x18, 0X18},        /* 18 (CAN)          */
     {0x19, 0x19, 0x19},        /* 19 (EM)           */
     {SLC_NF, SLC_NF, SLC_NF},  /* 1a                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 1b                */
     {0x1c, 0x1c, 0x1c},        /* 1c (Cursor Right) */
     {0x1d, 0x1d, 0x1d},        /* 1d (Cursor Left)  */
     {0x1e, 0x1e, 0x1e},        /* 1e (Cursor Up)    */
     {0x1f, 0x1f, 0x1f},        /* 1f (Cursor Down)  */
     {SLC_NF, SLC_NF, SLC_NF},  /* 20                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 21                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 22                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 23                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 24                */
     {0x0a, 0x0a, 0x0a},        /* 25 (LF)           */
     {0x17, 0x17, 0x17},        /* 26 (ETB)          */
     {0x1b, 0x1b, 0x1b},        /* 27 (ESC)          */
     {SLC_NF, SLC_NF, SLC_NF},  /* 28                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 29                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 2a                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 2b                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 2c                */
     {0x05, 0x05, 0x05},        /* 2d (ENQ)          */
     {0x06, 0x06, 0x06},        /* 2e (ACK)          */
     {0x07, 0x07, 0x07},        /* 2f (BEL)          */
     {SLC_NF, SLC_NF, SLC_NF},  /* 30                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 31                */
     {0x16, 0x16, 0x16},        /* 32 (SYN)          */
     {SLC_NF, SLC_NF, SLC_NF},  /* 33                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 34                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 35                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 36                */
     {0x04, 0x04, 0x04},        /* 37 (EOT)          */
     {SLC_NF, SLC_NF, SLC_NF},  /* 38                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 39                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 3a                */
     {SLC_NF, SLC_NF, SLC_NF},  /* 3b                */
     {0x14, 0x14, 0x14},        /* 3c (DC4)          */
     {0x15, 0x15, 0x15},        /* 3d (NAK)          */
     {SLC_NF, SLC_NF, SLC_NF},  /* 3e                */
     {0x1a, 0x1a, 0x1a},        /* 3f (SUB)          */
     {0x20, 0x20, 0x20},        /* 40 (SP)           */
     {0xa1, SLC_NF, SLC_NF},    /* 41 */
     {0xa2, SLC_NF, SLC_NF},    /* 42 */
     {0xa3, SLC_NF, SLC_NF},    /* 43 */
     {0xa4, SLC_NF, SLC_NF},    /* 44 */
     {0xa5, SLC_NF, SLC_NF},    /* 45 */
     {0xa6, SLC_NF, SLC_NF},    /* 46 */
     {0xa7, SLC_NF, SLC_NF},    /* 47 */
     {0xa8, SLC_NF, SLC_NF},    /* 48 */
     {0xa9, SLC_NF, SLC_NF},    /* 49 */
     {0x5b, 0x5b, 0x5b},        /* 4a */
     {0x2e, 0x2e, 0x2e},        /* 4b */
     {0x3c, 0x3c, 0x3c},        /* 4c */
     {0x28, 0x28, 0x28},        /* 4d */
     {0x2b, 0x2b, 0x2b},        /* 4e */
     {0x21, 0x21, 0x21},        /* 4f */
     {0x26, 0x26, 0x26},        /* 50 */
     {0xaa, SLC_NF, SLC_NF},    /* 51 */
     {0xab, SLC_NF, SLC_NF},    /* 52 */
     {0xac, SLC_NF, SLC_NF},    /* 53 */
     {0xad, SLC_NF, SLC_NF},    /* 54 */
     {0xae, SLC_NF, SLC_NF},    /* 55 */
     {0xaf, SLC_NF, SLC_NF},    /* 56 */
     {SLC_NF,SLC_NF, SLC_NF},   /* 57 */
     {0xb0, SLC_NF, SLC_NF},    /* 58 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 59 */
     {0x5d, 0x5d, 0x5d},        /* 5a */
     {0x5c, 0x24, 0x24},        /* 5b */
     {0x2a, 0x2a, 0x2a},        /* 5c */
     {0x29, 0x29, 0x29},        /* 5d */
     {0x3b, 0x3b, 0x3b},        /* 5e */
     {0x5e, 0x5e, 0x5e},        /* 5f */
     {0x2d, 0x2d, 0x2d},        /* 60 */
     {0x2f, 0x2f, 0x2f},        /* 61 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 62 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 63 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 64 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 65 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 66 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 67 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 68 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 69 */
     {0x7c, 0x7c, 0x7c},        /* 6a */
     {0x2c, 0x2c, 0x2c},        /* 6b */
     {0x25, 0x25, 0x25},        /* 6c */
     {0x5f, 0x5f, 0x5f},        /* 6d */
     {0x3e, 0x3e, 0x3e},        /* 6e */
     {0x3f, 0x3f, 0x3f},        /* 6f */
     {SLC_NF, SLC_NF, SLC_NF},  /* 70 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 71 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 72 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 73 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 74 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 75 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 76 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 77 */
     {SLC_NF, SLC_NF, SLC_NF},  /* 78 */
     {0x60, 0x60, 0x60},        /* 79 */
     {0x3a, 0x3a, 0x3a},        /* 7a */
     {0x23, 0x23, 0x23},        /* 7b */
     {0x40, 0x40, 0x40},        /* 7c */
     {0x27, 0x27, 0x27},        /* 7d */
     {0x3d, 0x3d, 0x3d},        /* 7e */
     {0x22, 0x22, 0x22},        /* 7f */
     {SLC_NF, SLC_NF, SLC_NF},  /* 80 */
     {0xb1, 0x61, 0x61},        /* 81 */
     {0xb2, 0x62, 0x62},        /* 82 */
     {0xb3, 0x63, 0x63},        /* 83 */
     {0xb4, 0x64, 0x64},        /* 84 */
     {0xb5, 0x65, 0x65},        /* 85 */
     {0xb6, 0x66, 0x66},        /* 86 */
     {0xb7, 0x67, 0x67},        /* 87 */
     {0xb8, 0x68, 0x68},        /* 88 */
     {0xb9, 0x69, 0x69},        /* 89 */
     {0xba, SLC_NF, SLC_NF},    /* 8a */
     {SLC_NF, SLC_NF, SLC_NF},  /* 8b */
     {0xbb, SLC_NF, SLC_NF},    /* 8c */
     {0xbc, SLC_NF, SLC_NF},    /* 8d */
     {0xbd, SLC_NF, SLC_NF},    /* 8e */
     {0xbe, SLC_NF, SLC_NF},    /* 8f */
     {0xbf, SLC_NF, SLC_NF},    /* 90 */
     {0xc0, 0x6a, 0x6a},        /* 91 */
     {0xc1, 0x6b, 0x6b},        /* 92 */
     {0xc2, 0x6c, 0x6c},        /* 93 */
     {0xc3, 0x6d, 0x6d},        /* 94 */
     {0xc4, 0x6e, 0x6e},        /* 95 */
     {0xc5, 0x6f, 0x6f},        /* 96 */
     {0xc6, 0x70, 0x70},        /* 97 */
     {0xc7, 0x71, 0x71},        /* 98 */
     {0xc8, 0x72, 0x72},        /* 99 */
     {0xc9, SLC_NF, SLC_NF},    /* 9a */
     {SLC_NF, SLC_NF, SLC_NF},  /* 9b */
     {SLC_NF, SLC_NF, SLC_NF},  /* 9c */
     {0xca, SLC_NF, SLC_NF},    /* 9d */
     {0xcb, SLC_NF, SLC_NF},    /* 9e */
     {0xcc, SLC_NF, SLC_NF},    /* 9f */
     {SLC_NF, SLC_NF, SLC_NF},  /* a0 */
     {0x7e, 0x7e, 0x7e},        /* a1 */
     {0xcd, 0x73, 0x73},        /* a2 */
     {0xce, 0x74, 0x74},        /* a3 */
     {0xcf, 0x75, 0x75},        /* a4 */
     {0xd0, 0x76, 0x76},        /* a5 */
     {0xd1, 0x77, 0x77},        /* a6 */
     {0xd2, 0x78, 0x78},        /* a7 */
     {0xd3, 0x79, 0x79},        /* a8 */
     {0xd4, 0x7a, 0x7a},        /* a9 */
     {0xd5, SLC_NF, SLC_NF},    /* aa */
     {SLC_NF, SLC_NF, SLC_NF},  /* ab */
     {0xd6, SLC_NF, SLC_NF},    /* ac */
     {0xd7, SLC_NF, SLC_NF},    /* ad */
     {0xd8, SLC_NF, SLC_NF},    /* ae */
     {0xd9, SLC_NF, SLC_NF},    /* af */
     {SLC_NF, SLC_NF, SLC_NF},  /* b0 */
     {SLC_NF, SLC_NF, SLC_NF},  /* b1 */
     {SLC_NF, SLC_NF, SLC_NF},  /* b2 */
     {SLC_NF, SLC_NF, SLC_NF},  /* b3 */
     {SLC_NF, SLC_NF, SLC_NF},  /* b4 */
     {SLC_NF, SLC_NF, SLC_NF},  /* b5 */
     {SLC_NF, SLC_NF, SLC_NF},  /* b6 */
     {SLC_NF, SLC_NF, SLC_NF},  /* b7 */
     {SLC_NF, SLC_NF, SLC_NF},  /* b8 */
     {SLC_NF, SLC_NF, SLC_NF},  /* b9 */
     {0xda, SLC_NF, SLC_NF},    /* ba */
     {0xdb, SLC_NF, SLC_NF},    /* bb */
     {0xdc, SLC_NF, SLC_NF},    /* bc */
     {0xdd, SLC_NF, SLC_NF},    /* bd */
     {0xde, SLC_NF, SLC_NF},    /* be */
     {0xdf, SLC_NF, SLC_NF},    /* bf */
     {0x7b, 0x7b, 0x7b},        /* c0 */
     {0x41, 0x41, 0x41},        /* c1 */
     {0x42, 0x42, 0x42},        /* c2 */
     {0x43, 0x43, 0x43},        /* c3 */
     {0x44, 0x44, 0x44},        /* c4 */
     {0x45, 0x45, 0x45},        /* c5 */
     {0x46, 0x46, 0x46},        /* c6 */
     {0x47, 0x47, 0x47},        /* c7 */
     {0x48, 0x48, 0x48},        /* c8 */
     {0x49, 0x49, 0x49},        /* c9 */
     {SLC_NF, SLC_NF, SLC_NF},  /* ca */
     {SLC_NF, SLC_NF, SLC_NF},  /* cb */
     {SLC_NF, SLC_NF, SLC_NF},  /* cc */
     {SLC_NF, SLC_NF, SLC_NF},  /* cd */
     {SLC_NF, SLC_NF, SLC_NF},  /* ce */
     {SLC_NF, SLC_NF, SLC_NF},  /* cf */
     {0x7d, 0x7d, 0x7d},        /* d0 */
     {0x4a, 0x4a, 0x4a},        /* d1 */
     {0x4b, 0x4b, 0x4b},        /* d2 */
     {0x4c, 0x4c, 0x4c},        /* d3 */
     {0x4d, 0x4d, 0x4d},        /* d4 */
     {0x4e, 0x4e, 0x4e},        /* d5 */
     {0x4f, 0x4f, 0x4f},        /* d6 */
     {0x50, 0x50, 0x50},        /* d7 */
     {0x51, 0x51, 0x51},        /* d8 */
     {0x52, 0x52, 0x52},        /* d9 */
     {SLC_NF, SLC_NF, SLC_NF},  /* da */
     {SLC_NF, SLC_NF, SLC_NF},  /* db */
     {SLC_NF, SLC_NF, SLC_NF},  /* dc */
     {SLC_NF, SLC_NF, SLC_NF},  /* dd */
     {SLC_NF, SLC_NF, SLC_NF},  /* de */
     {SLC_NF, SLC_NF, SLC_NF},  /* df */
     {0x24, 0x5c, 0x5c},        /* e0 */
     {SLC_NF, SLC_NF, SLC_NF},  /* e1 */
     {0x53, 0x53, 0x53},        /* e2 */
     {0x54, 0x54, 0x54},        /* e3 */
     {0x55, 0x55, 0x55},        /* e4 */
     {0x56, 0x56, 0x56},        /* e5 */
     {0x57, 0x57, 0x57},        /* e6 */
     {0x58, 0x58, 0x58},        /* e7 */
     {0x59, 0x59, 0x59},        /* e8 */
     {0x5a, 0x5a, 0x5a},        /* e9 */
     {SLC_NF, SLC_NF, SLC_NF},  /* ea */
     {SLC_NF, SLC_NF, SLC_NF},  /* eb */
     {SLC_NF, SLC_NF, SLC_NF},  /* ec */
     {SLC_NF, SLC_NF, SLC_NF},  /* ed */
     {SLC_NF, SLC_NF, SLC_NF},  /* ee */
     {SLC_NF, SLC_NF, SLC_NF},  /* ef */
     {0x30, 0x30, 0x30},        /* f0 */
     {0x31, 0x31, 0x31},        /* f1 */
     {0x32, 0x32, 0x32},        /* f2 */
     {0x33, 0x33, 0x33},        /* f3 */
     {0x34, 0x34, 0x34},        /* f4 */
     {0x35, 0x35, 0x35},        /* f5 */
     {0x36, 0x36, 0x36},        /* f6 */
     {0x37, 0x37, 0x37},        /* f7 */
     {0x38, 0x38, 0x38},        /* f8 */
     {0x39, 0x39, 0x39},        /* f9 */
     {SLC_NF, SLC_NF, SLC_NF},  /* fa */
     {SLC_NF, SLC_NF, SLC_NF},  /* fb */
     {SLC_NF, SLC_NF, SLC_NF},  /* fc */
     {SLC_NF, SLC_NF, SLC_NF},  /* fd */
     {SLC_NF, SLC_NF, SLC_NF},  /* fe */
     {0xff, 0xff, 0xff}         /* ff */
     };

#define  SLCD_A        0xc1
#define  SLCD_B        0xc2
#define  SLCD_C        0xc3
#define  SLCD_D        0xc4
#define  SLCD_E        0xc5
#define  SLCD_F        0xc6
#define  SLCD_G        0xc7
#define  SLCD_H        0xc8
#define  SLCD_I        0xc9
#define  SLCD_J        0xd1
#define  SLCD_K        0xd2
#define  SLCD_L        0xd3
#define  SLCD_M        0xd4
#define  SLCD_N        0xd5
#define  SLCD_O        0xd6
#define  SLCD_P        0xd7
#define  SLCD_Q        0xd8
#define  SLCD_R        0xd9
#define  SLCD_S        0xe2
#define  SLCD_T        0xe3
#define  SLCD_U        0xe4
#define  SLCD_V        0xe5
#define  SLCD_W        0xe6
#define  SLCD_Y        0xe8
#define  SLCD_Z        0xe9
#define  SLCD_BAR      0x60
#define  SLCD_APOST    0x7d

unsigned char slcrkboin[5] =
     {SLCD_A, SLCD_I, SLCD_U, SLCD_E, SLCD_O};

unsigned char slcrksiin[20] = {
     SLCD_B, SLCD_C, SLCD_D, SLCD_F, SLCD_G,
     SLCD_H, SLCD_J, SLCD_K, SLCD_L, SLCD_M,
     SLCD_N, SLCD_P, SLCD_Q, SLCD_R, SLCD_S,
     SLCD_T, SLCD_V, SLCD_W, SLCD_Y, SLCD_Z
     };

unsigned char slcrkk1[][2] =
     {0x81, 0x00, 0x82, 0x00, 0x83, 0x00, 0x84, 0x00, 0x85, 0x00};
     /*  BOIN  */

unsigned char slcrkk2[][4] = {
     0x9d,0xbe,0x00,0x00,  0x9e,0xbe,0x00,0x00,  0x9f,0xbe,0x00,0x00,
     0xa2,0xbe,0x00,0x00,  0xa3,0xbe,0x00,0x00,               /* b */
     0x86,0x00,0x00,0x00,  0x8d,0x00,0x00,0x00,  0x87,0x54,0x00,0x00,
     0x8f,0x00,0x00,0x00,  0x8a,0x00,0x00,0x00,               /* c */
     0x91,0xbe,0x00,0x00,  0x92,0xbe,0x00,0x00,  0x93,0xbe,0x00,0x00,
     0x94,0xbe,0x00,0x00,  0x95,0xbe,0x00,0x00,               /* d */
     0x9f,0x47,0x00,0x00,  0x9f,0x48,0x00,0x00,  0x9f,0x00,0x00,0x00,
     0x9f,0x51,0x00,0x00,  0x9f,0x52,0x00,0x00,               /* f */
     0x86,0xbe,0x00,0x00,  0x87,0xbe,0x00,0x00,  0x88,0xbe,0x00,0x00,
     0x89,0xbe,0x00,0x00,  0x8a,0xbe,0x00,0x00,               /* g */
     0x9d,0x00,0x00,0x00,  0x9e,0x00,0x00,0x00,  0x9f,0x00,0x00,0x00,
     0xa2,0x00,0x00,0x00,  0xa3,0x00,0x00,0x00,               /* h */
     0x8d,0xbe,0x53,0x00,  0x8d,0xbe,0x00,0x00,  0x8d,0xbe,0x54,0x00,
     0x8d,0xbe,0x51,0x00,  0x8d,0xbe,0x55,0x00,               /* j */
     0x86,0x00,0x00,0x00,  0x87,0x00,0x00,0x00,  0x88,0x00,0x00,0x00,
     0x89,0x00,0x00,0x00,  0x8a,0x00,0x00,0x00,               /* k */
     0xad,0x00,0x00,0x00,  0xae,0x00,0x00,0x00,  0xaf,0x00,0x00,0x00,
     0xba,0x00,0x00,0x00,  0xbb,0x00,0x00,0x00,               /* l */
     0xa4,0x00,0x00,0x00,  0xa5,0x00,0x00,0x00,  0xa6,0x00,0x00,0x00,
     0xa7,0x00,0x00,0x00,  0xa8,0x00,0x00,0x00,               /* m */
     0x96,0x00,0x00,0x00,  0x97,0x00,0x00,0x00,  0x98,0x00,0x00,0x00,
     0x99,0x00,0x00,0x00,  0x9a,0x00,0x00,0x00,               /* n */
     0x9d,0xbf,0x00,0x00,  0x9e,0xbf,0x00,0x00,  0x9f,0xbf,0x00,0x00,
     0xa2,0xbf,0x00,0x00,  0xa3,0xbf,0x00,0x00,               /* p */
     0x88,0x47,0x00,0x00,  0x88,0x48,0x00,0x00,  0x88,0x00,0x00,0x00,
     0x88,0x51,0x00,0x00,  0x88,0x52,0x00,0x00,               /* q */
     0xad,0x00,0x00,0x00,  0xae,0x00,0x00,0x00,  0xaf,0x00,0x00,0x00,
     0xba,0x00,0x00,0x00,  0xbb,0x00,0x00,0x00,               /* r */
     0x8c,0x00,0x00,0x00,  0x8d,0x00,0x00,0x00,  0x8e,0x00,0x00,0x00,
     0x8f,0x00,0x00,0x00,  0x90,0x00,0x00,0x00,               /* s */
     0x91,0x00,0x00,0x00,  0x92,0x00,0x00,0x00,  0x93,0x00,0x00,0x00,
     0x94,0x00,0x00,0x00,  0x95,0x00,0x00,0x00,               /* t */
     0x83,0xbe,0x47,0x00,  0x83,0xbe,0x48,0x00,  0x83,0xbe,0x00,0x00,
     0x83,0xbe,0x51,0x00,  0x83,0xbe,0x52,0x00,               /* v */
     0xbc,0x00,0x00,0x00,  0x83,0x48,0x00,0x00,  0x83,0x00,0x00,0x00,
     0x83,0x51,0x00,0x00,  0x46,0x00,0x00,0x00,               /* w */
     0xa9,0x00,0x00,0x00,  0x82,0x00,0x00,0x00,  0xaa,0x00,0x00,0x00,
     0x82,0x51,0x00,0x00,  0xac,0x00,0x00,0x00,               /* y */
     0x8c,0xbe,0x00,0x00,  0x8d,0xbe,0x00,0x00,  0x8e,0xbe,0x00,0x00,
     0x8f,0xbe,0x00,0x00,  0x90,0xbe,0x00,0x00                /* z */
     };

unsigned char slcrkk3[][2] = {0xbd, 0x00, 0x56, 0x00};

unsigned char slcrkt1[][4] = {
     0x92,0x53,0x00,0x00,  0x92,0x00,0x00,0x00,  0x92,0x54,0x00,0x00,
     0x92,0x51,0x00,0x00,  0x92,0x55,0x00,0x00,             /* ch? */
     0x94,0xbe,0x53,0x00,  0x94,0xbe,0x48,0x00,  0x94,0xbe,0x54,0x00,
     0x94,0xbe,0x51,0x00,  0x94,0xbe,0x55,0x00,             /* dh? */
     0x8d,0x53,0x00,0x00,  0x8d,0x00,0x00,0x00,  0x8d,0x54,0x00,0x00,
     0x8d,0x51,0x00,0x00,  0x8d,0x55,0x00,0x00,             /* sh? */
     0x94,0x53,0x00,0x00,  0x94,0x48,0x00,0x00,  0x94,0x54,0x00,0x00,
     0x94,0x51,0x00,0x00,  0x94,0x55,0x00,0x00,             /* th? */
     0x93,0x47,0x00,0x00,  0x93,0x48,0x00,0x00,  0x93,0x00,0x00,0x00,
     0x93,0x51,0x00,0x00,  0x93,0x52,0x00,0x00,             /* ts? */
     0x88,0xbe,0xbc,0x00,  0x88,0xbe,0x48,0x00,  0x88,0xbe,0x00,0x00,
     0x88,0xbe,0x51,0x00,  0x88,0xbe,0x52,0x00,             /* gw? */
     0x88,0xbc,0x00,0x00,  0x88,0x48,0x00,0x00,  0x88,0x00,0x00,0x00,
     0x88,0x51,0x00,0x00,  0x88,0x52,0x00,0x00              /* kw? */
     };

unsigned char slcrkt2[][3] = {
     0x9e,0xbe,0x00,   /*  b  */
     0x92,0x00,0x00,   /*  c  */
     0x92,0xbe,0x00,   /*  d  */
     0x9f,0x00,0x00,   /*  f  */
     0x87,0xbe,0x00,   /*  g  */
     0x9e,0x00,0x00,   /*  h  */
     0x8d,0xbe,0x00,   /*  j  */
     0x87,0x00,0x00,   /*  k  */
     0xae,0x00,0x00,   /*  l  */
     0xa5,0x00,0x00,   /*  m  */
     0x97,0x00,0x00,   /*  n  */
     0x9e,0xbf,0x00,   /*  p  */
     0x88,0x00,0x00,   /*  q  */
     0xae,0x00,0x00,   /*  r  */
     0x8d,0x00,0x00,   /*  s  */
     0x92,0x00,0x00,   /*  t  */
     0x8d,0xbe,0x00    /*  z  */
     };

unsigned char slcrkt3[][2] =
     {0x53, 0x00, 0x48, 0x00, 0x54, 0x00, 0x51, 0x00, 0x55, 0x00};

/*  slcetoa.c  : 1986.07.24 */

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

/*  slcatoe.c  :  1986.07.24  */

#define SLC_NF     0xff
#define SLCNF      0xffff

/*
 * 0byte is JAPANESE EBCDIC KANA
 * 1byte is JAPANESE EBCDIC ENGLISH
 * 2byte is USASCII
 */

extern unsigned char slcaetb[][3];

int slcatoe(incode, ebctype)
     register unsigned char incode;
     register int  ebctype;
     {
     register int type;
     register unsigned char (*ae)[3] = slcaetb;

     if(ebctype == C_USASCI)
          type = 2;
     else if(ebctype == C_JPNKAT)
          type = 0;
     else
          type = 1;
     if(ae[(unsigned)incode & 0377][type] == SLC_NF)
          return(-1);
     else
          return(ae[(unsigned)incode & 0377][type] & 0377);
     }
