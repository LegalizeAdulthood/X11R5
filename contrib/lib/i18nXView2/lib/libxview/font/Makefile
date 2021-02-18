#
#	@(#)Makefile 50.1 90/12/12 SMI
#
WHOAMI=		font
HDRSPUBLIC=	font.h
HDRSPRIVATE=	font_impl.h
CFILESLIB=	font.c font_set.c font_get.c font_x.c \
		pf_text.c pf_ttext.c pf.c font_data.c
SPCFILESLIB=	pf.c font.c font_data.c
NO_XSTR_FILES=  font_data.c

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
