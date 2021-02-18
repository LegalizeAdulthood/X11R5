#
#       @(#)Makefile  88/02/17 20.1 SMI
#
WHOAMI=		fullscreen
HDRSPUBLIC=	fullscreen.h
HDRSPRIVATE=	fs_impl.h
CFILESLIB=	fs.c fs_set.c fs_get.c fs_compat.c fs_data.c
SPCFILESLIB=	fs_compat.c fs.c fs_data.c
NO_XSTR_FILES=  fs_data.c

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
