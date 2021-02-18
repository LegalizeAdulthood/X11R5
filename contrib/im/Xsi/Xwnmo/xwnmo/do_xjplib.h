/*
 * $Id: do_xjplib.h,v 1.2 1991/09/16 21:36:43 ohm Exp $
 */
/*
 * Copyright 1991 by OMRON Corporation
 * Copyright 1991 by Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of OMRON and MIT not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  OMRON and MIT make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * Wnn consortium is one of distributors of the official Wnn source code
 * release.  Wnn consortium also makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * OMRON, MIT AND WNN CONSORTIUM DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OMRON, MIT OR WNN CONSORTIUM BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTUOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 *	Author:	Seiji Kuwari	OMRON Corporation
 *				kuwa@omron.co.jp
 *				kuwa%omron.co.jp@uunet.uu.net
 */				

#include <stdio.h>
#include "commonhd.h"
#include "sdefine.h"
#include "xim.h"
#include "sheader.h"
#include "proto.h"
#include "ext.h"

#ifdef	XJPLIB
typedef struct _XJpClientRec {
    long		dispmode;


    unsigned char c_data[128];
    Window		w;
    unsigned long	mask;
    long		p_width, p_height;
    long		x, y;
    long		width, height;
    unsigned long	fg, bg;
    long		bp;
    long		status_x, status_y;
    XIMClientRec	*xim_client;
#ifdef	XJPLIB_DIRECT
    int			direct_fd;
#endif	/* XJPLIB_DIRECT */
    int			ref_count;
    char		escape[4][4];
    struct _XJpClientRec	*next;
} XJpClientRec;

typedef struct _XJpInputRec {
    Window		w;
    XJpClientRec	*pclient;
    int			save_event;
    XKeyEvent		ev;
    struct _XJpInputRec	*next;
} XJpInputRec;

#define XJPLIB_PORT_IN	(0x9494)
#define UNIX_PATH	"/tmp/xwnmo.V2"
#define UNIX_ACPT	0
#define INET_ACPT	1

#define XJP_OPEN        1
#define XJP_CLOSE       2
#define XJP_BEGIN       3
#define XJP_END         4
#define XJP_VISIBLE     5
#define XJP_INVISIBLE   6
#define XJP_CHANGE      7
#define XJP_MOVE        8
#define XJP_INDICATOR   9

#define XJP_WINDOWID    1
#define XJP_ERROR       -1

#define         XJP_UNDER       0
#define         XJP_XY          1
#define         XJP_ROOT        2

#define         XJP_PWIDTH      0x00000001L
#define         XJP_PHEIGHT     0x00000002L
#define         XJP_X           0x00000004L
#define         XJP_Y           0x00000008L
#define         XJP_WIDTH       0x00000010L
#define         XJP_HEIGHT      0x00000020L
#define         XJP_FG          0x00000040L
#define         XJP_BG          0x00000080L
#define         XJP_BP          0x00000100L
#define         XJP_FONTS       0x00000200L
#define         XJP_STATUS      0x00000400L

#define XJP_F_FRONTEND_END 		0
#define XJP_F_FRONTEND_ERR_END		1
#define XJP_F_JSERVER_DISCONNECT	2

#define XJP_F_OPEN_BAD_WID		3
#define XJP_F_OPEN_BAD_SZ		4
#define XJP_F_OPEN_BAD_FN		5
#define XJP_F_OPEN_BAD_DM		6
#define XJP_F_OPEN_NOT_WIN		7
#define XJP_F_OPEN_NOT_SUPPORT		8

#define XJP_F_CLOSE_BAD_CL		9

#define XJP_F_BEGIN_BAD_CL		10
#define XJP_F_BEGIN_BAD_WID		11

#define XJP_F_END_BAD_CL		12
#define XJP_F_END_BAD_WID		13

#define XJP_F_VISIBLE_BAD_CL		14

#define XJP_F_INVISIBLE_BAD_CL		15

#define XJP_F_CHANGE_BAD_CL		16
#define XJP_F_CHANGE_BAD_SZ		17
#define XJP_F_CHANGE_NOT_WIN		18
#endif
