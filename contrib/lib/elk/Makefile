# Begin of user-configurable section:

# List of directories where to invoke "make".  See README for an overview.

TARGETS= src\
	 lib\
	 lib/util\
	 lib/xlib\
	 lib/xt\
	 lib/xaw\
	 lib/xm\
	 lib/xm/xt

# Top level directory (where the runtime system will be installed).
# You can set TOP_DIR to the directory where this Makefile resides, i.e.
# where you have unpacked the distribution.
#
# Scheme files are loaded from SCM_DIR, object files from LIB_DIR.

TOP_DIR= /usr/elk/1.4
SCM_DIR= $(TOP_DIR)/scm
LIB_DIR= $(TOP_DIR)/lib

# GENERIC pointer type.  Should be void*, but some C compilers (such as
# cc under 4.3BSD) can't handle void* correctly.

GENERIC= \"char*\"

# MACHTYPE is used to select a machine-specific version of the assembly
# files src/stack.s and src/alloca.s.  It is used as a filename suffix,
# i.e. src/stack.s.$(MACHTYPE) will be used by the Makefile in src.
#
# Set MACHTYPE to "port" if you want to make the portable version.
# In this case, make sure that PORTABLE is defined in src/config.h.
#
# Permitted values for MACHTYPE currently are 68k, vax, 386, sparc,
# symmetry, rt, sony68k, mips, hp9000s300, and port.

MACHTYPE= 68k

# If symbolic links are not supported by your system, define LINK=ln

LINK=ln -s

# Compiler options

 CFLAGS= -O                            # generic CFLAGS
#CFLAGS= -O -finline-functions         # gcc
#CFLAGS= -g -ma                        # IBM RT
#CFLAGS= +O1 -Wl,-x -Wc,-Ns4000        # HP-UX
#CFLAGS=                               # System V/386
#CFLAGS= -O -finline-functions -bsd    # NeXT

# Linker options.  On the NeXT, -u libsys replace -lm.  Everywhere else
# it must be present.

#LDFLAGS= -lm -x                                       # generic LDFLAGS
#LDFLAGS= -lm                                          # IBM RT, HP-UX
 LDFLAGS= -lm -Bstatic                                 # SunOS
#LDFLAGS= -lm -x -lld                                  # System V
#LDFLAGS= -lm -Wl,-D,800000 /usr/local/lib/gcc-gnulib  # MIPS (gcc)
#LDFLAGS= -x -u libsys_s -seglinkedit                  # NeXT

# Sun has broken their linker under SunOS 4.1; the -l option doesn't work
# together with -r any longer.  Thus the full library paths must be given.
# These symbols are only needed when you want to make the X11 interface.
#
# SunOS 4.1:
 LIBROOT= /lib
 XLIB=    $(LIBROOT)/libX11.a
 XT=      $(LIBROOT)/libXt.a
 XAW=     $(LIBROOT)/libXaw.a $(LIBROOT)/libXmu.a $(XT)
 MOTIF=   $(LIBROOT)/libXm.a
#
# Everywhere else:
#XLIB=    -lX11
#XT=      -lXt
#XAW=     -lXaw -lXmu $(XT)
#MOTIF=   -lXm

# End of user-configurable section


DEFS= -DSCM_DIR=\\\"$(SCM_DIR)\\\" -DLIB_DIR=\\\"$(LIB_DIR)\\\"\
      -DGENERIC=$(GENERIC)

LINTFLAGS= $(DEFS)

all:
	-@for i in $(TARGETS) ;\
	do \
	    ( if [ ! -d $$i ]; then \
		echo mkdir $$i ;\
		mkdir $$i ;\
	      fi ;\
	      if [ ! -r $$i/Makefile ]; then \
		echo $(LINK) $(TOP_DIR)/$$i/Makefile $$i ;\
		$(LINK) $(TOP_DIR)/$$i/Makefile $$i ;\
	    fi ) ;\
	done
	@for i in $(TARGETS) ;\
	do \
	    ( cd $$i ;\
	    echo Making $$i ;\
	    make "MACHTYPE=$(MACHTYPE)" "CC=$(CC)" "CFLAGS=$(DEFS) $(CFLAGS)"\
		 "LDFLAGS=$(LDFLAGS)" "LINK=$(LINK)" "XLIB=$(XLIB)" "XT=$(XT)"\
		 "XAW=$(XAW)" "MOTIF=$(MOTIF)" ) ;\
	done

lint:
	@for i in $(TARGETS) ;\
	do \
	    ( cd $$i ;\
	    echo Linting $$i ;\
	    make "LINTFLAGS=$(LINTFLAGS)" lint );\
	done

clean:
	@for i in $(TARGETS) ;\
	do \
	    ( cd $$i ;\
	    echo Cleaning $$i ;\
	    make clean ) ;\
	done

dist:
	gtar -cvf scheme.tar -X ExcludeFiles RELEASE README INSTALL ORIGIN\
	    MACHINES Makefile doc src scm examples lib stk
	compress scheme.tar
	rsh kraftbus uuencode scheme.tar.Z <scheme.tar.Z >scheme.tar.Z.uue
	rm scheme.tar.Z
	split -1000 scheme.tar.Z.uue
