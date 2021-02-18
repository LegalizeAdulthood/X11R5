/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <hp800/system.h>
        SYSTEM_H_FILE = hp800/system.h
#undef In_Imake

/* These next three lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = hp800
        SYS_OS_ARCH = hp_pa_risc

/* Get parent inclusions */
#include <allsys.mcr>


/* Redefine for HP-UX 8.0 */
        XLIBDIR = /usr/lib/X11R4
        XINCDIR = /usr/include/X11R4

/* Now for the system-dependent information. */
  
        MALLOCALIGNMENT = 8
        CDEBUGFLAGS = +01 +DA1.0 +DS1.0
        PICFLAG= +z
        SHLIBLDFLAGS= -b -E

/* In order to handle all the defines needed for im.c */
        STD_DEFINES = +z -W p,-H500000

/* Get site-specific inclusions */
#include <site.mcr>
