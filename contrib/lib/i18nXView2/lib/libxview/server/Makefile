#
#	@(#)Makefile  90/02/26 20.11 SMI
#
WHOAMI=		server
HDRSPUBLIC=	server.h
HDRSPRIVATE=	svr_impl.h svr_atom.h
CFILESLIB=	server.c svr_x.c svr_get.c svr_set.c svr_data.c
SPCFILESLIB=	server.c svr_data.c
NO_XSTR_FILES=	svr_data.c

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
