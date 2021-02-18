#
# @(#)Makefile 50.1 90/07/13 SMI
#
WHOAMI=		canvas
HDRSPUBLIC=	canvas.h
HDRSPRIVATE=	cnvs_impl.h
CFILESLIB= 	canvas.c      cnvs_set.c    cnvs_get.c    cnvs_scrol.c \
		cnvs_resze.c  cnvs_input.c  cnvs_view.c   cnvs_data.c  \
		cnvs_callback.c
SPCFILESLIB=	canvas.c      cnvs_view.c   cnvs_data.c
NO_XSTR_FILES=  cnvs_data.c

OBJS =		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%) 

all:		xall

include ../Makefile.master

.KEEP_STATE:


xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)

clean:		master.clean

