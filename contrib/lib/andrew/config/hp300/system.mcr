/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <hp300/system.h>
        SYSTEM_H_FILE = hp300/system.h
#undef In_Imake

/* These next three lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = hp300
        SYS_OS_ARCH = hp_68k

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
        CDEBUGFLAGS = +O1
              STD_DEFINES = +Nd4000 +Ns3000 -W p,-H500000  -W2,-x -D_CLASSIC_TYPES


/* Get site-specific inclusions */
#include <site.mcr>
