/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <i386_mach/system.h>
        SYSTEM_H_FILE = i386_mach/system.h
#undef In_Imake

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = i386_mach
        SYS_OS_ARCH = i386_mach


/* Get parent inclusions */
#include <allsys.mcr>

/* Now for the system-dependent information. */

        CDEBUGFLAGS = -O -fwritable-strings      

/* Get site-specific inclusions */
#include <site.mcr>
