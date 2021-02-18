#
#	@(#)Makefile 50.1 90/12/12
#
WHOAMI=		base
HDRSPUBLIC=	base.h generic.h pkg_public.h pkg.h xview_xvin.h \
		drawable.h xv_error.h 
HDRSPRIVATE=	xv_debug.h xv_list.h draw_impl.h 
CFILESLIB=	xv.c xv_init.c xv_init_x.c generic.c drawable.c \
		xv_debug.c xv_parse.c xv_usage.c xv_list.c xv_error.c base_data.c
SPCFILESLIB=	xv_parse.c xv_usage.c generic.c drawable.c sv.c xv_debug.c \
		base_data.c
NO_XSTR_FILES=	base_data.c xv_usage.c

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
