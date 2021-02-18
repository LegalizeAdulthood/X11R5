/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <sun3_41/system.h>
        SYSTEM_H_FILE = sun3_41/system.h
#undef In_Imake

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = sun3_41
        SYS_OS_ARCH = sun_68k_41

/* Get parent inclusions */
#include <allsys.mcr>

#ifdef OPENWINDOWS_ENV
        CONVERTFONT = /usr/openwin/bin/convertfont
        BLDFAMILY = /usr/openwin/bin/bldfamily
#endif /*OPENWINDOWS_ENV */

/* Now for the system-dependent information. */

/* Flag to compile position-independent code. */

        PICFLAG = -pic

/* Flags to make a shared library. */

        SHLIBLDFLAGS = -assert pure-text

/* Get site-specific inclusions */
#include <site.mcr>
