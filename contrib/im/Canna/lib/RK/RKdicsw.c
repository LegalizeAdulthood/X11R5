/* Copyright 1991 NEC Corporation, Tokyo, Japan.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of NEC Corporation
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.  NEC 
 * Corporation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 *
 * NEC CORPORATION DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN 
 * NO EVENT SHALL NEC CORPORATION BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF 
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR 
 * OTHER TORTUOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE. 
 */

#ifndef lint
static char rcsid[]="@(#) 102.1 $Header: /work/nk.R3.1/lib/RK/RCS/RKdicsw.c,v 1.3 91/05/21 18:39:40 mako Exp $";
#endif
/*LINTLIBRARY*/

#include	"RK.h"
#include	"RKintern.h"
extern int	_Rkpopen(), _Rkpclose(), _Rkpsearch(), _Rkpio(), _Rkpctl();
extern int	_Rktopen(), _Rktclose(), _Rktsearch(), _Rktio(), _Rktctl();

struct RkDST 	_RkDST[] = {
/* PERMDIC */ { _Rkpopen, _Rkpclose, _Rkpsearch, _Rkpio, _Rkpctl,  },
/* TEMPDIC */ { _Rktopen, _Rktclose, _Rktsearch, _Rktio, _Rktctl,  },
};
