/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <sco_i386/system.h>
        SYSTEM_H_FILE = sco_i386/system.h
#undef In_Imake

/* These next three lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = sco_i386
        SYS_OS_ARCH = sco_i386

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
        CDEBUGFLAGS = -O
        STD_DEFINES = -DM_UNIX
        ConstructMFLAGS =
        XUTILDIR = /usr/bin/X11
        LN = ln
        DEPENDSCRIPT = $(CSHELL) $(TOP)/config/depend.csh
        RANLIB = echo ranlib is not required on this system
        NETLIBS = -lsocket
        MMDFLIBS = -lmmdf
        AUTHLIBS = -lcrypt -lprot

/* Get site-specific inclusions */
#include <site.mcr>
