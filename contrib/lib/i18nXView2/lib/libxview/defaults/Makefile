#
#	@(#)Makefile  90/02/26 20.15 SMI
#
WHOAMI=		defaults
HDRSPUBLIC=	defaults.h
HDRSPRIVATE=
CFILESLIB=	defaults.c
SPCFILESLIB=	defaults.c

OBJS=		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%) 

all:		xall

include ../Makefile.master

.KEEP_STATE:

xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)


clean:		master.clean
