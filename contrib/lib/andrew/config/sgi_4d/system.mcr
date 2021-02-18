/* Copyright IBM Corporation 1988,1989 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

/* The next two lines need to be kept in sync */

#define In_Imake 1
#include <sgi_4d/system.h>
        SYSTEM_H_FILE = sgi_4d/system.h
#undef In_Imake

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = sgi_4d
        SYS_OS_ARCH = sgi_mips

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
        XSRCDIR = /usr/src/X11R4/mit/
        XBASEDIR =
        XUTILDIR = /usr/bin/X11
        RANLIB = echo ranlib is not needed on this system

/* MIPS' compiler seems to have the standard set of PCC bugs dealing with 
 *     void...
 * The -G 0 is to prevent dynamically loadable modules from having global
 *     area sections.
 * The -Wl,D,8000000 switch is to move the data area down and
 * the -Wl,T,4000000 switch is to move the text area up so that
 *     dynamically loaded routines are addressible within the 28 bit jump
 *     offset limit of the MIPS architecture.
 */
#ifdef ReadObjects
              CC = cc -G 0 -cckr "-Wl,-D,8000000" "-Wl,-T,4000000"
              CCNOGO = cc -cckr "-Wl,-D,8000000" "-Wl,-T,4000000"
#else /* Do mmap instead */
              CC = cc -G 0 -cckr
              CCNOG0 = cc -cckr
#endif

     STD_DEFINES = -DSYSV -D_BSD_SIGNALS
     SYSAUX_LIBRARIES = -lsun -lc_s
     MMDFLIBS = -lsun -lmld

    MKSDPOOL = ${BASEDIR}/etc/mksdpool

/* Get site-specific inclusions */
#include <site.mcr>
