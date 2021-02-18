#
# @(#)Makefile 1.7 89/03/08
#
WHOAMI=		misc
HDRSPUBLIC=	attrol.h xview.h
HDRSPRIVATE=
CFILESLIB=	windowloop.c xv_deaf.c xv_version.c
SPCFILESLIB=   

NO_XSTR_FILES=	xv_version.c

OBJS =		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%)

all: 

include ../Makefile.master

.KEEP_STATE:

xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)

clean:		master.clean
