#
#	@(#)Makefile	90/02/26 20.8
#
WHOAMI=		io_stream
HDRSPUBLIC=	
HDRSPRIVATE=	io_stream.h
CFILESLIB=	input.c output.c io.c \
		str_strms.c file_strms.c othr_strms.c
SPCFILESLIB=	str_strms.c file_strms.c othr_strms.c

OBJS=		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%) 

all:		xall

include ../Makefile.master

.KEEP_STATE:

$(SPCFILESLIB:%.c=obj/%.o) \
		$(SPCFILESLIB:%.c=obj_p/%.o) \
		$(SPCFILESLIB:%.c=shared/%.o)  :=  ROI_DATA =

xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $$(OBJS)

clean:		master.clean
