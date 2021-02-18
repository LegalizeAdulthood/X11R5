#
# @(#)Makefile 1.11 90/02/26
#
WHOAMI=		help
HDRSPUBLIC=
HDRSPRIVATE=
CFILESLIB=	help.c help_file.c
SPCFILESLIB=    help.c

OBJS=		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%)

all:		xall

include ../Makefile.master

.KEEP_STATE:

xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)

clean:		master.clean
