#
#	@(#)Makefile  90/02/26 20.12 SMI
#
WHOAMI=		cb
HDRSPUBLIC=	
HDRSPRIVATE=	lookupchoice.h aux.h
CFILESLIB=      luc.c 		stat_imcb.c  aux.c
SPCFILESLIB=	luc.c		stat_imcb.c  aux.c
NO_XSTR_FILES=  

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
