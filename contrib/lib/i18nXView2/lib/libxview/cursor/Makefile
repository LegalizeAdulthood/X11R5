#
#	@(#)Makefile	90/02/26 20.11 SMI
#
WHOAMI=		cursor
HDRSPUBLIC=	cursor.h
HDRSPRIVATE=	curs_impl.h
CFILESLIB=	cursor.c curs_pri.c curs_cmpat.c curs_data.c
SPCFILESLIB=	cursor.c curs_pri.c curs_cmpat.c curs_data.c
NO_XSTR_FILES=  curs_data.c



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
