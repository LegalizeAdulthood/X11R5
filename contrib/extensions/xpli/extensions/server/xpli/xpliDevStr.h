/*
 * Copyright IBM Corporation 1991
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
#ifndef XPLIDEVSTR_H
#define XPLIDEVSTR_H

#include <sys/types.h>
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "screenint.h"
#include "xpliStrkQHdr.h"

/*
  Because of the number of possible xpli devices, the devStr defined here
  represents a common interface. When an xpli device is configured into
  the server, it allocates one of these structures and returns a pointer
  to the configuring routine. Configuration normally happens at
  InitExtensions time, just after AddDevice is called. The devicePrivate
  pointer in the device struct is set to this devStr.
*/

typedef struct {
  unsigned long  digType;
      /* 00000001 - digitizer reports proximity */
      /* 00000002 - digitizer reports pressure */
  unsigned short coordScale; /* pixelPos = scalePos / (2**coordScale) */
  unsigned short samplesSec;
  unsigned short xResol, yResol; /* scalePos per cm */
  unsigned long  timeResolSec;
  unsigned long  timeResolNsec;
} t_pliConfig;

typedef struct s_xpliDevStr {
  /* functions supplied by device independent frontend */
  t_strokeQHdrP (*newQHdr)(); /* when new stroke */
  t_XYsP        (*newXYs)();  /* when new XYs needed */
  void          (*proxReport)(); /* when prox event wanted */
  void          (*startStroke)(); /* when start stroke Event wanted */
  void          (*inStroke)(); /* when in stroke event wanted */
  void          (*endStroke)(); /* when end stroke event wanted */        
  unsigned long dragThresh; /* time to wait before forcing mouse emul */
  unsigned short dragX, dragY; /* max size of motion box */
  /* functions supplied by device specific backend */
  t_pliConfig   config; /* configuration info */
  t_plicoeff    coeff;  /* alignment coefficients */
  DevicePtr    devP;   /* DIX device block */
  /* functions supplied by display backend */
  caddr_t      dispRAM; /* display RAM address */
  caddr_t      dispROM;
  caddr_t      dispCTL; /* display coprocessor address */
  caddr_t      dispIO;  /* display I/O address */
  ScreenPtr    screenP;     /* X screen for digitizer */
  /* functions suplied by display inker backend */
  void         (*drawCross)(); /* draw alignment cross & return cross loc */
  void         (*initInkPoint)(); /* set initial ink point for polyline */
  void         (*ink)();   /* draw ink in ink planes */
  void         (*endInk)(); /* no more points to ink */
  void         (*clear)(); /* clear the ink planes */
  /* functions supplied by coordinate processing & alignment */
  void         (*alignInit)(); /* set alignment mode */
  void         (*alignAbort)(); /* abort alignment mode */
  void         (*processXY)(); /* process XY and flag data */
} t_xpliDevStr, *t_xpliDevStrP;
  
#endif XPLIDEVSTR
