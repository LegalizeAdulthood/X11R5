/*
 * Copyright 1990 by Tektronix, Inc., Beaverton, Oregon, and
 * the Massachusetts Institute of Technology, Cambridge, Massachusetts.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Tektronix or M.I.T. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Tektronix and M.I.T. make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * TEKTRONIX AND M.I.T. DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL TEKTRONIX OR M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: /usr/local/src/x11r5/mit/extensions/RCS/roMvex.h,v 1.4 1991/09/19 22:34:01 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#ifndef _ROMVEX_H
#define _ROMVEX_H
/*
 *	DEFINES
 */

/*
 * Video-in
 *     - signal width/height.
 *     - picture width/height.
 *     - picture x and y start
 * See notes in roMvexInit.c
 */
#define RO_SIG_W	780	/* picture can be selected from this width */
#define RO_SIG_H	525	/* ... and this height */
#define RO_MAX_SIG_W	700	/* ... must be no wider than this */
#define RO_MAX_SIG_H	500	/* ... and no higher than this */
#define RO_MIN_Y	2	/* This is the minimum y */
#define RO_PICT_W	640	/* suggested signal width */
#define RO_PICT_H	480	/* suggested signal height */
#define RO_PICT_X	102	/* offset for x == 0 */
#define RO_PICT_Y	16	/* offset for y == 0 */

/*
 *	TYPEDEFS
 */

/*
 *	EXTERNS
 */
extern int RoPortDevices[];

extern Bool	roValidateRequest();
extern Bool	roSetupVideo();
extern Bool	roStartVideo();
extern void	roStopVideo();
extern void	roPollVideo();
static void	roReset();


#endif /* _ROMVEX_H */
