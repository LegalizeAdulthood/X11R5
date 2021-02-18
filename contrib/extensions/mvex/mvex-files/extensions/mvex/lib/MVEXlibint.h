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
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/MVEXlibint.h,v 2.1 1991/08/13 19:46:19 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#ifndef _MVEXLIBINT_H_
#define _MVEXLIBINT_H_

#include "Xlib.h"
#include "MVEXlib.h"
#include "MVEXproto.h"

/*
 * Magic number
 */
#define	VideoNumberEvents	5

/*
 * device type.
 */
#define Input 1
#define Output 0

/*
 * MVEX library internal function declarations.
 */

extern XExtCodes *_MVEXCheckExtension ();
extern void _MVEXFreeVideoInfo();
extern Status _MVEXQueryVideo();

typedef struct {
	VideoIO		id;	/* created VideoID */
	union {
	    MVEXVin	*vin;	/* pointer to originating MVEXVin */
	    MVEXVout	*vout;	/* pointer to originating MVEXVout */
	} u;
} VidMap, *VidMapPtr;

typedef struct {
	VidMapPtr	map;
	int		num;
} VidList, *VidListPtr;

typedef struct {
	Atom	atom;
	char	*string;
} VEXAtom;

typedef struct {
	int		screen;
	int		input_overlap;
	int		capture_overlap;
	int		io_overlap;
	int		major_version;
	int		minor_version;
	unsigned long	query_video_time;
	unsigned long	video_connect_time;
	int		ndepths;
	Depth  		 *depths;
	int		nvin;
	MVEXVin		*vin;
	int		nvout;
	MVEXVout	*vout;
	VidList		vin_ids;
	VidList		vout_ids;
} VideoInfo;

VideoInfo *_MVEXGetVideoInfo();

#endif /* _MVEXLIBINT_H_ */
