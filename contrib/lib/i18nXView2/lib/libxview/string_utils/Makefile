#
#	@(#)Makefile	90/02/26 20.9 SMI
#
WHOAMI=		string_utils
HDRSPUBLIC=	str_utils.h
HDRSPRIVATE=
CFILESLIB=	str_utils.c whitespace.c

OBJS=		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%) 
		
all:		xall

include ../Makefile.master

.KEEP_STATE:

xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)

clean:		 master.clean
