/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

/*
  this module provides ink and alignment cross services for the VGA
  device.
*/

#include "X.h"
#define NEED_EVENTS
#define NEED_REPLIES
#include "Xproto.h"
#include "servermd.h"
#include "misc.h"
#include "plicoord.h"
#include "xpliDevStr.h"

/* CROSSPOS is inset of cross from screen corners in pixels */
#define CROSSPOS ndc2tc(48) 
/* CROSSPOS is size of cross in pixels */
#define CROSSSIZE 64
#define CROSSADJ ndc2tc(CROSSSIZE/2)

#define INK_PLANE_MASK 0x08

/*---------- Common VGA defines used everywhere ---------*/

/* defines for vga register indexes */
/* read map select */
#define GA_GCA_RMS  0x04
/* undoc register for reading data latches */
#define GA_CRA_MRL  0x22
/* data rotate register */
#define GA_GCA_DRR  0x03
/* set/reset register */
#define GA_GCA_SRR  0x00
/* enable SRR register */
#define GA_GCA_ESR  0x01
/* graphics mode register */
#define GA_GCA_GMR  0x05
/* map mask register */
#define GA_SQA_MMR  0x02
/* bit mask register */
#define GA_GCA_BMR  0x08

#define GA_MEMREF(offs)  *((unsigned char *)(dispRAM+offs))

/* --------  Defines for the RS 6000 --------*/
#ifdef _IBMR2

#define GA_GCA_ADDR  (dispIO + 0x3CE)
#define GA_GCV_ADDR  (dispIO + 0x3CF)
#define GA_SQA_ADDR  (dispIO + 0x3C4)
#define GA_SQV_ADDR  (dispIO + 0x3C5)
#define GA_CRA_ADDR  (dispIO + 0x3D4)
#define GA_CRV_ADDR  (dispIO + 0x3D5)

#define GA_GCA(new_val) (*((unsigned char *) GA_GCA_ADDR) = new_val)
#define GA_GCV(new_val) (*((unsigned char *) GA_GCV_ADDR) = new_val)
#define GA_SQA(new_val) (*((unsigned char *) GA_SQA_ADDR) = new_val)
#define GA_SQV(new_val) (*((unsigned char *) GA_SQV_ADDR) = new_val)
#define GA_CRA(new_val) (*((unsigned char *) GA_CRA_ADDR) = new_val)
#define GA_CRV(new_val) (*((unsigned char *) GA_CRV_ADDR) = new_val)
#define GA_GCAR *((unsigned char *) GA_GCA_ADDR)
#define GA_GCVR *((unsigned char *) GA_GCV_ADDR)
#define GA_SQAR *((unsigned char *) GA_SQA_ADDR)
#define GA_SQVR *((unsigned char *) GA_SQV_ADDR)
#define GA_CRAR *((unsigned char *) GA_CRA_ADDR)
#define GA_CRVR *((unsigned char *) GA_CRV_ADDR)

#endif /* RS 6000 */

#ifdef PS2
/* PS2 specific defines for addressing and accessing adapter */

#define GA_GCA_ADDR  0x3ce
#define GA_GCV_ADDR  0x3cf
#define GA_SQA_ADDR  0x3c4
#define GA_SQV_ADDR  0x3c5
#define GA_CRA_ADDR  0x3d4  
#define GA_CRV_ADDR  0x3d5  

#ifdef DRIVER
#define GA_GCA(new_val) iooutb(GA_GCA_ADDR,new_val)
#define GA_GCV(new_val) iooutb(GA_GCV_ADDR,new_val)
#define GA_CRA(new_val) iooutb(GA_CRA_ADDR,new_val)
#define GA_CRV(new_val) iooutb(GA_CRV_ADDR,new_val)
#define GA_SQA(new_val) iooutb(GA_SQA_ADDR,new_val)
#define GA_SQV(new_val) iooutb(GA_SQV_ADDR,new_val)
#define GA_GCAR ioinb(GA_GCA_ADDR)
#define GA_GCVR ioinb(GA_GCV_ADDR)
#define GA_CRAR ioinb(GA_CRA_ADDR)
#define GA_CRVR ioinb(GA_CRV_ADDR)
#define GA_SQAR ioinb(GA_SQA_ADDR)
#define GA_SQVR ioinb(GA_SQV_ADDR)
#else
#define GA_GCA(new_val) outb(GA_GCA_ADDR,new_val)
#define GA_GCV(new_val) outb(GA_GCV_ADDR,new_val)
#define GA_CRA(new_val) outb(GA_CRA_ADDR,new_val)
#define GA_CRV(new_val) outb(GA_CRV_ADDR,new_val)
#define GA_SQA(new_val) outb(GA_SQA_ADDR,new_val)
#define GA_SQV(new_val) outb(GA_SQV_ADDR,new_val)
#define GA_GCAR inb(GA_GCA_ADDR)
#define GA_GCVR inb(GA_GCV_ADDR)
#define GA_CRAR inb(GA_CRA_ADDR)
#define GA_CRVR inb(GA_CRV_ADDR)
#define GA_SQAR inb(GA_SQA_ADDR)
#define GA_SQVR inb(GA_SQV_ADDR)
#endif /* DRIVER */
#endif /* PS2 */

/*--------------- More common functions -----------------*/

#define GA_DRR(new_val) \
  (GA_GCA(GA_GCA_DRR), GA_GCV(new_val))
#define GA_MMR(new_val) \
  (GA_SQA(GA_SQA_MMR), GA_SQV(new_val))
#define GA_GMR(new_val) \
  (GA_GCA(GA_GCA_GMR), GA_GCV(new_val))
#define GA_SRR(new_val) \
  (GA_GCA(GA_GCA_SRR), GA_GCV(new_val))
#define GA_ESR(new_val) \
  (GA_GCA(GA_GCA_ESR), GA_GCV(new_val))
#define GA_BMR(new_val) \
  (GA_GCA(GA_GCA_BMR), GA_GCV(new_val))

/*---------- VGA and DRIVER/CLIENT specific routines --------*/

#ifdef DRIVER
/* VGA driver code saves and restores VGA state */

/* GA is never busy for driver */
#define GA_BUSY 0

static struct{
  unsigned char ga_gca, ga_drr, ga_gmr, ga_srr, ga_esr, ga_rms, ga_bmr,
                ga_sqa, ga_mmr, ga_cra, ga_mem[4], ga_lat[4], ga_gbg
} gaSave;

/* save only the VGA registers we actually use */
#define GA_LOCK \
  gaSave.ga_cra = GA_CRAR; gaSave.ga_gca = GA_GCAR; \
  gaSave.ga_sqa = GA_SQAR; GA_CRA(GA_CRA_MRL); \
  GA_GCA(GA_GCA_RMS); gaSave.ga_rms = GA_GCVR; \
  GA_GCV(0x00); gaSave.ga_lat[0] = GA_CRVR; \
  GA_GCV(0x01); gaSave.ga_lat[1] = GA_CRVR; \
  GA_GCV(0x02); gaSave.ga_lat[2] = GA_CRVR; \
  GA_GCV(0x03); gaSave.ga_lat[3] = GA_CRVR; \
  GA_GCA(GA_GCA_DRR); gaSave.ga_drr = GA_GCVR; \
  GA_GCA(GA_GCA_SRR); gaSave.ga_srr = GA_GCVR; \
  GA_GCA(GA_GCA_ESR); gaSave.ga_esr = GA_GCVR; \
  GA_GCA(GA_GCA_GMR); gaSave.ga_gmr = GA_GCVR; \
  GA_GCA(GA_GCA_BMR); gaSave.ga_bmr = GA_GCVR; \
  GA_SQA(GA_SQA_MMR); gaSave.ga_mmr = GA_SQVR;

  /* set the GMR to read mode 0 (no color compare)
     then set the read map select & read each plane at AFFFF, 
     then set write mode 0, 
     then set the map mask and write the latch data to each plane at AFFFF,
     then load the latches from AFFFF, 
     then write the data from AFFFF back to AFFFF,
     then restore the remaining registers */
#define GA_UNLOCK \
  GA_GCA(GA_GCA_GMR); GA_GCV(gaSave.ga_gmr&0xF7); \
  GA_GCA(GA_GCA_RMS); \
  GA_GCV(0x00); gaSave.ga_mem[0] = GA_MEMREF(0xFFFF); \
  GA_GCV(0x01); gaSave.ga_mem[1] = GA_MEMREF(0xFFFF); \
  GA_GCV(0x02); gaSave.ga_mem[2] = GA_MEMREF(0xFFFF); \
  GA_GCV(0x03); gaSave.ga_mem[3] = GA_MEMREF(0xFFFF); \
  GA_GCA(GA_GCA_GMR); GA_GCV(gaSave.ga_gmr&0xFC); \
  GA_SQA(GA_SQA_MMR); \
  GA_SQV(0x00); GA_MEMREF(0xFFFF) = gaSave.ga_lat[0]; \
  GA_SQV(0x01); GA_MEMREF(0xFFFF) = gaSave.ga_lat[1]; \
  GA_SQV(0x02); GA_MEMREF(0xFFFF) = gaSave.ga_lat[2]; \
  GA_SQV(0x03); GA_MEMREF(0xFFFF) = gaSave.ga_lat[3]; \
  gaSave.ga_gbg = GA_MEMREF(0xFFFF); \
  GA_SQV(0x00); GA_MEMREF(0xFFFF) = gaSave.ga_mem[0]; \
  GA_SQV(0x01); GA_MEMREF(0xFFFF) = gaSave.ga_mem[1]; \
  GA_SQV(0x02); GA_MEMREF(0xFFFF) = gaSave.ga_mem[2]; \
  GA_SQV(0x03); GA_MEMREF(0xFFFF) = gaSave.ga_mem[3]; \
  GA_GCA(GA_GCA_SRR); GA_GCV(gaSave.ga_srr); \
  GA_GCA(GA_GCA_ESR); GA_GCV(gaSave.ga_esr); \
  GA_GCA(GA_GCA_DRR); GA_GCV(gaSave.ga_drr); \
  GA_GCA(GA_GCA_RMS); GA_GCV(gaSave.ga_rms); \
  GA_GCA(GA_GCA_GMR); GA_GCV(gaSave.ga_gmr); \
  GA_GCA(GA_GCA_BMR); GA_GCV(gaSave.ga_bmr); \
  GA_SQA(GA_SQA_MMR); GA_SQV(gaSave.ga_mmr); \
  GA_GCA(gaSave.ga_gca); GA_SQA(gaSave.ga_sqa); \
  GA_CRA(gaSave.ga_cra);

#else
/* not the driver, busy, lock and unlock macros do nothing */

#define GA_BUSY 0
#define GA_LOCK
#define GA_UNLOCK

#endif /* DRIVER */

#define XPELS 640
#define XWORDS 80
#define XMAX  639
#define YPELS 480
#define YMAX  479

#define POSINF (int) 0x00010000  /* positive infinity */
#define NEGINF (int) 0xfffe0000  /* negative infinity */

#define MINMOVE 2                /* min distance to draw line */

/************************************************************************
*       DrawInkLine                                                      *
************************************************************************/
static t_xpliDevStrP xpliDevStrP;
static t_dCoord x1, y1;
static int scale, round;
static int firstdot = -1, eraseink = 0, ok = 0;
static char *dispIO, dispRAM;

static void 
InitInkPoint(x, y, ink)
     t_tCoord x, y; int ink;
{
  x1 = ntc2dc(x);   y1 = ntc2dc(y);   firstdot = 1;
  if (ink) eraseink = 0; else eraseink = 1;
  return;
}

static void
Ink(ox2, oy2)
     t_tCoord ox2, oy2;
{
  long dx, dy, t, xy0, xym, yx0, yxm, e, es, en, x2, y2;
  int cntint;
  unsigned int o1, o2, dot;
  volatile unsigned char temp, *vgamemP;
  caddr_t dispIO, dispRAM;

  /* is the adapter busy */
  if (GA_BUSY) return;

  /* adapter type seen by InitInk was not correct */
  if (ok == 0) return;
  
  /* NOTE: comments are in terms of slope octants, rather
     than up and down because the VGA buffer addressing
     is effectively quadrant 4 rather than quadrant 1 */
  
  if (firstdot < 0) 
    {
      /* x1, y1, invalid - create line from origin to clue
	 operator that there is trouble with the code */
      x1 = 0;
      y1 = 0;
      firstdot = 1;
    }
  
  x2 = ntc2dc(ox2);
  y2 = ntc2dc(oy2);

  /* calculate deltas */
  dx = x2-x1;
  dy = y2-y1;

  if ((dx == 0) && (dy == 0))
    return;

  if ((x1 < 0) || (x1 > XMAX) || (x2 < 0) || (x2 > XMAX) ||
      (y1 < 0) || (y1 > YMAX) || (y2 < 0) || (y2 > YMAX))
    {
      /* line not entirely on display, check for entirely outside  */
      if (((x1 < 0) && (x2 < 0)) || ((x1 > XMAX) && (x2 > XMAX)) ||
	  ((y1 < 0) && (y2 < 0)) || ((y1 > YMAX) && (y2 > YMAX)))
	{
	  x1 = ntc2dc(ox2);
	  y1 = ntc2dc(oy2);
	  return;
	}
      else /* line not entirely outside, clip necessary */
	{
	  /* clip necessary */
	  
	  /* NOTE !!! - this code works on complement arithmetic only */
	  /* observe the use of ^ for sign tests                      */
	  
	  /* 
	    x == x1 - (((dx*((yb-y1)<<1)+-dy))/(dy<<1))
	    X intercept at Y boundary calculation
	    yb = y boundary
	    dy added or subtracted for rounding depending on the
	    sign of dx*(yb-y1)
	    NOTE: works for |x|, |y| < 2**15
	    */
	  
	  /* compute the x intersections with the upper and lower bounds */
	  if (dy != 0)
	    {
	      /* case yb == 0 */
	      xy0 = x1 + (dx * (-(y1<<1) - 1)) / (dy << 1);
	      
	      /* case yb == YMAX */
	      xym = x1 + (dx * (((YMAX - y1)<<1) + 1)) / (dy << 1);
	    }
	  else /* dy == 0, make x intersections outside the line */
	    {
	      xy0 = NEGINF;
	      xym = POSINF;
	    }

	  /* compute intersections with left and right bounds */
	  if (dx != 0)
	    {
	      /* case xb == 0 */
	      yx0 = y1 + (dy * (-(x1<<2) - 1)) / (dx << 1);
	      
	      /* case xb == XMAX */
	      t =  (XMAX - x1) << 1;
	      yxm = y1 + (dy * (((XMAX-x1)<<1) + 1)) / (dx << 1);
	    }
	  else /* dx == 0, make y intersections outside the line */
	    {
	      yx0 =  NEGINF;
	      yxm = POSINF;
	    }

	  cntint = 0;  /* count the valid intersections */

	  /* if the intersection with y == 0 is valid, clip the line */
	  if (((((xy0-x1)^(xy0-x2)) < 0)  
	       || ((dy > 0) && (xy0 == x2))
               || ((dy < 0) && (xy0 == x1)))
	      && ((y1^y2) < 0))
	    {
	      cntint++;
	      if (dy > 0) /* x1, y1 is outside */
		{
		  y1 = 0;
		  x1 = xy0;
		}
	      else /* dy < 0 , x2,y2 is outside */
		{
		  y2 = 0;
		  x2 = xy0;
		}
	    }
	  
	  /* if the intersection with y == YMAX is valid, clip the line */
	  if (((((xym-x1)^(xym-x2)) < 0)  
	       || ((dy < 0) && (xym == x2))
	       || ((dy > 0) && (xym == x1)))
	      && (((y1-YMAX)^(y2-YMAX)) < 0))
	    {
	      cntint++;
	      if (dy < 0) /* x1, y1 is outside */
		{
		  y1 = YMAX;
		  x1 = xym;
		}
	      else /* dy > 0 , x2,y2 is outside */
		{
		  y2 = YMAX;
		  x2 = xym;
		}
	    }
	  
	  /* if the intersection with x == 0 is valid, clip the line */
	  if (((((yx0-y1)^(yx0-y2)) < 0)  
	       || ((dx > 0) && (yx0 == y2))
               || ((dx < 0) && (yx0 == y1)))
	      && ((x1^x2) < 0))
	    {
	      cntint++;
	      if (dx > 0) /* x1, y1 is outside */
		{
		  x1 = 0;
		  y1 = yx0;
		}
	      else /* dx < 0 , y2,x2 is outside */
		{
		  x2 = 0;
		  y2 = yx0;
		}
	    }
	  
	  /* if the intersection with x == XMAX is valid, clip the line */
	  if (((((yxm-y1)^(yxm-y2)) < 0)  
	       || ((dx < 0) && (yxm == y2))
	       || ((dx > 0) && (yxm == y1)))
	      && (((x1-XMAX)^(x2-XMAX)) < 0))
	    {
	      cntint++;
	      if (dx < 0) /* x1, y1 is outside */
		{
		  x1 = XMAX;
		  y1 = yxm;
		}
	      else /* dx > 0 , y2,x2 is outside */
		{
		  x2 = XMAX;
		  y2 = yxm;
		}
	    }
	  if (cntint == 0) /* no valid intersections were found */
	    {
	      x1 = ntc2dc(ox2);
	      y1 = ntc2dc(oy2);
	      return;
	    }
	  dx = x2 - x1;
	  dy = y2 - y1;  
	} /* end of else clip is necessary */
    } /* end of line not entirely inside */

  /* set the adapter lock */
  GA_LOCK;

  /* set access to ink planes only */
  GA_MMR(INK_PLANE_MASK);
  GA_GMR(0x00);
  /* set function select. data rotate */
  GA_DRR(0x00);

  if (eraseink)
    {
      /* set color to 0 in ink planes */
      GA_SRR(0x00);
    }
  else
    {
      /* set color to ones in ink planes */
      GA_SRR(INK_PLANE_MASK);
    }
  /* enable the set/reset values */
  GA_ESR(INK_PLANE_MASK);
  /* set GA to access bit mask register */
  GA_GCA(GA_GCA_BMR);

  /* set the VGA buffer pointer */
  vgamemP = dispRAM + y1*XWORDS + (x1>>3);
  /* set the initial dot position */
  dot = 0x80 >> (x1 & 0x7);
  
  /* write the first dot */
  if (firstdot)
    {
      /* setup the dot in the bit mask register */
      GA_GCV(dot);
      /* read to load VGA latches from buffer */
      temp = *vgamemP;
      /* write the dot */
      *vgamemP = dot;
    }
  
  /* resolve the octant */
  if (dx >= 0)
    {
      if (dy >= 0)
	{
	  if (dy <= dx)
	    {
	      /* octant 1, dx >= 0 && dy >= 0 && dy <= dx */
	      e = 2*dy - dx;
	      es = 2*(dy - dx);
	      en = 2*dy;
	      for (; dx>0; dx--)
		{
		  /* test error term for y step */
		  if (e>=0)
		    {
		      /* step the Y */
		      vgamemP += XWORDS;
		      e += es;
		    }
		  else
		    e += en;
		  /* move dot next bit to the right */
		  if ((dot = dot >> 1) == 0) 
		    {
		      dot = 0x80;
		      vgamemP++;
		    }
		  /* setup the dot in the bit mask register */
		  GA_GCV(dot);
		  /* read from buffer word to load VGA latches */
		  temp = *vgamemP;
		  /* write the dot (OR with content of latches) */
		  *vgamemP = dot;
		}
	    }
	  else /* dy > dx */ 
	    {
	      /* octant 2, dx >= 0 && dy >= 0 && dy > dx */
	      e = dx * 2 - dy;
	      es = 2 * (dx - dy);
	      en = 2 * dx;
	      for (;dy > 0;dy--)
		{
		  /* test error term for x step */
		  if (e>=0)
		    {
		      /* move dot next bit to the right */
		      if ((dot = dot >> 1) == 0)
			{
			  dot = 0x80;
			  vgamemP++;
			}
		      e += es;
		    }
		  else
		    e += en;
		  /* step the y */
		  vgamemP += XWORDS;
		  /* setup the dot in the bit mask register */
		  GA_GCV(dot);
		  /* read from buffer word to load VGA latches */
		  temp = *vgamemP;
		  /* write the dot (OR with content of latches) */
		  *vgamemP = dot;
		}
	    }
	}
      else /* dy < 0 */
	{
	  if ((dy = -dy) <= dx)
	    {
	      /* octant 8 */
	      e = 2*dy - dx;
	      es = 2*(dy - dx);
	      en = 2*dy;
	      for (; dx > 0; dx--)
		{
		  /* test error term for y step */
		  if (e>=0)
		    {
		      /* step the Y */
		      vgamemP -= XWORDS;
		      e += es;
		    }
		  else
		    e += en;
		  /* move dot next bit to the right */
		  if ((dot = dot >> 1) == 0)
		    {
		      dot = 0x80;
		      vgamemP++;
		    }
		  /* setup the dot in the bit mask register */
		  GA_GCV(dot);
		  /* read from buffer word to load VGA latches */
		  temp = *vgamemP;
		  /* write the dot (OR with content of latches) */
		  *vgamemP = dot;
		}
	    }
	  else /* octant 7, dx >= 0 && dy < 0 && dy > dx */
	    {
	      e = dx * 2 - dy;
	      es = 2 * (dx - dy);
	      en = 2 * dx;
	      for (;dy > 0; dy--)
		{
		  /* test error term for x step */
		  if (e>=0)
		    {
		      /* move dot next bit to the right */
		      if ((dot = dot >> 1) == 0)
			{
			  dot = 0x80;
			  vgamemP++;
			}
		      e += es;
		    }
		  else
		    e += en;
		  /* step the y */
		  vgamemP -= XWORDS;
		  /* setup the dot in the bit mask register */
		  GA_GCV(dot);
		  /* read from buffer word to load VGA latches */
		  temp = *vgamemP;
		  /* write the dot (OR with content of latches) */
		  *vgamemP = dot;
		}
	    }
	}
    }
  else /* dx < 0 */
    {
      /* make dx > 0 */
      dx = -dx;
      
      if (dy >= 0)
	{
	  if (dy <= dx)
	    {
	      /* octant 4 */
	      e = 2*dy - dx;
	      es = 2*(dy - dx);
	      en = 2*dy;
	      for (; dx>0; dx--)
		{
		  /* test error term for y step */
		  if (e>=0)
		    {
		      /* step the Y */
		      vgamemP += XWORDS;
		      e += es;
		    }
		  else
		    e += en;
		  /* move dot next bit to the left */
		  if ((dot = dot << 1) > 0x80)
		    {
		      dot = 0x01;
		      vgamemP--;
		    }
		  /* setup the dot in the bit mask register */
		  GA_GCV(dot);
		  /* read from buffer word to load VGA latches */
		  temp = *vgamemP;
		  /* write the dot (OR with content of latches) */
		  *vgamemP = dot;
		}
	    }
	  else
	    {
	      /* octant 3 */
	      e = dx * 2 - dy;
	      es = 2 * (dx - dy);
	      en = 2 * dx;
	      for (;dy > 0;dy--)
		{
		  /* test error term for x step */
		  if (e>=0)
		    {
		      /* move dot next bit to the left */
		      if ((dot = dot << 1) > 0x80)
			{
			  dot = 0x01;
			  vgamemP--;
			}
		      e += es;
		    }
		  else
		    e += en;
		  /* step the y */
		  vgamemP += XWORDS;
		  /* setup the dot in the bit mask register */
		  GA_GCV(dot);
		  /* read from buffer word to load VGA latches */
		  temp = *vgamemP;
		  /* write the dot (OR with content of latches) */
		  *vgamemP = dot;
		}
	    }
	}
      else /* dy < 0 */
	{
	  /* make dy > 0 */
	  dy = -dy;
	  
	  if (dy <= dx)
	    {
	      /* octant 5 */
	      e = 2*dy - dx;
	      es = 2*(dy - dx);
	      en = 2*dy;
	      for (; dx > 0; dx--)
		{
		  /* test error term for y step */
		  if (e>=0)
		    {
		      /* step the Y */
		      vgamemP -= XWORDS;
		      e += es;
		    }
		  else
		    e += en;
		  /* move dot next bit to the left */
		  if ((dot = dot << 1) > 0x80)
		    {
		      dot = 0x01;
		      vgamemP--;
		    }
		  /* setup the dot in the bit mask register */
		  GA_GCV(dot);
		  /* read from buffer word to load VGA latches */
		  temp = *vgamemP;
		  /* write the dot (OR with content of latches) */
		  *vgamemP = dot;
		}
	    }
	  else /* dx >= 0 && dy < 0 && dy > dx */
	    {
	      e = dx * 2 - dy;
	      es = 2 * (dx - dy);
	      en = 2 * dx;
	      for (;dy > 0; dy--)
		{
		  /* test error term for x step */
		  if (e>=0)
		    {
		      /* move dot next bit to the left */
		      if ((dot = dot << 1) > 0x80)
			{
			  dot = 0x01;
			  vgamemP--;
			}
		      e += es;
		    }
		  else
		    e += en;
		  /* step the y */
		  vgamemP -= XWORDS;
		  /* setup the dot in the bit mask register */
		  GA_GCV(dot);
		  /* read from buffer word to load VGA latches */
		  temp = *vgamemP;
		  /* write the dot (OR with content of latches) */
		  *vgamemP = dot;
		}
	    }
	
	}
    }
  
  /* setup for next line segment of polyline */
  firstdot = 0;
  x1 = ntc2dc(ox2);
  y1 = ntc2dc(oy2);

  /* unlock the adapter */
  GA_UNLOCK;

  return;
} /* end function */

static void
EndInk()
{ /* does nothing in this implementation - but, if ink erasure
     were done by a device driver or by hardware and Ink were
     sending down XYs, this call would
     signal that all the XY's have been sent and that the stroke can
     be erased */
}
  
static void
ClearInkPlane()
{
  int i;
  unsigned char *vgamemP;
  /* set access to ink planes only */
  GA_MMR(INK_PLANE_MASK);
  GA_GMR(0x00);
  /* set function select. data rotate */
  GA_DRR(0x00);
  /* set color to 0 in ink planes */
  GA_SRR(0x00);
    /* enable the set/reset values */
  GA_ESR(INK_PLANE_MASK);
  /* set bit mask register to all bits, 0xff */
  GA_BMR(0xff);
  vgamemP = dispRAM;
  for (i=0; i<(XWORDS*YPELS); i++) *vgamemP++ = (unsigned char) 0x00;
}

/************************************************************************
*       AlignmentCross Functions                                        *
************************************************************************/

static void
DrawCross(ink, crossNo, dxP, dyP)
     int crossNo, *dxP, *dyP;
{ int dx, dy;
  switch (crossNo) {
  case 1:
    *dxP = *dyP = dx = dy = CROSSPOS;
    break;
  case 2:
    *dxP = dx = XMAX - CROSSPOS;
    *dyP = dy = CROSSPOS;
    break;
  case 3:
    *dxP = dx = XMAX - CROSSPOS;
    *dyP = dy = YMAX - CROSSPOS;
    break; }
  InitInkPoint(ink, dx-CROSSADJ, dy);
  Ink(dx+CROSSADJ, dy); 
  InitInkPoint(ink, dx, dy-CROSSADJ);
  Ink(dx, dy+CROSSADJ);
}

/* the only public function is this one - all others are accessed via
   the xpliDevStr. */
int
InitVGAInk(IIxpliDevStrP)
   t_xpliDevStrP IIxpliDevStrP;
{
  int rc;
  /* ASSUMED: the display init routine has been called which identifies
     the display type, obtains access to the display RAM and registers,
     and records the addressess of these items in the xpliDevStr. */
  xpliDevStrP = IIxpliDevStrP;
  dispIO = xpliDevStrP->dispIO;
  dispRAM = xpliDevStrP->dispRAM;
  /* setup function pointers in xpliDevStr */
  IIxpliDevStrP->drawCross = DrawCross;
  IIxpliDevStrP->initInkPoint = InitInkPoint;
  IIxpliDevStrP->ink = Ink;
  IIxpliDevStrP->endInk = EndInk;
  IIxpliDevStrP->clear = ClearInkPlane;
  return 1;
}
  
