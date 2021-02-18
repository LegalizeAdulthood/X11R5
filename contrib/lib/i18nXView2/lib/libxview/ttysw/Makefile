#
#	@(#)Makefile	90/08/02 50.1 SMI
#
WHOAMI=		ttysw
HDRSPUBLIC=	ttysw.h        tty.h         termsw.h
HDRSPRIVATE=	charimage.h    charscreen.h  ttyansi.h       tty_impl.h   \
		term_impl.h
CFILESLIB=	cim_change.c   cim_size.c    csr_change.c    csr_init.c   \
		ttyansi.c      tty_gtty.c    tty_init.c      tty_ntfy.c   \
		tty_stty.c     ttytl.c       ttyselect.c     tty_main.c   \
		tty_menu.c     tty_mapkey.c  tty.c           tty_modes.c  \
		tty_es.c       tty_data.c    term_ntfy.c     tty_compat.c \
		termsw.c       term_data.c   tty_newtxt.c    tty_cb.c
SPCFILESLIB=	ttyansi.c      tty_menu.c    csr_change.c    tty_data.c   \
		ttyselect.c    tty_init.c    tty_main.c      tty_mapkey.c 
NO_XSTR_FILES=	tty_data.c     term_data.c

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


