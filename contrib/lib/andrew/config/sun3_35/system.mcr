/* Copyright IBM Corporation 1988,1989 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <sun3_35/system.h>
        SYSTEM_H_FILE = sun3_35/system.h
#undef In_Imake

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = sun3_35
        SYS_OS_ARCH = sun_68k

/* Get parent inclusions */
#include <allsys.mcr>

#ifdef OPENWINDOWS_ENV
        CONVERTFONT = /usr/openwin/bin/convertfont
        BLDFAMILY = /usr/openwin/bin/bldfamily
#endif /*OPENWINDOWS_ENV */

/* Now for the system-dependent information. */
/* (currently none) */

/* Get site-specific inclusions */
#include <site.mcr>
