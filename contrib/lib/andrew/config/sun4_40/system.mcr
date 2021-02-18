/* Copyright IBM Corporation 1988,1989 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <sun4_40/system.h>
        SYSTEM_H_FILE = sun4_40/system.h
#undef In_Imake

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = sun4_40
        SYS_OS_ARCH = sun_sparc

/* Get parent inclusions */
#include <allsys.mcr>

#ifdef OPENWINDOWS_ENV
        CONVERTFONT = /usr/openwin/bin/convertfont
        BLDFAMILY = /usr/openwin/bin/bldfamily
#endif /*OPENWINDOWS_ENV */

/* Now for the system-dependent information. */

        MALLOCALIGNMENT = 8


/* Get site-specific inclusions */
#include <site.mcr>
