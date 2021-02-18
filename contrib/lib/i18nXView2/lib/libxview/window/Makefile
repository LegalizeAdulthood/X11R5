#
#	@(#)Makefile  90/12/12 50.1 SMI
#
WHOAMI=		window
HDRSPUBLIC=	window.h
HDRSPRIVATE=	windowimpl.h
CFILESLIB=      window.c      window_get.c   window_set.c  windowlayt.c   \
		windowutil.c  window_cms.c  windowcmpt.c   \
		windowdata.c
SPCFILESLIB=	window.c      window_set.c
NO_XSTR_FILES=  windowdata.c

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

