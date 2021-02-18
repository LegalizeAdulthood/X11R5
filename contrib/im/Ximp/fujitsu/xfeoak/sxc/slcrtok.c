/* @(#)slcrtok.c	2.2 91/07/05 16:07:57 FUJITSU LIMITED. */
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
static char sccs_id[] = "@(#)slcrtok.c	2.2 91/07/05 16:07:57 FUJITSU LIMITED.";
#endif

/*  slcrtok.c  : 1986.04.22 */

#include <sys/types.h>
#include "slcode.h"
#include "slcrtok.h"

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
