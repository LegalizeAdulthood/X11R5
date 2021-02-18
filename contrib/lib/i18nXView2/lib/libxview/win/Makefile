#
#	@(#)Makefile	90/02/26 20.22 SMI
#
WHOAMI=		win
HDRSPUBLIC=	win_enum.h win_env.h win_event.h win_input.h win_notify.h \
		win_screen.h  win_struct.h
HDRSPRIVATE=	win_info.h    win_keymap.h   win_kmdata.h win_ascii.h
CFILESLIB=	win_cursor.c  win_env.c	     win_geom.c   \
		win_global.c  win_cntral.c   win_input.c  \
		win_treeop.c  win_bell.c     win_damage.c \
		win_compat.c  win_keymap.c
SPCFILESLIB=	win_input.c   win_ttyenv.c

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
