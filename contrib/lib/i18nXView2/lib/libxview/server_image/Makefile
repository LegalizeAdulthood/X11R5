#
#	@(#)Makefile  90/02/26 20.11 SMI
#
WHOAMI=		server_image
HDRSPUBLIC=	svrimage.h
HDRSPRIVATE=	svrim_impl.h
CFILESLIB=      svrim_ops.c svrim_pblc.c svrim_data.c
SPCFILESLIB=	svrim_pblc.c svrim_data.c
NO_XSTR_FILES=  svrim_data.c

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
