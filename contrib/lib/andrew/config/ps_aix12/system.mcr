/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <ps_aix12/system.h>
        SYSTEM_H_FILE = ps_aix12/system.h
#undef In_Imake

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = ps_aix12
        SYS_OS_ARCH = aix_i386

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
        XSRCDIR = /
        INCLUDES =  -I${BASEDIR}/include/atk -I${BASEDIR}/include -I${XBASEDIR}/usr/include

        CC = cc
        STD_DEFINES =  -D_BSD
#define ConstructMFLAGS 1
        RANLIB = echo ranlib is not needed on this system

#ifdef LINKINSTALL_ENV
         RM = rm -fs
#else
          RM = rm -f
#endif /* LINKINSTALL_ENV */

          NM = /bin/nm

/* Get site-specific inclusions */
#include <site.mcr>
