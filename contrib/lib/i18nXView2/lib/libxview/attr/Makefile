#
#	@(#)Makefile	90/12/12 50.1 SMI
#
WHOAMI=		attr
HDRSPUBLIC=	attr.h
HDRSPRIVATE=	attr_impl.h
CFILESLIB=	attr.c attr_copy.c attr_cu.c attr_util.c
SPCFILESLIB=	attr.c

OBJS=		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%) 

all: xall

include ../Makefile.master

.KEEP_STATE:

$(SPCFILESLIB:%.c=obj/%.o) \
                $(SPCFILESLIB:%.c=obj_p/%.o) \
                $(SPCFILESLIB:%.c=shared/%.o)  :=  ROI_DATA =

xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)

clean:		master.clean
