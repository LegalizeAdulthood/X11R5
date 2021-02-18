/*
			P r i m i t i v e . h

    $Header: Primitive.h,v 1.0 91/10/04 17:01:01 rthomson Exp $

    This file contains procedure declarations and useful constants and
    macros for the PEX Toolkit.

		Copyright (C) 1990, 1991, Evans & Sutherland

			 Author: Rich Thomson
		      Date: May 4th, 1990

*/

/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

	Copyright 1990, 1991 by Evans & Sutherland Computer Corporation,
			   Salt Lake City, Utah

			    All Rights Reserved

    Permission to use, copy, modify, and distribute this software and its 
    documentation for any purpose and without fee is hereby granted, 
    provided that the above copyright notice appear in all copies and that
    both that copyright notice and this permission notice appear in 
    supporting documentation, and that the names of Evans & Sutherland not be
    used in advertising or publicity pertaining to distribution of the
    software without specific, written prior permission.  

    EVANS & SUTHERLAND  DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
    INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
    EVENT SHALL EVANS & SUTHERLAND BE LIABLE FOR ANY SPECIAL, INDIRECT OR
    CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
    DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/

#ifndef PEXt_Primitive_h
#define PEXt_Primitive_h

void PEXtCircle2();			/* 2D circle primitive */
void PEXtCircle();			/* 3D circle primitive */
void PEXtEllipse2();			/* 2D ellipse primitive */
void PEXtEllipsoid2();			/* 2D ellipsoid primitive */

void PEXtCube();			/* 3D unit cube */
void PEXtCylinder();			/* 3D "unit" cylinder */
void PEXtSphere();			/* 3D "unit" sphere */

#endif PEXt_Primitive_h
