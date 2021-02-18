/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <pmax_3/system.h>
        SYSTEM_H_FILE = pmax_3/system.h
#undef In_Imake

/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and
    atk/console/stats/common. */
        SYS_IDENT = pmax_3
        SYS_OS_ARCH = dec_mips

/* Get parent inclusions */
#include <allsys.mcr>


/* Now for the system-dependent information. */
/* (currently none) */

/* MIPS' compiler seems to have the standard set of PCC bugs dealing with 
 *     void...
 * The -G 0 is to prevent dynamically loadable modules from having global
 *     area sections.
 * The -Wl,D,1000000 switch is to move the data area down so that
 *     dynamically loaded routines are addressible within the 28 bit jump
 *     offset limit of the MIPS architecture.
 */
        CC = cc -Dvoid=int -G 0
        CCNOG0 = cc -Dvoid=int
        STD_DEFINES =  "-Wl,-D,1000000"
        MIPS_GS_FILE = $(DESTDIR)/etc/mips_global_space.c
        MIPS_GSIZE_SCRIPT = $(DESTDIR)/etc/mips_gsize.awk

/* Get site-specific inclusions */
#include <site.mcr>
