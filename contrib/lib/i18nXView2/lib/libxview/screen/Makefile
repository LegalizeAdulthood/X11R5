#
#	@(#)Makefile  90/02/26 20.13 SMI
#
WHOAMI=		screen
HDRSPUBLIC=	screen.h
HDRSPRIVATE=	scrn_impl.h scrn_vis.h
CFILESLIB=	screen.c scrn_get.c scrn_lyout.c scrn_vis.c scrn_data.c
SPCFILESLIB=	screen.c scrn_data.c
NO_XSTR_FILES=  scrn_data.c

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
