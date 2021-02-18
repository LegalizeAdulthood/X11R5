#
#	@(#)Makefile	90/12/12 50.1 SMI
#
WHOAMI=		sel
HDRSPUBLIC=	selection.h sel_attrs.h sel_svc.h seln.h sel_compat.h
HDRSPRIVATE=	sel_impl.h
CFILESLIB= 	selection.c sel_appl.c sel_clnt.c sel_common.c  \
		sel_policy.c sel_agent.c sel_compat.c sel_data.c

NO_XSTR_FILES=  sel_data.c

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
