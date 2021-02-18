#
#	@(#)Makefile 20.13 90/02/26
#
WHOAMI=		menu
HDRSPUBLIC=	openmenu.h 
HDRSPRIVATE=	omi_impl.h om_impl.h
CFILESLIB= 	omi.c om_public.c om_render.c \
		om_set.c om_get.c om_compat.c \
		om_data.c
SPCFILESLIB=	om_public.c om_set.c om_data.c \
		om_render.c om_data.c
NO_XSTR_FILES=	om_data.c

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
