#
#	@(#)Makefile	20.10	90/02/26 SMI
#
WHOAMI=		icon
HDRSPUBLIC=	icon.h icon_load.h 
HDRSPRIVATE=	icon_impl.h
CFILESLIB= 	icon.c icon_obj.c icon_load.c icon_input.c icon_data.c
SPCFILESLIB=	icon_obj.c icon_data.c
NO_XSTR_FILES=  icon_data.c

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

