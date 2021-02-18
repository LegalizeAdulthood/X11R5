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
static char *rcsid=  "$Header: /usr/local/src/x11r5.mvex/RCS/vdev.h,v 1.27 1991/09/12 23:22:11 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#ifndef VDEV_H
#define VDEV_H

/*
 * Communication protocol
 */
#define VDEV_TTY 	1
#define VDEV_TELNET 	2
#define VDEV_RLOGIN	3
/*
 * TTY flags
 */
#define VMI7BIT		1
#define VMIODDPARITY	2
#define VMIEVENPARITY	4

#define VDEV_BUFSIZ 512

typedef int (*METHOD)();

typedef struct 
{
    int		fd;		/* the file descriptor*/
    char	*dev_path;	/* pointer to device path name */
    int		protocol;	/* Communications protocol */

    /*
     * For communications
     */
    unsigned long       timeout_sec;        /* in seconds */
    unsigned long       timeout_usec;        /* in micro seconds */
    BOOL Verbose;
    BOOL SendOK;
    BOOL ReceiveOK;
    /*
     * For device unique stuff.
     */
    pointer dev_specific;

} VDEV, *VDEV_ptr;
#endif /* VDEV_H */
