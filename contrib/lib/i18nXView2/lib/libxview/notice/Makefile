# 
# @(#)Makefile 20.11 90/02/26
#
WHOAMI=		notice
HDRSPUBLIC=	notice.h alert.h
HDRSPRIVATE=	noticeimpl.h
CFILESLIB=	notice.c
SPCFILESLIB=	notice.c

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
