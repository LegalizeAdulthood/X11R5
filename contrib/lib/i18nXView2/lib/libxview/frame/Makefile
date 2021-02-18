#
# @(#)Makefile 20.19 90/02/26 SMI
#
WHOAMI=		frame
HDRSPUBLIC=	frame.h
HDRSPRIVATE=	fm_impl.h \
		frame_base.h frame_cmd.h frame_help.h
		
CFILESLIB=	frame.c frame_init.c fm_get.c fm_set.c fm_destroy.c \
		fm_layout.c  fm_cmdline.c \
		fm_display.c fm_win.c \
		fm_geom.c fm_input.c fm_bdry.c frame_sw.c \
		fm_rescale.c \
		frame_base.c fmbs_set.c fmbs_get.c fmbs_dsty.c \
		frame_cmd.c fmcmd_set.c fmcmd_get.c fmcmd_dsty.c \
		frame_help.c fmhlp_set.c fmhlp_get.c \
		fmhlp_dsty.c fm_data.c

SPCFILESLIB=    frame.c frame_init.c fm_display.c fm_set.c fm_help.c \
		frame_cmd.c frame_base.c fm_data.c
NO_XSTR_FILES=	fm_data.c

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
