#
#	@(#)Makefile 20.11 90/02/26 SMI
#
WHOAMI=		wmgr
HDRSPUBLIC=	wmgr.h 
HDRSPRIVATE=	wmgr_decor.h
CFILESLIB= 	wmgr_rect.c wmgr_state.c wmgr_menu.c wmgr_decor.c
SPCFILESLIB=	wmgr_menu.c

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
