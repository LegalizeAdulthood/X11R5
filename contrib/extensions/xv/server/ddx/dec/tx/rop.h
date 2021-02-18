/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/*
** File: 
**
**   rop.h --- RasterOps device interface header
**
** Author: 
**
**   Joel Gringorten ???
**
** Revisions:
**
**   29.08.91 Carver
**     - added RopRec and RopPtr types; this structure is now used by rop.c
**       and libpip.c uniformly as a handle to the board.
**
*/
#ifndef ROP_H
#define ROP_H

typedef struct {
	unsigned char *board;
	unsigned char *bit8;
	unsigned char *bit24;
	unsigned char *SelectionPlane;
	unsigned char *VideoEnablePlane;
	unsigned char *mapRegister;
	unsigned char *MappedArea;  /*3max only */
	unsigned char *pipRegisters;
	unsigned char *dutyCycle;
} ropStruct, RopRec, *RopPtr;

#ifndef TCO_DUTY_CYCLE    0x040020  /* Duty Cycle */
#define TCO_DUTY_CYCLE    0x040020  /* Duty Cycle */
#endif

#endif ROP_H
