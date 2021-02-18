#
#	@(#)Makefile  90/04/19 1.19 SMI
#
WHOAMI=		scrollbar
HDRSPUBLIC=	scrollbar.h 
HDRSPRIVATE=	sb_impl.h
CFILESLIB= 	sb_compat.c sb_destroy.c sb_get.c sb_pos.c  \
		sb_set.c sb_event.c sb_paint.c \
		sb_scroll.c sb.c sb_data.c
NO_XSTR_FILES=  sb_data.c
SPCFILESLIB=	sb.c sb_paint.c sb_data.c

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
