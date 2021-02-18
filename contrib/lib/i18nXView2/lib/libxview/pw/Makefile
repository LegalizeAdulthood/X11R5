#
#	@(#)Makefile  90/02/26 20.18 SMI
#
WHOAMI=		pw
HDRSPUBLIC=	pixwin.h
HDRSPRIVATE= 	pw_impl.h xv_color.h
CFILESLIB=	pw_btchrop.c pw_cms.c  \
		pw_get.c pw_line.c pw_plygon2.c pw_plyline.c \
		pw_plypt.c pw_put.c pw_read.c \
		pw_traprop.c pw_vector.c xv_olgx.c xv_text.c  \
		xv_stencil.c xv_rop.c mem.c mem_ops.c
SPCFILESLIB=	xv_rop.c  xv_stencil.c pw_cms.c

OBJS=		$(CFILESLIB:%.c=$(VARIANT)/%.o)

TAGFILESALL=	$(CFILESLIB:%=$(FULLPATH)/%) \
		$(HDRSPUBLIC:%=$(FULLPATH)/%) \
		$(HDRSPRIVATE:%=$(FULLPATH)/%) 

all:		xall

include ../Makefile.master

.KEEP_STATE:


xall:		$$(LIBS)
$(LIBS):	$$(VARIANT) $(HDRSPUBLIC) $(HDRSPRIVATE) $(SFILES) $$(OBJS)

clean:		master.clean
