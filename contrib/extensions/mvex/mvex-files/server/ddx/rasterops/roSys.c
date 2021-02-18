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
static char *rcsid=  "$Header: /users/x11devel/x11r5/mit/server/RCS/roSys.c,v 1.3 1991/09/23 00:57:51 toddb Exp $";
#endif /* RCS_ID */
#endif /* LINT   */

#include    "sun.h"
#include    "roStruct.h"
#include    "servermd.h"

#include    <pixrect/cg8var.h>
#include    <pixrect/pr_planegroups.h>
#include    <stdio.h>


static Bool Running;
/*
 * The intent for this file is to "mostly" abstract the device dependence
 * if the implementation.  In theory, this implementation will run
 * on a dec box by changing this file along.  Ha, ha.
 */

/*
 *    NAME
 *        roIoctl - do an ioctl on the rasterops
 *
 *    SYNOPSIS
 */
Bool
roIoctl(fd, command, value, errorReturn)
    int fd;
    int command;
    pointer value;
    int *errorReturn;
/*
 *    DESCRIPTION
 *        perform an ioctl.  If it fails, set errorReturn to BadAlloc.
 *
 *    RETURNS
 *        TRUE if the ioctl succeeded; FALSE otherwise (and errorReturn
 *	  is set to BadAlloc).
 */
{
    if (ioctl(fd, command, value) < 0) {
	char *name;

	switch (command) {
        case PIPIO_G_FB_INFO:		name = "get fb info"; break;
        case PIPIO_G_CURSOR_COLOR_FREEZE:name = "get color freeze"; break;
        case PIPIO_G_ALTERNATE_PLL:	name = "get alternate pll"; break;
        case PIPIO_G_FIELD_ONLY:	name = "get field only"; break;
        case PIPIO_G_FIELD_INVERT:	name = "get field invert"; break;
        case PIPIO_G_FLIP_HORIZONTAL:	name = "get flip horizontal"; break;
        case PIPIO_G_FLIP_VERTICAL:	name = "get flip vertical"; break;
        case PIPIO_G_PIP_ON_OFF:	name = "get running"; break;
        case PIPIO_G_PIP_ON_OFF_S_OFF:	name = "get halt"; break;
        case PIPIO_G_PIP_PRESENT:	name = "get present"; break;
        case PIPIO_G_SOURCE_ACTIVE:	name = "get active"; break;
        case PIPIO_G_SOURCE:		name = "get signal"; break;
        case PIPIO_G_TIMING:		name = "get timing"; break;
        case PIPIO_G_DAC_BRIGHTNESS:	name = "get brightness"; break;
        case PIPIO_G_DAC_CONTRAST:	name = "get contrast"; break;
        case PIPIO_G_DAC_HUE:		name = "get hue"; break;
        case PIPIO_G_DAC_SATURATION:	name = "get saturation"; break;
        case PIPIO_G_EMULATION_MODE:	name = "get emulation"; break;
        case PIPIO_G_SIGRECT:		name = "get source rectangle"; break;
        case PIPIO_G_OUTRECT:		name = "get dest rectangle"; break;

        case PIPIO_S_CURSOR_COLOR_FREEZE:name = "set color freeze"; break;
        case PIPIO_S_ALTERNATE_PLL:	name = "set alternate pll"; break;
        case PIPIO_S_FIELD_ONLY:	name = "set field only"; break;
        case PIPIO_S_FIELD_INVERT:	name = "set field invert"; break;
        case PIPIO_S_FLIP_HORIZONTAL:	name = "set flip horizontal"; break;
        case PIPIO_S_FLIP_VERTICAL:	name = "set flip vertical"; break;
        case PIPIO_S_PIP_ON_OFF:	name = "set running"; break;
        case PIPIO_S_SOURCE:		name = "set port"; break;
        case PIPIO_S_EMULATION_MODE:	name = "set emulation"; break;
        case PIPIO_S_SIGRECT:		name = "set src rectangle"; break;
	case PIPIO_S_OUTRECT:		name = "set dest rectangle";
	default:			name = "?? command"; break;

        case PIPIO_S_DAC_BRIGHTNESS:
        case PIPIO_S_DAC_CONTRAST:
        case PIPIO_S_DAC_HUE:
        case PIPIO_S_DAC_SATURATION:
	    /*
	     * Hack! The 3.0 version of the sun Rasterops
	     * driver returns '1' by accident, which translates
	     * to an EPERM.  The 3.1 version of the 
	     * driver returns '22' (EINVAL), but makes the change
	     * anyway.  Ignore the error (we assume the 3.1 driver here).
	     */
	    if (errno == EINVAL)
		return TRUE;

	    switch (command) {
	    case PIPIO_S_DAC_BRIGHTNESS:name = "set brightness"; break;
	    case PIPIO_S_DAC_CONTRAST:	name = "set contrast"; break;
	    case PIPIO_S_DAC_HUE:	name = "set hue"; break;
	    case PIPIO_S_DAC_SATURATION:name = "set saturation"; break;
	    }
	    break;
	}

	ErrorF("RasterOps ioctl error: ");
	perror(name);
	*errorReturn = BadAlloc;
	return FALSE;
    }

    return TRUE;
}

/*
 *    NAME
 *        roIsActive - see if pip is active yet
 *
 *    SYNOPSIS
 */
Bool
roIsActive(fd)
    int fd;
/*
 *    DESCRIPTION
 *        see if pip is active, but don't spend much time at it
 *
 *    RETURNS
 *        true if its active, false if not active or an error
 */
{
    int errorReturn;
    int value;

    if (! roIoctl(fd, PIPIO_G_SOURCE_ACTIVE, (pointer)&value, &errorReturn))
	return FALSE;
    return (value != 0);
}

/*
 *    NAME
 *        roWaitActive - wait for pip to do something
 *
 *    SYNOPSIS
 */
Bool
roConfirmActive(fd, errorReturn)
    int fd;
    int *errorReturn;
/*
 *    DESCRIPTION
 *        wait until pip is active;  avoid infinite loops
 *
 *    RETURNS
 *        true if its active, false if not active or an error
 */
{
    int value;
    int count = 0;

    for (;;) {
        if (! roIoctl(fd, PIPIO_G_SOURCE_ACTIVE, (pointer)&value, errorReturn))
            return FALSE;
        if (value)
            break;

        usleep(1);	/* smallest amount of time */
        if (! roIoctl(fd, PIPIO_G_SOURCE_ACTIVE, (pointer)&value, errorReturn))
            return FALSE;
        if (value)
            break;

	/*
	 * If we have tried this for a while, don't wait any longer.
	 */
        if (++count > 3)
	    return FALSE;

	/*
	 * XXX this should be done asynchronously
	 */
        usleep(100);	/* ~ .1 millisecond */
    }
    return TRUE;
}

/*
 *    NAME
 *        roWaitNotRunning - wait for pip to stop running
 *
 *    SYNOPSIS
 */
void
roWaitNotRunning(fd, errorReturn)
    int fd;
    int *errorReturn;
/*
 *    DESCRIPTION
 *        wait until pip stops running
 *
 *    RETURNS
 *        true if its active, false if not active or an error
 */
{
    int value;
    int count = 0;

    for (;;) {
	if (! roIoctl(fd, PIPIO_G_PIP_ON_OFF, (pointer)&value, errorReturn))
	    return;
	/*
	 * if it says its off, check it one more time to be sure that
	 * its not just in horizontal retrace.
	 */
	if (! value) {
	    usleep(1);
	    if (! roIoctl(fd, PIPIO_G_PIP_ON_OFF, (pointer)&value, errorReturn))
		return;
	    if (! value)
		return;
	}

	/*
	 * Just to make sure... we turn it off again.
	 */
	value = FALSE;
	if (! roIoctl(fd, PIPIO_S_PIP_ON_OFF, (pointer)&value, errorReturn))
	    return;

        usleep(100);	/* ~ .1 millisecond */
    }
}


/*
 *    NAME
 *        roIoctl - do an ioctl on the rasterops
 *
 *    SYNOPSIS
 */
Bool
roPipRunning(fd)
    int fd;
/*
 *    DESCRIPTION
 *        see if pip is still running
 *
 *    RETURNS
 *        true if its running, false if not active or an error
 */
{
    int value, errorReturn;

    if (! roIoctl(fd, PIPIO_G_PIP_ON_OFF, (pointer)&value, &errorReturn))
	return FALSE;
    return (value != 0);
}

Bool
roSetSaturationDac(fd, value, errorReturn)
    int fd;
    int value;
    int *errorReturn;
{
    return roIoctl(fd, PIPIO_S_DAC_SATURATION, (pointer)&value, errorReturn);
}

Bool
roSetContrastDac(fd, value, errorReturn)
    int fd;
    int value;
    int *errorReturn;
{
    return roIoctl(fd, PIPIO_S_DAC_HUE, (pointer)&value, errorReturn);
}

Bool
roSetHueDac(fd, value, errorReturn)
    int fd;
    int value;
    int *errorReturn;
{
    return roIoctl(fd, PIPIO_S_DAC_HUE, (pointer)&value, errorReturn);
}

Bool
roSetBrightnessDac(fd, value, errorReturn)
    int fd;
    int value;
    int *errorReturn;
{
    return roIoctl(fd, PIPIO_S_DAC_BRIGHTNESS, (pointer)&value, errorReturn);
}

Bool
roSetPort(fd, value, errorReturn)
    int fd;
    int value;
    int *errorReturn;
{
    return roIoctl(fd, PIPIO_S_SOURCE, (pointer)&value, errorReturn);
}

static void
CheckRectangle(fd, cmd, value)
    int fd;
    int cmd;
    struct rect *value;
{
    struct rect actual;
    int junk;

    if (! roIoctl(fd, cmd, (pointer)&actual, &junk))
	return;
    if (value->r_left    != actual.r_left
     || value->r_top     != actual.r_top
     || value->r_width   != actual.r_width
     || value->r_height  != actual.r_height) {
	ErrorF("%s rect: requested {%d,%d,%d,%d}, driver did {%d,%d,%d,%d}\n",
	    cmd == PIPIO_G_OUTRECT ? "dest" : "src",
	    value->r_left,  value->r_top,  value->r_width,  value->r_height,
	    actual.r_left, actual.r_top, actual.r_width, actual.r_height);
	*value = actual;
    }
}

Bool
roSetInputRectangle(fd, value, errorReturn)
    int fd;
    struct rect *value;
    int *errorReturn;
{
    if (! roIoctl(fd, PIPIO_S_SIGRECT, (pointer)value, errorReturn))
	return FALSE;
    CheckRectangle(fd, PIPIO_G_SIGRECT, value);
}

Bool
roSetOutputRectangle(fd, value, errorReturn)
    int fd;
    struct rect *value;
    int *errorReturn;
{
    if (! roIoctl(fd, PIPIO_S_OUTRECT, (pointer)value, errorReturn))
	return FALSE;
    CheckRectangle(fd, PIPIO_G_OUTRECT, value);
}

Bool
roStartPip(fd, errorReturn)
    int fd;
    int *errorReturn;
{
    int value;

    if (Running)
	return;
    Running = value = TRUE;
    return roIoctl(fd, PIPIO_S_PIP_ON_OFF, (pointer)&value, errorReturn);
}

Bool
roStopPip(fd, errorReturn)
    int fd;
    int *errorReturn;
{
    int value;

    if (! Running)
	return;
    Running = value = FALSE;
    return roIoctl(fd, PIPIO_S_PIP_ON_OFF, (pointer)&value, errorReturn);
}
