#
#	@(#)Makefile  89/12/03 1.1 SMI
#
WHOAMI=		color
HDRSPUBLIC=	cms.h
HDRSPRIVATE=	cms_impl.h 
CFILESLIB=	cms.c cms_data.c cms_pblc.c
SPCFILESLIB=	cms_data.c
NO_XSTR_FILES=  cms_data.c

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
