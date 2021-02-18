#
#	@(#)Makefile	90/02/26 20.9 SMI
#
WHOAMI=		rect
HDRSPUBLIC=	rect.h rectlist.h xv_xrect.h
CFILESLIB=	rect.c rectlist.c rect_util.c rect_data.c
SPCFILESLIB=	rect_data.c
NO_XSTR_FILES=	rect_data.c

OBJS=		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) 

all:		xall

include ../Makefile.master

.KEEP_STATE:

$(SPCFILESLIB:%.c=obj/%.o) \
		$(SPCFILESLIB:%.c=obj_p/%.o) \
		$(SPCFILESLIB:%.c=shared/%.o)  :=  ROI_DATA =

$(NO_XSTR_FILES:%.c=obj/%.o) \
		$(NO_XSTR_FILES:%.c=obj_p/%.o) \
		$(NO_XSTR_FILES:%.c=shared/%.o)  :=  NO_XSTR = ON

xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $$(OBJS)

clean:		master.clean
