/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <rt_aix221/system.h>
        SYSTEM_H_FILE = rt_aix221/system.h
#undef In_Imake

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = rt_aix221
        SYS_OS_ARCH = aix_rt

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
        STD_DEFINES = -Nn6000 -Nd8000 -Np2400 -Nt4000 \
	    -DBSD_INCLUDES -DBSD_REMAP_SIGNAL_TO_SIGVEC \
	    -U_C_func -a 

        RANLIB = echo ranlib is not needed this system
        XFC = /usr/lpp/X11/bin/bdftortx
        XMKFONTDIR = /usr/lpp/X11/bin/mkfontdir
#define ConstructMFLAGS 1


/* Get site-specific inclusions */
#include <site.mcr>
