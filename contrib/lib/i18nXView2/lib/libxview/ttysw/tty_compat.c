#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)tty_compat.c 50.2 90/10/16";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Compatibility bridge for SunView1.X programs.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <xview_private/term_impl.h>
#ifdef OW_I18N
#include <widec.h>
#include <wctype.h>
#include <xctype.h>
#endif

ttysw_becomeconsole(ttysw0)
    caddr_t         ttysw0;
{
    xv_set(ttysw0, TTY_CONSOLE, TRUE, 0);
}

extern
ttysw_cmd(ttysw_opaque, buf, buflen)
    caddr_t         ttysw_opaque;
    char           *buf;
    int             buflen;
{
    int             result;

    (void) xv_set(ttysw_opaque, TTY_INPUT, buf, buflen, &result, 0);
    return (result);
}

ttysw_input(ttysw0, addr, len)
    caddr_t         ttysw0;
    char           *addr;
    int             len;
{
    return (ttysw_input_it(TTY_PRIVATE_FROM_ANY_PUBLIC(ttysw0), addr, len));
}

#ifdef OW_I18N
ttysw_input_wcs(ttysw0, addr, len)
    caddr_t		ttysw0;
    wchar_t		*addr;
    int			len;
{
    return (ttysw_input_it_wcs(TTY_PRIVATE_FROM_ANY_PUBLIC(ttysw0), addr, len));
}
#endif
