# 
# @(#)Makefile 1.19 90/03/22
#
WHOAMI=		openwin
HDRSPUBLIC=	openwin.h
HDRSPRIVATE=	ow_impl.h
CFILESLIB=	openwin.c ow_evt.c ow_get.c ow_paint.c ow_resize.c ow_set.c \
		ow_view.c ow_rescale.c ow_data.c
# ow_seln.c - this was removed, because it contained only ifdef'd out functions

SPCFILESLIB=	openwin.c ow_data.c
NO_XSTR_FILES=  ow_data.c

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
