/* Copyright IBM Corporation 1988,1991 - All Rights Reserved */
/* For full copyright information see:'andrew/config/COPYRITE' */

/**/#
/**/# The following is from the allsys.mcr file, and may be over-ridden
/**/# by the platform-specific system.mcr file or the
/**/# site-specific site.mcr file.
/**/#

/* System-independent macros: included at the head of system.mcr. */

/* XBASEDIR should point to the top of an X11 installation */
/* if you haven't installed X11, you can carefully set everything below */
/* that uses XBASEDIR to point to the right places */
        XBASEDIR = /
/* XSRCDIR should be an absolute path to the top of an X11 source tree */
/* it is only used for getting imake and makedepend (and fc under X11R2) */
/* if you have imake and makedepend installed somwhere, you can leave */
/* this blank and take care of imake and makedepend (and fc) below */
        XSRCDIR = 

#ifdef AFS_ENV
/* AFSBASEDIR should point to the top of an AFS installation. */
/* Thus, $(AFSBASEDIR)/lib/afs $(AFSBASEDIR)/include/afs should both exist. */
/* for historical reasons the default is set to /usr/andy; */
/* if you need to change this setting, you can do so in site.h . */
        AFSBASEDIR = /usr/andrew
#endif /* AFS_ENV */

/* If your AFS protection server relies on the MIT Athena Kerberos */
/* library, set KRBLIB to name where the Kerberos library lives. */
/* You will also need to define this variable if you make the MIT neos
application */
        KRBLIB = 

/* set DESTDIR to where you want Andrew installed */
/* it is necessary that it be installed incrementally, while it is built */
/* DEFAULT_ANDREWDIR_ENV is set in the allsys.h file to the location where */
/* DESTDIR will be visible by users after installation.  If this is different */
/* from the location where it will be installed incrementally at build time, */
/* set BASEDIR to that latter location by redefining it in your site.h file. */
        BASEDIR = DEFAULT_ANDREWDIR_ENV
        DESTDIR = ${BASEDIR}

/* XUTILDIR is for programs that have been installed in some place other */
/* than the standard location - the ITC uses it for 'makedepend' and 'imake' */
        XUTILDIR = /usr/local/bin

/* CDEBUGFLAGS are passed to C compilations.  */
/* To generate debugger symbol tables, use -g instead of -O.  */
        CDEBUGFLAGS = -O

/* MAKEDODEBUG determines whether .dog files are generated from makedo. */
/* Set it to -g to generate them.  You must set CDEBUGFLAGS to -g as well. */
        MAKEDODEBUG = 

        SHELL = /bin/sh
        CSHELL = /bin/csh
        CC = cc
/* Malloc will generate addresses divisible by MALLOCALIGNMENT,  */
/* which must be a multiple of 4 */
        MALLOCALIGNMENT = 4


/* Check and Set the RESOLVELIB path below */
/* There are at least two sources of libresolv.a */
/* 1) 4.3 distribution */
/* 2) bind distribution */
/* At CMU we use the bind distribution of libresolv.a */
#ifdef RESOLVER_ENV
/* if in a separate library: */
/*      RESOLVLIB = /usr/lib/libresolv.a
/* if in /lib/libc.a: */
        RESOLVLIB =
#else /* RESOLVER_ENV */
        RESOLVLIB =
#endif /* RESOLVER_ENV */

/* the following variable is for any libraries
 * needed for networking support
 */
        NETLIBS =
/* this is to catch machine-specific libraries that contain
 * fixes for various functions
 */
        SUPLIBS =

/* uncomment this if your make program has MAKEFLAGS but not MFLAGS */
/* #define ConstructMFLAGS */

        ADDALIASES = $(BASEDIR)/etc/addalias
        CLASS = ${BASEDIR}/bin/class
        MAKEDO = ${BASEDIR}/bin/makedo
        MAKEDOFLAGS = ${MAKEDODEBUG} -d ${BASEDIR}/lib -b ${BASEDIR}/bin
        DOINDEX = $(BASEDIR)/bin/doindex
        XINCDIR = $(XBASEDIR)/usr/include/X11
#ifdef AFS_ENV
        INCLUDES =  -I${BASEDIR}/include/atk -I${BASEDIR}/include -I$(AFSBASEDIR)/include -I${XINCDIR}
#else /* AFS_ENV */
        INCLUDES =  -I${BASEDIR}/include/atk -I${BASEDIR}/include -I${XINCDIR}
#endif /* AFS_ENV */
        CLASSINCLUDES = ${INCLUDES}
        DEPENDSCRIPT = $(TOP)/config/depend.csh
        XMAKEDEPEND = $(XUTILDIR)/makedepend
        IMAKE = $(XUTILDIR)/imake
        XLIBDIR = $(XBASEDIR)/usr/lib
        OLDXLIB = $(XLIBDIR)/liboldX.a
        IRULESRC = $(TOP)/config
        FDBWM = $(BASEDIR)/bin/fdbwm
        FDBBDF = $(BASEDIR)/bin/fdbbdf
#ifdef	RELEASE2_ENV
        XFC = $(XSRCDIR)/fonts/compiler/fc
#else	/* RELEASE2_ENV */
#ifndef FONTS_TO_PCF_ENV
        XFC = $(XBASEDIR)/usr/bin/X11/bdftosnf
#else /* FONTS_TO_PCF_ENV */
        XFC = $(XBASEDIR)/usr/bin/X11/bdftopcf
#endif /* FONTS_TO_PCF_ENV */
#endif /* RELEASE2_ENV */
        XMKFONTDIR = $(XBASEDIR)/usr/bin/X11/mkfontdir
        MACH = and

        TOP = TOPDIR
        AS = as
        CPP = /lib/cpp
        LD = ld
        LINT = lint
#ifdef BUILDANDREWINSTALL_ENV
        INSTALL = $(BASEDIR)/etc/install
#else /* BUILDANDREWINSTALL_ENV */
        INSTALL = install
#endif /* BUILDANDREWINSTALL_ENV */
        TAGS = ctags
        RM = rm -f
        MV = mv
        CP = cp
        LN = ln -s
        RANLIB = ranlib
        AR = ar clq
        ARDEL = ar d
        CHMODW = chmod +w
        LS = ls
        AWK = awk
        SORT = sort
        TR = tr
        NM = nm
        MAKE = make
        SED = sed
        LEX = lex
        YACC = yacc
        SCRIBE = scribe
        LINTOPTS = -axz
        LINTLIBFLAG = -C
        STD_DEFINES =

/**/#
/**/# End of what comes from the allsys.mcr file.
/**/#
