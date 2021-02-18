/* $Id: system.mcr,v 1.6 1991/09/13 15:58:16 bobg Exp $ */
SHELL=/bin/sh
 
/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */
 
#define In_Imake 1
/* The next two lines need to be kept in sync */
#include <rs_aix31/system.h>
        SYSTEM_H_FILE = rs_aix31/system.h
#undef In_Imake
 
/* These next two lines help configure the embedded machine-dependent
    directories overhead/class/machdep, atk/console/stats, and atk/console/stats/common. */
        SYS_IDENT = rs_aix31
        SYS_OS_ARCH = aix_31
 
/* Get parent inclusions */
#include <allsys.mcr>
 
/* Now for the system-dependent information. */
CC = cc
STD_DEFINES = -DAIX -D_AIX -D_IBMR2 -DAIX_31 \
	-D_BSD -D_NO_PROTO -D_H_NLCTYPE -qflag=i:i
 
        RANLIB = echo ranlib is not needed this system
        XFC = /usr/lpp/X11/bin/bdftosnf
        XMKFONTDIR = /usr/lpp/X11/bin/mkfontdir

SYS_LIBRARIES = -L${BASEDIR}/lib

MAKEDOFLAGS = ${MAKEDODEBUG} -d ${BASEDIR}/lib -b ${BASEDIR}/bin ${LDFLAGS}
 
#define ConstructMFLAGS 1
 
/* Get site-specific inclusions */
 
#include <site.mcr>
 
