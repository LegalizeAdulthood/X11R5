/* CMU-local overrides to the distributed system.mcr file.  The distributed version */
/*   of this file is empty, and patches are never sent for it. */

        CDEBUGFLAGS = -g
        MAKEDODEBUG = -g

#if 0
#ifdef AFS_ENV
#ifdef sys_pmax_3
        AFSBASEDIR = /usr/mylocal
#else
        AFSBASEDIR = /usr/local
#endif /*sys_pmax_3 */
#endif /* AFS_ENV */
#endif /* 0 */

#ifdef AFS_ENV
        AFSBASEDIR = /usr/local
#endif /*AFS_ENV */

        ITCDIR = /usr/itc/released
        XMKFONTDIR = /usr/local/bin/mkfontdir
        XFC = /usr/local/bin/bdftosnf

#if 0
#ifdef sys_pmax_3
        XLIBDIR = /usr/mylocal/lib
        XINCDIR = /usr/mylocal/include/X11
#else
        XLIBDIR = /usr/local/lib
        XINCDIR = /usr/local/include/X11
#endif
#endif /* 0 */

        XLIBDIR = /usr/local/lib
        XINCDIR = /usr/local/include/X11

#if defined (RESOLVER_ENV) && !defined (_IBMR2) 
        RESOLVLIB = ${AFSBASEDIR}/lib/res/libresolv.a
        RESINC = -I$(AFSBASEDIR)/include/res
#endif

#if 0
#ifdef sys_pmax_3
        MIPSLIB_G0 =  /afs/andrew/system/beta/pmax_3/andrew/mips_G0/lib
        MIPSINC_G0 =  /afs/andrew/system/beta/pmax_3/andrew/mips_G0/include
        MIPSLIBC_G0 = ${MIPSLIB_G0}/libc
        RESOLVLIB = ${MIPSLIB_G0}/res/libresolv.a
        RESINC = -I/usr/local/include/res
#endif
#endif /* 0 */

        INCLUDES =  -I${BASEDIR}/include -I$(BASEDIR)/include/atk $(RESINC) -I$(AFSBASEDIR)/include -I${XINCDIR}


        LOCALDIR = DEFAULT_LOCALDIR_ENV
        COMMON = /afs/andrew.cmu.edu/itc/common
        COMMONLOCAL = $(COMMON)

